#!/usr/bin/python
# -*- coding: utf-8 -*-

import iO
import time

link=iO.iO()

# Start reading all 8 ADC pins (mask = 0x11111111)
link.adc(0xff)

# Get a reading
link.report()
data = iO.iOData(link.receive_block())

print data.adc
