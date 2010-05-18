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
#include "servo.h"

#include <avr/interrupt.h>

static volatile unsigned int position[SERVO_MAX];

ISR(TIMER0_COMP_vect)
{
	static unsigned int nr_0125ms = 0;

	nr_0125ms++;
	if (nr_0125ms == 1600) {
		// Start of the 20ms cycle, set pins high
		if (position[0]) PORTB |= _BV(PB3);
		if (position[0]) PORTD |= _BV(PD5);
		if (position[0]) PORTD |= _BV(PD4);
		if (position[0]) PORTD |= _BV(PD7);
		nr_0125ms = 0;
	} else {
		// When their time slot ends, set pins lows
		if (nr_0125ms >= position[0]) PORTB &= ~(_BV(PB3));
		if (nr_0125ms >= position[1]) PORTD &= ~(_BV(PD5));
		if (nr_0125ms >= position[2]) PORTD &= ~(_BV(PD4));
		if (nr_0125ms >= position[3]) PORTD &= ~(_BV(PD7));
	}
}

void
servo_configure(void)
{
	TCCR0 = _BV(WGM01) | _BV(CS00);
	OCR0 = 100;

	PORTB &= ~(_BV(PB3));
	DDRB |= _BV(PB3);
	PORTD &= ~(_BV(PD5) | _BV(PD4) | _BV(PD7));
	DDRD |= _BV(PD5) | _BV(PD4) | _BV(PD7);

	TIMSK |= _BV(OCIE0);
}

void
servo_set(unsigned char pin, unsigned int pos)
{
	if (pin == 0) return;
	pin--;
	if (pin > SERVO_MAX) return;

	position[pin] = pos;
}
