#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import os
import fcntl
import termios
import struct
import time

IFLAG, OFLAG, CFLAG, LFLAG, ISPEED, OSPEED, CC = range(7)

def list_ports():
    # This doesn't work with 2008 kernel
    return map(lambda x: os.path.realpath("/dev/tts/" + x), os.listdir("/dev/tts"))


class Port:
    def __init__(self, device_name, speed=38400, mode="8n1"):
        if mode != "8n1":
            raise ValueError("Mode '%s' is not yet supported" % mode)
        
        self.fd = os.open(device_name, os.O_RDWR)
        prefs = termios.tcgetattr(self.fd)
        
        # Set speed
        speed_val = getattr(termios, "B" + str(speed), None)
        if not speed_val:
            raise ValueError("Unknown speed value '%s'" % speed)
        prefs[OSPEED] = prefs[ISPEED] = speed_val
        
        # Activate receiver
        prefs[CFLAG] |= (termios.CLOCAL | termios.CREAD)
        
        # Disable echo, output modification, etc
        prefs[LFLAG] = 0
        prefs[OFLAG] = 0
        
        # Set mode (8n1)
        prefs[CFLAG] &= ~termios.CSIZE
        prefs[CFLAG] |= termios.CS8
        prefs[CFLAG] &= ~(termios.PARENB|termios.PARODD)
        prefs[CFLAG] &= ~termios.CSTOPB
        
        # No flow control
        prefs[IFLAG] &= ~(termios.IXON|termios.IXOFF | termios.IXANY)
        prefs[CFLAG] &= ~(termios.CRTSCTS)
        
        # Dont mess with CRLF chars
        prefs[IFLAG] &= ~(termios.IGNCR | termios.ICRNL | termios.INLCR)
        
        # Ignore parity and break
        prefs[IFLAG] |= termios.IGNPAR | termios.IGNBRK
        
        termios.tcsetattr(self.fd, termios.TCSANOW, prefs)
    
    def __getattr__(self, name):
        if name == "cts":
            s = fcntl.ioctl(self.fd, termios.TIOCMGET, struct.pack('I', 0))
            return struct.unpack('I',s)[0] & termios.TIOCM_CTS != 0
        elif name == "dsr":
            s = fcntl.ioctl(self.fd, termios.TIOCMGET, struct.pack('I', 0))
            return struct.unpack('I',s)[0] & termios.TIOCM_DSR != 0
        elif name == "ri":
            s = fcntl.ioctl(self.fd, termios.TIOCMGET, struct.pack('I', 0))
            return struct.unpack('I',s)[0] & termios.TIOCM_RI != 0
        elif name == "cd":
            s = fcntl.ioctl(self.fd, termios.TIOCMGET, struct.pack('I', 0))
            return struct.unpack('I',s)[0] & termios.TIOCM_CD != 0
        else:
            raise AttributeError, name
    
    def __setattr__(self, name, value):
        if name == "rts":
            cmd = termios.TIOCMBIC
            if value:
                cmd = termios.TIOCMBIS
            fcntl.ioctl(self.fd, cmd, struct.pack('I', termios.TIOCM_RTS))
        elif name == "dtr":
            cmd = termios.TIOCMBIC
            if value:
                cmd = termios.TIOCMBIS
            fcntl.ioctl(self.fd, cmd, struct.pack('I', termios.TIOCM_DTR))
        else:
            self.__dict__[name] = value
    
    def read(self, len):
        return os.read(self.fd, len)
    
    def write(self, data):
        return os.write(self.fd, data)
    
    def flush_input(self):
        termios.tcflush(self.fd, termios.TCIFLUSH)


if __name__ == "__main__":
    print "Detected devices:", list_ports()
    port = Port("/dev/ttyUSB0")
    port = Port("/dev/ttyUSB0", speed=115200)
    print port.cts, port.dsr, port.ri, port.cd
    #port.rts = True
    #port.rts = False
    #port.dtr = True
    #port.dtr = False
    port.write("lala")
    print port.read(10)
