/*
 * iO Firmware
 * Copyright (C) 2008 i Bilişim Teknolojileri Ltd. Şti.
 * http://www.ibilisim.com.tr
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of GNU General Public License.
 *
 */

#include "common.h"

#define UART_BAUD_RATE 38400

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <util/crc16.h>

#include "calibration.h"
#include "uart.h"
#include "pwm.h"
#include "servo.h"
#include "adc.h"
#include "dio.h"
#include "i2c.h"

/* Incoming command data is stored here. Make sure it is bigger than */
/* the maximum of the individual incoming command data sizes. */
static unsigned char payload[144];

static void
send_reply(char cmd, const unsigned char *buf, unsigned char size)
{
	unsigned char crc;

	uart_putc('R');
	crc = _crc_ibutton_update(0, cmd);
	uart_putc(cmd);
	while (size) {
		crc = _crc_ibutton_update(crc, buf[0]);
		uart_putc(buf[0]);
		size--;
		buf++;
	}
	uart_putc(crc);
}

/*
** Commands
*/

static void
cmd_version(void)
{
// Arguments: none
// Returns: 'i' 'O' <version> <revision>
	unsigned char buf[3];

	buf[0] = 'O';
	buf[1] = iO_VERSION;
	buf[2] = iO_REVISION;

	send_reply('i', buf, 3);
}

static void
cmd_configure_pwm(void)
{
// Arguments: <pin> <prescaler> <mode> <toph> <topl> <valueh> <valuel>
// Returns: none
	pwm_configure(payload[0], payload[1], payload[2],
		(payload[3] << 8) + payload[4]);
	pwm_set(payload[0], (payload[5] << 8) + payload[6]);
}

static void
cmd_set_pwm(void)
{
// Arguments: <pin> <valueh> <valuel>
// Returns: none
	pwm_set(payload[0], (payload[1] << 8) + payload[2]);
}

static void
cmd_configure_servo(void)
{
// Arguments: none yet
// Returns: none
	servo_configure();
}

static void
cmd_set_servo(void)
{
// Arguments: <pin> <valueh> <valuel>
// Returns: none
	servo_set(payload[0], (payload[1] << 8) + payload[2]);
}

static void
cmd_configure_adc(void)
{
// Arguments: <pinmask>
// Returns: none
	adc_configure(payload[0]);
}

static void
cmd_set_dio(void)
{
// Arguments: <pin> <mode>
// Returns: none
	dio_set(payload[0], payload[1]);
}

static void
cmd_report(void)
{
// Arguments: none
// Returns: <dio>*4
	unsigned char buf[20];

	dio_read(&buf[0]);
	adc_read(&buf[4]);

	send_reply('d', buf, 20);
}

static void
cmd_led(void)
{
	if (payload[0] & 0x10) {
		if (payload[0] & 0x01) {
			PORTB &= ~0x01;
			DDRB |= 0x01;
		} else {
			DDRB &= ~0x01;
		}
	}
	if (payload[0] & 0x20) {
		if (payload[0] & 0x02) {
			PORTB &= ~0x02;
			DDRB |= 0x02;
		} else {
			DDRB &= ~0x02;
		}
	}
}

static void
cmd_i2c(void)
{
	unsigned char i;
	unsigned char len;

	// Address should be valid
	if (payload[0] & 0x80) return;

	i2c_start_writing(payload[0]);

	len = payload[1];
	if (len > 141) len = 141;

	for (i = 0; i<len; i++) {
		i2c_write(payload[2+i]);
	}

	i2c_stop();
}

// NOTE: implement new commands here


static void
parse_cmd(unsigned char c, unsigned char *needp, void (**handlerp)(void))
{
	*needp = 0;
	*handlerp = NULL;

	if (c == 'v') {
		*needp = 0;
		*handlerp = cmd_version;
	} else if (c == 'P') {
		*needp = 7;
		*handlerp = cmd_configure_pwm;
	} else if (c == 'p') {
		*needp = 3;
		*handlerp = cmd_set_pwm;
	} else if (c == 'S') {
		*needp = 0;
		*handlerp = cmd_configure_servo;
	} else if (c == 's') {
		*needp = 3;
		*handlerp = cmd_set_servo;
	} else if (c == 'A') {
		*needp = 1;
		*handlerp = cmd_configure_adc;
	} else if (c == 'd') {
		*needp = 2;
		*handlerp = cmd_set_dio;
	} else if (c == 'r') {
		*needp = 0;
		*handlerp = cmd_report;
	} else if (c == 'l') {
		*needp = 1;
		*handlerp = cmd_led;
	} else if (c == 'i') {
		*needp = 144;
		*handlerp = cmd_i2c;
	}
	// NOTE: register new commands here
}

/*
** Main loop
*/

// Command parser machine states
enum {
	C_START,
	C_COMMAND,
	C_DATA,
	C_CRC
};

int
main(void)
{
	unsigned int c;
	unsigned char state = C_START;
	unsigned char pos = 0;
	unsigned char crc = 0;
	unsigned char need = 0;
	void (*handler)(void) = NULL;

	// Calibrate internal oscillator for better timing
	// RS232 definitely needs this
	calibrate_oscillator();

	// Setup hardware
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
	i2c_master_init();

	// Enable interrupts
	sei();

	// Main loop
	while (1) {
		c = uart_getc();
		if ((c & 0xff00) != 0) continue;
		c &= 0xff;

		switch (state) {
			case C_START:
				if (c == 'K') state = C_COMMAND;
				break;
			case C_COMMAND:
				crc = _crc_ibutton_update(0, c);
				parse_cmd(c, &need, &handler);
				if (handler) {
					state = C_DATA;
					if (need == 0) state = C_CRC;
					pos = 0;
				} else {
					// Unknown command
					state = C_START;
				}
				break;
			case C_DATA:
				crc = _crc_ibutton_update(crc, c);
				payload[pos++] = c;
				if (pos == need) state = C_CRC;
				break;
			case C_CRC:
				if (c == crc) handler();
				state = C_START;
				break;
		}
	}
}
