# AVR-GCC Makefile

PROJECT=iO
SOURCES=main.o calibration.o uart.o pwm.o servo.o dio.o adc.o i2c.o
HEADERS=common.h calibration.h uart.h pwm.h servo.h dio.h adc.h i2c.h
MMCU=atmega16
DUDEOPTS=-p m16 -c ft232

CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS=-mmcu=$(MMCU) -Wall -Os

all: $(PROJECT).hex

$(PROJECT).hex: $(PROJECT).out
	$(OBJCOPY) -j .text -j .data -O ihex $(PROJECT).out $(PROJECT).hex

$(PROJECT).out: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -I./ -o $(PROJECT).out $(SOURCES)

clean:
	rm -f *.o calibtemp *.out *.hex

upload:
	avrdude $(DUDEOPTS) -v -U $(PROJECT).hex

fastupload:
	avrdude $(DUDEOPTS) -V -v -U $(PROJECT).hex

# Set 8 MHz internal clock, disable JTAG
setfuse:
	avrdude $(DUDEOPTS) -U lfuse:w:0xe4:m
	avrdude $(DUDEOPTS) -U hfuse:w:0xD1:m

calibrate:
	avrdude $(DUDEOPTS) -U calibration:r:calibtemp:i
	avrdude $(DUDEOPTS) -U eeprom:w:calibtemp

dump:
	avrdude $(DUDEOPTS) -U eeprom:r:lala:h
