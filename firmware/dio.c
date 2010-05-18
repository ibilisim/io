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
#include "dio.h"

void
dio_set(unsigned char pin, unsigned char mode)
{
	volatile unsigned char *port;
	volatile unsigned char *ddr;
	unsigned char bit;

	switch (pin) {
		case  0: /* pin counting starts from 1 */ return;

		/* 20 pin socket */

		case  1: /* no connection */ return;
		case  2: port = &PORTA; ddr = &DDRA; bit = _BV(PA3); break;
		case  3: port = &PORTA; ddr = &DDRA; bit = _BV(PA4); break;
		case  4: port = &PORTA; ddr = &DDRA; bit = _BV(PA5); break;
		case  5: port = &PORTA; ddr = &DDRA; bit = _BV(PA6); break;

		case  6: port = &PORTA; ddr = &DDRA; bit = _BV(PA7); break;
		case  7: port = &PORTC; ddr = &DDRC; bit = _BV(PC7); break;
		case  8: port = &PORTC; ddr = &DDRC; bit = _BV(PC6); break;
		case  9: port = &PORTC; ddr = &DDRC; bit = _BV(PC5); break;
		case 10: port = &PORTC; ddr = &DDRC; bit = _BV(PC4); break;

		case 11: port = &PORTC; ddr = &DDRC; bit = _BV(PC3); break;
		case 12: port = &PORTC; ddr = &DDRC; bit = _BV(PC2); break;
		case 13: port = &PORTD; ddr = &DDRD; bit = _BV(PD6); break;
		case 14: port = &PORTD; ddr = &DDRD; bit = _BV(PD3); break;
		case 15: port = &PORTD; ddr = &DDRD; bit = _BV(PD2); break;

		case 16: port = &PORTA; ddr = &DDRA; bit = _BV(PA2); break;
		case 17: port = &PORTA; ddr = &DDRA; bit = _BV(PA1); break;
		case 18: port = &PORTA; ddr = &DDRA; bit = _BV(PA0); break;
		case 19: port = &PORTB; ddr = &DDRB; bit = _BV(PB2); break;
		case 20: /* no connection */ return;

		/* 4 pin PWM socket as dio */

		case 21: port = &PORTB; ddr = &DDRB; bit = _BV(PB3); break;
		case 22: port = &PORTD; ddr = &DDRD; bit = _BV(PD5); break;
		case 23: port = &PORTD; ddr = &DDRD; bit = _BV(PD4); break;
		case 24: port = &PORTD; ddr = &DDRD; bit = _BV(PD7); break;

		default:
			return;
	}

	switch (mode) {
		case DIO_IN:
			*port &= (unsigned char)~bit;
			*ddr &= (unsigned char)~bit;
			break;
		case DIO_IN_PULLUP:
			*port |= bit;
			*ddr &= (unsigned char)~bit;
			break;
		case DIO_OUT_LOW:
			*port &= (unsigned char)~bit;
			*ddr |= bit;
			break;
		case DIO_OUT_HIGH:
			*port |= bit;
			*ddr |= bit;
			break;
	}
}

void
dio_read(unsigned char *buf)
{
	unsigned char a,b,c,d;

	a = PINA;
	b = PINB;
	c = PINC;
	d = PIND;

	buf[0] = 0;
	// 0x01 no connection
	if (a & _BV(PA3)) buf[0] |= 0x02;
	if (a & _BV(PA4)) buf[0] |= 0x04;
	if (a & _BV(PA5)) buf[0] |= 0x08;
	if (a & _BV(PA6)) buf[0] |= 0x10;
	if (a & _BV(PA7)) buf[0] |= 0x20;
	if (c & _BV(PC7)) buf[0] |= 0x40;
	if (c & _BV(PC6)) buf[0] |= 0x80;
	buf[1] = 0;
	if (c & _BV(PC5)) buf[1] |= 0x01;
	if (c & _BV(PC4)) buf[1] |= 0x02;
	if (c & _BV(PC3)) buf[1] |= 0x04;
	if (c & _BV(PC2)) buf[1] |= 0x08;
	if (d & _BV(PD6)) buf[1] |= 0x10;
	if (d & _BV(PD3)) buf[1] |= 0x20;
	if (d & _BV(PD2)) buf[1] |= 0x40;
	if (a & _BV(PA2)) buf[1] |= 0x80;
	buf[2] = 0;
	if (a & _BV(PA1)) buf[2] |= 0x01;
	if (a & _BV(PA0)) buf[2] |= 0x02;
	if (b & _BV(PB2)) buf[2] |= 0x04;
	// 0x08 no connection
	if (b & _BV(PB3)) buf[2] |= 0x10;
	if (d & _BV(PD5)) buf[2] |= 0x20;
	if (d & _BV(PD4)) buf[2] |= 0x40;
	if (d & _BV(PD7)) buf[2] |= 0x80;
	buf[3] = 0;
}
