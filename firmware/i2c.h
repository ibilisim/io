/*
 * iO Firmware
 * Copyright (C) 2008 i Bilişim Teknolojileri Ltd. Şti.
 * http://www.ibilisim.com.tr
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of GNU General Public License.
 *
 */

// Frequency calculations
#define I2C_SCL_CLOCK 100000L
#define I2C_DIVIDER(f_cpu) (((f_cpu) / I2C_SCL_CLOCK) - 16) / 2

// Addresses
#define I2C_SLAVE_0 0x01

// i2c_poll return values
#define I2C_READ  0x0100
#define I2C_WRITE 0x0200
#define I2C_DATA(c) ((c) & 0x00ff)

// Master functions
extern void i2c_master_init(void);
extern uint8_t i2c_start_writing(uint8_t address);
extern uint8_t i2c_start_reading(uint8_t address);
extern unsigned char i2c_write(unsigned char data);
extern uint8_t i2c_read(unsigned char *buf, unsigned char size);
extern void i2c_stop(void);

// Slave functions
extern void i2c_slave_init(uint8_t address);
extern unsigned int i2c_poll(void);
extern uint8_t i2c_slave_write(unsigned char *buf, unsigned char size);
