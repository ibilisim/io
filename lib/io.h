/*
 * inci driver
 * Copyright (C) 2008 i Bilisim Teknolojileri Ltd. Sti.
 *
 */

#include <stddef.h>

#include "seriport.h"

struct iO_data {
	unsigned int dio;
	unsigned int encoder[3];
	unsigned int adc[9];
};

class iO {
public:
	iO();
	~iO();

	int reconnect();
	void set_dio(unsigned int mask, unsigned int state);
	// FIXME: set_servo and set_motor_pwm should be more generic here
	// inci specific stuff should be moved into inci.cc
	void set_servo(int servo);
	void set_motor_pwm(int fl, int fr, int rear);
	void read_report(struct iO_data &data);

	int errno;
	unsigned int version;
	unsigned int revision;

private:
	SeriPort *port;
	unsigned char inbuf[32];
	unsigned char outbuf[32];
	int inpos;

	void handshake(void);
	int recv();
	int send(char cmd, size_t len);
};
