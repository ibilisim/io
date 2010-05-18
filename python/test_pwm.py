#!/usr/bin/python
# -*- coding: utf-8 -*-

import iO
import math
import time
import sys

try:
    pinno = int(sys.argv[1])
except:
    pinno = -1

link=iO.iO()

def yakson(pin):
    link.pwm(pin, 0, prescaler=1, mode="fast", top=0)
    
    for i in range(256):
        link.pwm(pin, i)
    
    for i in range(255,-1,-1):
        link.pwm(pin, i)
    
    link.pwm(pin, 0, prescaler=1, mode="off", top=0)

if pinno == -1:
    for i in range(1,5):
        yakson(i)
else:
    yakson(pinno)
