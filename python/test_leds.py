#!/usr/bin/python
# -*- coding: utf-8 -*-

import iO
import time

link = iO.iO()

def led(a, b):
    link.led(a=a,  b=b)
    time.sleep(0.5)

def cycle():
    led(1, 0)
    led(1, 1)
    led(0, 1)
    led(0, 0)

cycle()
