/*
 * inci driver
 * Copyright (C) 2008 i Bilisim Teknolojileri Ltd. Sti.
 *
 */

#include <unistd.h>
#include <stdlib.h>

#include "io.h"

static unsigned char
ibutton_crc(const unsigned char *buf, size_t len)
{
	unsigned char crc = 0;

	for (unsigned int pos = 0; pos < len; ++pos) {
		crc = crc ^ buf[pos];
		for (int i = 0; i < 8; i++) {
			if (crc & 0x01)
				crc = (crc >> 1) ^ 0x8C;
			else
				crc >>= 1;
		}
	}
	return crc;
}


iO::iO()
{
	inpos = 0;

	// Opening serial port activates iO
	port = new SeriPort("/dev/ttyS0", 38400);
	if (!port->is_ok) { errno = -1; return; }

	// Give iO some time to setup UART
	usleep(500000); // half a second

	handshake();
	if (errno) return;

	errno = 0;
}

iO::~iO()
{
	delete port;
}

int
iO::reconnect(void)
{
	const char override[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

	// Send max command size of zeroes, this puts firmware back
	// to the command state.
	port->write(override, sizeof(override) - 1);
	// Flush the input buffer, it may contain previous garbage
	inpos = 0;
	port->flush_input();
	// Handshake again
	handshake();

	return errno == 0;
}

void
iO::set_dio(unsigned int mask, unsigned int state)
{
	outbuf[2] = mask & 0xff;
	outbuf[3] = (mask & 0xff00) >> 8;
	outbuf[4] = (mask & 0xff0000) >> 16;
	outbuf[5] = (mask & 0xff000000) >> 24;
	outbuf[6] = state & 0xff;
	outbuf[7] = (state & 0xff00) >> 8;
	outbuf[8] = (state & 0xff0000) >> 16;
	outbuf[9] = (state & 0xff000000) >> 24;

	send('o', 8);
}

void
iO::set_servo(int servo)
{
	outbuf[2] = 0x01;
	outbuf[3] = servo & 0xff;
	outbuf[4] = (servo & 0xff00) >> 8;
	send('s', 3);
}

void
iO::set_motor_pwm(int fl, int fr, int rear)
{
	unsigned int mask;
	unsigned int state = 0;

	mask = (1 << 13) | (1 << 14) | (1 << 21) | (1 << 22) | (1 << 23) | (1 << 24);

	if (fl >= 0) state |= (1 << 14); else state |= (1 << 13);
	if (fr >= 0) state |= (1 << 22); else state |= (1 << 21);
	if (rear >= 0) state |= (1 << 24); else state |= (1 << 23);
	set_dio(mask, state);

	outbuf[2] = 0x01 | 0x02 | 0x04;
	outbuf[3] = abs(fl);
	outbuf[4] = abs(fr);
	outbuf[5] = abs(rear);
	send('p', 4);
}

void
iO::read_report(struct iO_data &data)
{
	send('r', 0);
inpos = 0;
	while (0 == recv())
		;
inpos = 0;
	if (inbuf[1] != 'd') return;
std::cout << "got it" << std::endl;
	data.encoder[0] = (inbuf[2] << 8) + inbuf[3];
	data.encoder[1] = (inbuf[4] << 8) + inbuf[5];
	data.encoder[2] = (inbuf[6] << 8) + inbuf[7];

	data.adc[0] = inbuf[8 + 0] + ((inbuf[8 + 4] & 0xc0) << 2);
	data.adc[1] = inbuf[8 + 1] + ((inbuf[8 + 4] & 0x30) << 4);
	data.adc[2] = inbuf[8 + 2] + ((inbuf[8 + 4] & 0x0c) << 6);
	data.adc[3] = inbuf[8 + 3] + ((inbuf[8 + 4] & 0x03) << 8);
	data.adc[4] = inbuf[8 + 5] + ((inbuf[8 + 9] & 0xc0) << 2);
	data.adc[5] = inbuf[8 + 6] + ((inbuf[8 + 9] & 0x30) << 4);
	data.adc[6] = inbuf[8 + 7] + ((inbuf[8 + 9] & 0x0c) << 6);
	data.adc[7] = inbuf[8 + 8] + ((inbuf[8 + 9] & 0x03) << 8);
	data.adc[8] = inbuf[8 + 10] + (inbuf[8 + 11] << 8);

	data.dio = inbuf[20]
		+ (inbuf[21] << 8)
		+ (inbuf[22] << 16)
		+ (inbuf[23] << 24);
}

void
iO::handshake(void)
{
	send('v', 0);
	while (0 == recv())
		;
	if (inbuf[1] != 'i' || inbuf[2] != 'O') {
		errno = -5;
		return;
	}
	version = inbuf[3];
	revision = inbuf[4];
}

int
iO::recv()
{
	int ret;
	int need;

	if (inpos < 2) need = 2 - inpos;
	else {
		if (inbuf[0] != 'R') {
			errno = -5;
			return 0;
		}

		if (inbuf[1] == 'i') need = 6 - inpos;
		else if (inbuf[1] == 'd') need = 25 - inpos;
		else {
			errno = -5;
			return 0;
		}
	}

	ret = port->read((char *) inbuf + inpos, need);
	if (ret == -1) {
		errno = -5;
		return 0;
	}
	inpos += ret;

	if (inpos > 2 && ret == need) {
		if (ibutton_crc(&inbuf[1], inpos - 2) != inbuf[inpos-1]) {
			errno = -5;
			return 0;
		}
		return 1;
	}

	return 0;
}

int
iO::send(char cmd, size_t len)
{
	outbuf[0] = 'K';
	outbuf[1] = cmd;
	outbuf[len + 2] = ibutton_crc(&outbuf[1], len + 1);

	port->write((char *) outbuf, len + 3);

	return 0;
}
