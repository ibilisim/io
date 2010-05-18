/*
 * inci driver
 * Copyright (C) 2008 i Bilisim Teknolojileri Ltd. Sti.
 *
 */

#include <fcntl.h>
#include <termios.h>
namespace unistd {
#include <unistd.h>
};

#include "seriport.h"

static speed_t
get_speed_enum(unsigned int speed)
{
	if (speed == 1200) return B1200;
	if (speed == 2400) return B2400;
	if (speed == 4800) return B4800;
	if (speed == 9600) return B9600;
	if (speed == 19200) return B19200;
	if (speed == 38400) return B38400;
	if (speed == 57600) return B57600;
	if (speed == 115200) return B115200;

	// Unknown
	return B0;
}

SeriPort::SeriPort(const char *portdev, unsigned int speed)
{
	is_ok = 0;

	fd = open(portdev, O_RDWR);
	if (fd == -1) return;

	struct termios prefs;

	if (0 != tcgetattr(fd, &prefs)) return;

	// Set speed
	speed_t st = get_speed_enum(speed);
	if (st == B0) return;
	if (0 != cfsetispeed(&prefs, st)) return;
	if (0 != cfsetospeed(&prefs, st)) return;

	// Activate receiver
	prefs.c_cflag |= (CLOCAL | CREAD);

	// Disable echo, output modification, etc
	prefs.c_lflag = 0;
	prefs.c_oflag = 0;

	// Set mode (8n1)
	prefs.c_cflag &= ~CSIZE; // clear mode bits first
	prefs.c_cflag |= CS8;
	prefs.c_cflag &= ~(PARENB | PARODD);
	prefs.c_cflag &= ~CSTOPB;

	// No flow control
	prefs.c_iflag &= ~(IXON | IXOFF | IXANY);
	prefs.c_cflag &= ~CRTSCTS;

	// Dont mess with CRLF chars
	prefs.c_iflag &= ~(IGNCR | ICRNL | INLCR);

	// Ignore parity and break
	prefs.c_iflag |= IGNPAR | IGNBRK;

	if (0 != tcsetattr(fd, TCSANOW, &prefs)) return;

	is_ok = 1;
}

SeriPort::~SeriPort()
{
	if (fd != -1) unistd::close(fd);
}

int
SeriPort::read(char *buffer, size_t len)
{
	return unistd::read(fd, (void *) buffer, len);
}

int
SeriPort::write(const char *buffer, size_t len)
{
	return unistd::write(fd, (void *) buffer, len);
}

void
SeriPort::flush_input()
{
	tcflush(fd, TCIFLUSH);
}
