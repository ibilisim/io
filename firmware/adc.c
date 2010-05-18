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

#include <avr/interrupt.h>

volatile unsigned int adc_value[8];
static volatile unsigned char my_pins;
static volatile unsigned char cur_pin;

static void
valid_pin(void)
{
	while (1) {
		if (cur_pin > 7) cur_pin = 0;
		if (my_pins & (1 << cur_pin)) break;
		cur_pin++;
	}
}

ISR(ADC_vect)
{
	unsigned int value;

	// Read converted value
	value = ADCL;
	value |= ADCH << 8;
	adc_value[cur_pin] = value;

	// Select next channel
	cur_pin++;
	valid_pin();
	ADMUX = cur_pin;
	// Start conversation
	ADCSRA |= _BV(ADSC);
}

void
adc_configure(unsigned char pinmask)
{
	unsigned char ps;

	// Turn off ADC
	ADCSRA = 0;

	my_pins = pinmask;
	if (!my_pins) return;

	// Prescaler should reduce main clock freq to below 200KHz
#if F_CPU == 8000000L
	// prescaler 1/64
	ps = _BV(ADPS2) | _BV(ADPS1);
#else
#error Prescaler is not coded for current F_CPU value
#endif

	// Now enable ADC and its interrupt
	ADCSRA = _BV(ADEN) | _BV(ADIE) | ps;

	// Select first channel
	cur_pin = 0;
	valid_pin();
	ADMUX = cur_pin;
	// Start conversation
	ADCSRA |= _BV(ADSC);
}

void
adc_read(unsigned char *buf)
{
	unsigned char i;

	cli();
	for (i = 0; i < 8; i++) {
		buf[0] = adc_value[i] >> 8;
		buf[1] = adc_value[i] & 0x00ff;
		buf += 2;
	}
	sei();
#if 0
	unsigned char leftover;
	unsigned int value;

	leftover = 0;
	for (i = 0; i < 4; i++) {
		value = adc_sample(i);
		*buf++ = value & 0xff;
		leftover |= ((value & 0x0300) >> ((i + 1) * 2));
	}
	*buf++ = leftover;
	leftover = 0;
	for (i = 4; i < 8; i++) {
		value = adc_sample(i);
		*buf++ = value & 0xff;
		leftover |= ((value & 0x0300) >> ((i - 3) * 2));
	}
	*buf = leftover;
#endif
}
