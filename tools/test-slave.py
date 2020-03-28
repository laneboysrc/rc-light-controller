#!/usr/bin/env python
'''
test-tlc5940-16f1825-slave.py

This utility sends various light patterns to a "slave" that is based on the
TLC5940 and PIC16F1825.
Its behaviour is similar to the test-tlc5940-16f1825.hex program.

Author:         Werner Lane
E-mail:         laneboysrc@gmail.com
'''

from __future__ import print_function

import sys
import time
import serial

BAUDRATE = 115200

SLAVE_MAGIC_BYTE = 0x87
NUMBER_OF_LEDS = 16

VAL_STEP1 = 63
VAL_STEP2 = 31
VAL_STEP3 = 15
VAL_STEP4 = 7
VAL_STEP5 = 3
VAL_STEP6 = 1
VAL_FULL = 63


def delay(timeout_in_s):
    ''' Delay execution for the given number of seconds (can be fractional) '''
    time.sleep(timeout_in_s)


class Testapp(object):
    ''' Send the test patterns to the TLC5940 based slave '''

    def __init__(self):
        try:
            port = sys.argv[1]
        except IndexError:
            port = '/dev/ttyUSB0'

        try:
            self.uart = serial.Serial(port, BAUDRATE)
        except serial.SerialException as error:
            print("Unable to open port %s: %s" % (port, error))
            sys.exit(0)

        print("Sending SLAVE data on {uart} at {baudrate} baud.".format(
            uart=self.uart.port, baudrate=self.uart.baudrate))


    def send_to_slave(self, light_data):
        ''' Send light_data to the slave via the slave UART protocol '''
        data = bytearray([SLAVE_MAGIC_BYTE]) + bytearray(light_data)
        self.uart.write(data)
        self.uart.flush()


    def sequence_lights(self, value):
        ''' Sequence all lights in turn with the given value '''
        print("Sequence: value={} ".format(value), end="")
        for index in range(NUMBER_OF_LEDS):
            light_data = [0] * NUMBER_OF_LEDS
            light_data[index] = value
            self.send_to_slave(light_data)
            print(".", end="")
            sys.stdout.flush()
            delay(0.1)
        print("")


    def all_lights(self, value):
        ''' Turn all lights on at the given level '''
        print("All lights: value={}".format(value))
        light_data = [value] * NUMBER_OF_LEDS
        self.send_to_slave(light_data)


    def run(self):
        ''' Send the test patterns to the TLC5940 based slave '''
        while True:
            self.sequence_lights(VAL_STEP1)
            self.sequence_lights(VAL_STEP2)
            self.sequence_lights(VAL_STEP3)
            self.sequence_lights(VAL_STEP4)
            self.sequence_lights(VAL_STEP5)
            self.sequence_lights(VAL_STEP6)

            self.all_lights(VAL_FULL)
            delay(5)

            self.all_lights(VAL_STEP6)
            delay(1)
            self.all_lights(VAL_STEP5)
            delay(1)
            self.all_lights(VAL_STEP4)
            delay(1)
            self.all_lights(VAL_STEP3)
            delay(1)
            self.all_lights(VAL_STEP2)
            delay(1)
            self.all_lights(VAL_STEP1)
            delay(1)
            self.all_lights(0)
            delay(1)


if __name__ == '__main__':
    try:
        Testapp().run()
    except KeyboardInterrupt:
        print("")
        sys.exit(0)
