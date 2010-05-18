/*
 * iO Firmware
 * Copyright (C) 2008 i Bilişim Teknolojileri Ltd. Şti.
 * http://www.ibilisim.com.tr
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of GNU General Public License.
 *
 */

enum {
	DIO_IN,
	DIO_IN_PULLUP,
	DIO_OUT_LOW,
	DIO_OUT_HIGH
};

extern void dio_set(unsigned char pin, unsigned char mode);
extern void dio_read(unsigned char *buf);
