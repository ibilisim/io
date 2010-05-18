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
	PWM_PRESCALER_1,
	PWM_PRESCALER_8,
	PWM_PRESCALER_32,
	PWM_PRESCALER_64,
	PWM_PRESCALER_128,
	PWM_PRESCALER_256,
	PWM_PRESCALER_1024
};

enum {
	PWM_MODE_OFF,
	PWM_MODE_FAST,
	PWM_MODE_FAST_TOP

};

extern void pwm_configure(unsigned char pin, unsigned char prescaler, unsigned char mode, unsigned int top);
extern void pwm_set(unsigned char pin, unsigned int value);
