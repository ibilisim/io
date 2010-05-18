/*
 * iO Firmware
 * Copyright (C) 2008 i Bilişim Teknolojileri Ltd. Şti.
 * http://www.ibilisim.com.tr
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of GNU General Public License.
 *
 */

#define SERVO_MAX 4

extern void servo_configure(void);
extern void servo_set(unsigned char pin, unsigned int pos);
