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
#include "pwm.h"

static void
pin1_conf(unsigned char prescaler, unsigned char mode, unsigned char top)
{
	unsigned char t;

	switch (prescaler) {
		case PWM_PRESCALER_1024:  t = _BV(CS02) | _BV(CS00);  break;
		case PWM_PRESCALER_256:   t = _BV(CS02);              break;
		case PWM_PRESCALER_64:    t = _BV(CS01) | _BV(CS00);  break;
		case PWM_PRESCALER_8:     t = _BV(CS01);              break;
		default:                  t = _BV(CS00);              break;
	}

	switch (mode) {
		case PWM_MODE_OFF:   t |= 0; break;
		case PWM_MODE_FAST:  t |= _BV(WGM01) | _BV(WGM00) | _BV(COM01);  break;
		// FIXME: other modes
	}

	TCCR0 = t;

	DDRB |= _BV(PB3);
}

static void
pin2_conf(unsigned char prescaler, unsigned char mode, unsigned int top)
{
	unsigned char t;

	switch (prescaler) {
		case PWM_PRESCALER_1024:  t = _BV(CS12) | _BV(CS10);  break;
		case PWM_PRESCALER_256:   t = _BV(CS12);              break;
		case PWM_PRESCALER_64:    t = _BV(CS11) | _BV(CS10);  break;
		case PWM_PRESCALER_8:     t = _BV(CS11);              break;
		default:                  t = _BV(CS10);              break;
	}

	switch (mode) {
		case PWM_MODE_OFF:
			TCCR1A &= (unsigned char)~(_BV(COM1A0) | _BV(COM1A1));
			break;
		case PWM_MODE_FAST:
			TCCR1A &= (unsigned char)~(_BV(COM1A0) | _BV(WGM11));
			TCCR1A |= _BV(COM1A1) | _BV(WGM10);
			TCCR1B &= (unsigned char)~(_BV(CS10) | _BV(CS11) | _BV(CS12) | _BV(WGM13));
			TCCR1B |= _BV(WGM12) | t;
			break;
		case PWM_MODE_FAST_TOP:
			TCCR1A &= (unsigned char)~(_BV(COM1A0) | _BV(WGM10));
			TCCR1A |= _BV(COM1A1) | _BV(WGM11);
			TCCR1B &= (unsigned char)~(_BV(CS10) | _BV(CS11) | _BV(CS12));
			TCCR1B |= _BV(WGM13) | _BV(WGM12) | t;
			ICR1 = top;
			break;
	}

	DDRD |= _BV(PD5);
}

static void
pin3_conf(unsigned char prescaler, unsigned char mode, unsigned int top)
{
	unsigned char t;

	switch (prescaler) {
		case PWM_PRESCALER_1024:  t = _BV(CS12) | _BV(CS10);  break;
		case PWM_PRESCALER_256:   t = _BV(CS12);              break;
		case PWM_PRESCALER_64:    t = _BV(CS11) | _BV(CS10);  break;
		case PWM_PRESCALER_8:     t = _BV(CS11);              break;
		default:                  t = _BV(CS10);              break;
	}

	switch (mode) {
		case PWM_MODE_OFF:
			TCCR1A &= (unsigned char)~(_BV(COM1B0) | _BV(COM1B1));
			break;
		case PWM_MODE_FAST:
			TCCR1A &= (unsigned char)~(_BV(COM1B0) | _BV(WGM11));
			TCCR1A |= _BV(COM1B1) | _BV(WGM10);
			TCCR1B &= (unsigned char)~(_BV(CS10) | _BV(CS11) | _BV(CS12) | _BV(WGM13));
			TCCR1B |= _BV(WGM12) | t;
			break;
		case PWM_MODE_FAST_TOP:
			TCCR1A &= (unsigned char)~(_BV(COM1B0) | _BV(WGM10));
			TCCR1A |= _BV(COM1B1) | _BV(WGM11);
			TCCR1B &= (unsigned char)~(_BV(CS10) | _BV(CS11) | _BV(CS12));
			TCCR1B |= _BV(WGM13) | _BV(WGM12) | t;
			ICR1 = top;
			break;
	}

	DDRD |= _BV(PD4);
}

static void
pin4_conf(unsigned char prescaler, unsigned char mode, unsigned char top)
{
	unsigned char t;

	switch (prescaler) {
		case PWM_PRESCALER_1024:  t = _BV(CS22) | _BV(CS21) | _BV(CS20);  break;
		case PWM_PRESCALER_256:   t = _BV(CS22) | _BV(CS21);  break;
		case PWM_PRESCALER_128:   t = _BV(CS22) | _BV(CS20);  break;
		case PWM_PRESCALER_64:    t = _BV(CS22);              break;
		case PWM_PRESCALER_32:    t = _BV(CS21) | _BV(CS20);  break;
		case PWM_PRESCALER_8:     t = _BV(CS21);              break;
		default:                  t = _BV(CS20);              break;
	}

	switch (mode) {
		case PWM_MODE_OFF:   t |= 0; break;
		case PWM_MODE_FAST:  t |= _BV(WGM21) | _BV(WGM20) | _BV(COM21);  break;
		// FIXME: other modes
	}

	TCCR2 = t;

	DDRD |= _BV(PD7);
}

void
pwm_configure(unsigned char pin, unsigned char prescaler, unsigned char mode, unsigned int top)
{
	switch (pin) {
		case 1:
			pin1_conf(prescaler, mode, (char)(top & 0xff));
			break;
		case 2:
			pin2_conf(prescaler, mode, top);
			break;
		case 3:
			pin3_conf(prescaler, mode, top);
			break;
		case 4:
			pin4_conf(prescaler, mode, (char)(top & 0xff));
			break;
	}
}

void
pwm_set(unsigned char pin, unsigned int value)
{
	switch (pin) {
		case 1:
			OCR0 = (unsigned char)(value & 0xff);
			break;
		case 2:
			OCR1A = value;
			break;
		case 3:
			OCR1B = value;
			break;
		case 4:
			OCR2 = (unsigned char)(value & 0xff);
			break;
	}
}
