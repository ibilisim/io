/*
 * iO Firmware
 * Copyright (C) 2008 i Bilişim Teknolojileri Ltd. Şti.
 * http://www.ibilisim.com.tr
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of GNU General Public License.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <util/twi.h>

#include "common.h"
#include "i2c.h"

/*
** I2C Master Functions
*/

void
i2c_master_init(void)
{
	// Enable internal pull-up resistors on the I2C bus
	PORTC |= _BV(PC0) | _BV(PC1);

	// Adjust I2C clock frequency
	TWSR = 0;
	TWBR = I2C_DIVIDER(F_CPU);

	TWCR = _BV(TWEN);
}

uint8_t
i2c_start_writing(uint8_t address)
{
	uint8_t st;

	// Send START condition
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTA);
	while ((TWCR & _BV(TWINT)) == 0);
	st = TW_STATUS;
	if ((st != TW_START) && (st != TW_REP_START)) return 1;

	// Send slave address
	TWDR = address << 1;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while ((TWCR & _BV(TWINT)) == 0);
	st = TW_STATUS;
	if (st != TW_MT_SLA_ACK) return st;

	return 0;
}

uint8_t
i2c_start_reading(uint8_t address)
{
	uint8_t st;

	// Send START condition
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTA);
	while ((TWCR & _BV(TWINT)) == 0);
	st = TW_STATUS;
	if ((st != TW_START) && (st != TW_REP_START)) return 1;

	// Send slave address
	TWDR = (address << 1) | 0x01;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while ((TWCR & _BV(TWINT)) == 0);
	st = TW_STATUS;
	if (st != TW_MR_SLA_ACK) return st;

	return 0;
}

unsigned char
i2c_write(unsigned char data)
{
	uint8_t st;

	TWDR = data;
	TWCR = _BV(TWINT) | _BV(TWEN);
	while ((TWCR & _BV(TWINT)) == 0);
	st = TW_STATUS;
	if (st != TW_MT_DATA_ACK) return st;
	return 0;
}

uint8_t
i2c_read(unsigned char *buf, unsigned char size)
{
	uint8_t st;

	while (size > 0) {
		if (size > 1) {
			// ACK first bytes
			TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
		} else {
			// NACK the last byte
			TWCR = _BV(TWINT) | _BV(TWEN);
		}
		while ((TWCR & _BV(TWINT)) == 0);
		st = TW_STATUS;
// FIXME: what code we should get here?
//		if (st != TW_MT_DATA_ACK) return st;
		*buf++ = TWDR;
		size--;
	}
	return 0;
}

void
i2c_stop(void)
{
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO);
// FIXME: is this needed?
//	while ((TWCR & _BV(TWINT)) == 0);
}

/*
** I2C Slave Functions
*/

void
i2c_slave_init(uint8_t address)
{
	// Adjust I2C clock frequency
	TWSR = 0;
	TWBR = I2C_DIVIDER(F_CPU);

	// Catch messages for this address
	TWAR = address << 1;
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
}

unsigned int
i2c_poll(void)
{
	uint8_t st;
	unsigned int ret;

	// Wait until something happens on the bus
	while ((TWCR & _BV(TWINT)) == 0);
	st = TW_STATUS;

	ret = 0;
	if (st == TW_SR_DATA_ACK) {
		ret = TWDR;
		ret |= I2C_WRITE;
	}
	if (st == TW_ST_SLA_ACK) {
		return I2C_READ;
	}
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
	return ret;
}

uint8_t
i2c_slave_write(unsigned char *buf, unsigned char size)
{
	uint8_t st;

	while (size) {
		TWDR = *buf++;
		if (size > 1) {
			// ACK first bytes
			TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
		} else {
			// NACK the last byte
			TWCR = _BV(TWINT) | _BV(TWEN);
		}
		while ((TWCR & _BV(TWINT)) == 0);
		st = TW_STATUS;
		size--;
		if (st != TW_ST_DATA_ACK && size) return st;
	}
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
	return 0;
}
