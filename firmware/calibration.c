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

#include <avr/eeprom.h>

// EEPROM addresses to store calibration value
uint8_t calibration_pointer_1mhz __attribute__((section(".eeprom")));
uint8_t calibration_pointer_2mhz __attribute__((section(".eeprom")));
uint8_t calibration_pointer_4mhz __attribute__((section(".eeprom")));
uint8_t calibration_pointer_8mhz __attribute__((section(".eeprom")));

void
calibrate_oscillator(void)
{
	unsigned char value;

	// Calibration value is read from eeprom
	// It must be written to eeprom via 'make calibrate' before actual use of IC
	eeprom_busy_wait();
#if F_CPU == 8000000L
	value = eeprom_read_byte(&calibration_pointer_8mhz);
#else
#error Calibration pointer is not coded for this F_CPU value
#endif

	// 0xff means eeprom is erased and calibration value is not written
	if (value != 0xff) OSCCAL = value;
}
