#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright (C) 2008, i Bilisim Teknolojileri Ltd. Sti.
#

import time

import seriport
import crc

def CRC(data):
    c = crc.Crc(width=8,poly=0x31L,reflect_in=True,xor_in=0L,reflect_out=True,xor_out=0L)
    return "%c" % c.table_driven(data)


class iOData:
    def __init__(self, packet):
        if packet[0] != "d":
            raise ValueError, "Not a iO data packet"
        packet = map(ord, packet[1:])
        self.pin = (packet[3] << 24) + (packet[2] << 16) + (packet[1] << 8) + packet[0]
        packet = packet[4:]
        adc = zip(packet[::2], packet[1::2])
        self.adc = [(x << 8) + y for x, y in adc]


class iO:
    def __init__(self, device="/dev/ttyUSB0", speed=38400):
        self.receive(init=True)
        self.port = seriport.Port(device, speed)
        # Pull RESET to high for rev1 (FTDI design)
        self.port.rts=0
        # Give Atmel some time to setup UART
        time.sleep(0.5)
        # Continue with handshake
        self.ask_version()
        data = self.receive_block()
        if data[0] != 'i' or data[1] != 'O':
            raise IOError("Not an iO device")
        self.version = ord(data[2])
        self.revision = ord(data[3])
        if self.version != 1:
            raise IOError("Version %d of iO is not known by this driver" % self.version)
    
    def send(self, data):
        self.port.write("K%s%c" % (data, CRC(data)))
    
    def receive(self, init=False):
        if init:
            self.buffer = ""
            self.need = 2
            return
        self.buffer += self.port.read(self.need - len(self.buffer))
        size = len(self.buffer)
        if size == 2:
            if self.buffer[0] != 'R':
                raise IOError("Bogus data '%s'" % self.buffer)
            c = self.buffer[1]
            if c == 'i':
                self.need = 6
            elif c == 'd':
                self.need = 23
            else:
                raise IOError("Unknown command '%c'" % c)
        if size != self.need:
            return None
        payload = self.buffer[1:-1]
        if self.buffer[-1] != CRC(payload):
            raise IOError("CRC Error")
        self.buffer = ""
        self.need = 2
        return payload
    
    def receive_block(self):
        while True:
            data = self.receive()
            if data:
                return data
    
    def ask_version(self):
        self.send("v")
    
    def reconnect(self):
        # Send max command len of zeroes, so firmware get backs to command state
        self.port.write("\x00" * 10)
        # Flush the input buffer, it may contain previous garbage
        self.port.flush_input()
        self.receive(init=True)
        # Do the version exchange again
        self.ask_version()
        data = self.receive_block()
        if data[0] != 'i' or data[1] != 'O':
            raise IOError("Not an iO device")
        print "Reconnected"
    
    def report(self):
        self.send("r")
    
    def led(self,  a=None,  b=None):
        cmd = 0
        if not a is None:
            cmd |= 0x10
            if a:
                cmd |= 0x01
        if not b is None:
            cmd |= 0x20
            if b:
                cmd |= 0x02
        self.send("l%c" % cmd)
    
    def i2c_send(self,  addr,  data):
        pad = ""
        if len(data) < 142:
            pad = "\x00" * (142 - len(data))
        self.send("i%c%c%s" % (addr,  len(data), data + pad))
    
    def pin_in(self, pin, pullup=0):
        if pullup:
            out = 1
        else:
            out = 0
        self.send("d%c%c" % (pin, out))
    
    def pin_out(self, pin, value):
        if value:
            out = 3
        else:
            out = 2
        self.send("d%c%c" % (pin, out))
    
    def adc(self, mask):
        if mask < 0 or mask > 255:
            raise ValueError, "Invalid ADC mask"
        self.send("A%c" % mask)
    
    def pwm(self, pin, value, prescaler=None, mode=None, top=None):
        prescalers = (1, 8, 32, 64, 128, 256, 1024)
        if not pin in (1, 2, 3, 4):
            raise ValueError, "Invalid pin no"
        if value < 0 or value > 65535:
            raise ValueError, "Invalid PWM value"
        
        if prescaler or mode or top:
            # Defaults
            if not prescaler:
                prescaler = 1
            if not mode:
                mode = "fast"
            if not top:
                top = 0xffff
            # Validate
            if not prescaler in prescalers:
                raise ValueError, "Invalid prescaler"
            md = 0
            if mode == "off":
                md = 0
            elif mode == "fast":
                md = 1
            elif mode == "top":
                md = 2
            else:
                raise ValueError, "Invalid mode"
            # Set
            self.send("P%c%c%c%c%c%c%c" % (
                pin,
                prescalers.index(prescaler),
                md,
                (top&0xff00) >> 8, top&0xff,
                (value&0xff00) >> 8, value&0xff
            ))
        else:
            self.send("p%c%c%c" % (pin, (value&0xff00) >> 8, value&0xff))
