#!/usr/bin/python
# -*- coding: utf-8 -*-

import iO
import time

link=iO.iO()

# Output 5V
link.pin_out(2, 1)
# Output 0V
link.pin_out(3, 0)
# Input (floating)
link.pin_in(4)
# Input with pull-up
link.pin_in(5, pullup=1)

# Get a reading
link.report()
data = iO.iOData(link.receive_block())

def pinstr(x):
    if data.pin & (1 << (x-1)):
        return "1"
    else:
        return "0"

print "Pin 2 (5V) =", pinstr(2)
print "Pin 3 (0V) =", pinstr(3)
print "Pin 4 (input) =", pinstr(4)
print "Pin 5 (input with pullup) =", pinstr(5)
