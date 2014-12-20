#!/usr/bin/python

'''
This tool was used to create a random sequence of 4 lights that are used
in Patrick's Wraith as one of the switchable patterns for the roof light bar.
'''

import random

x = range(4)
old_led = None

for i in xrange(30):
    random.shuffle(x)
    for led in x:
        if led == old_led:
            continue
        if old_led is not None:
            print "    retlw   LED{} + OFF".format(old_led + 1)
        print "    retlw   LED{} + ON".format(led + 1)
        print "    retlw   SEQUENCE_DELAY"
        old_led = led

if old_led is not None:
    print "    retlw   LED{} + OFF".format(old_led + 1)


