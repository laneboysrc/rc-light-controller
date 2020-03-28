#!/usr/bin/env python
'''

slave-reader.py

This utility reads the slave output of an RC light controller
and displays it in human readable form.

The serial port where the light controller is connected can be specified
on the command line.

Author:         Werner Lane
E-mail:         laneboysrc@gmail.com

'''

from __future__ import print_function

import argparse
import serial
import sys
import time


def parse_commandline():
    ''' Read the Pre-processor output for diagnostic purpose. '''
    parser = argparse.ArgumentParser(
        description="Read the Light Controller slave output for diagnostic purpose.")

    parser.add_argument("-b", "--baudrate", type=int, default=115200,
        help='Baudrate to use. Default is 115200.')

    parser.add_argument("tty", nargs="?", default="/dev/ttyUSB0",
        help="Serial port to use. ")

    return parser.parse_args()


# def make_signed(value):
#     '''
#     Convert the unsigned value into a signed value
#     '''
#     value = ord(value)
#     if value > 127:
#         return -(256 - value)
#     return value


def slave_reader(uart):
    '''
    Read from the UART, parse the skave protocol and print the result.
    '''

    time_of_last_line = start_time = time.time()
    header_printed = False

    while True:
        sync = 0x00
        while sync != 0x87:
            sync = ord(uart.read(1))
            if sync != 0x87:
                print("Out of sync: %x" % sync)

        l00 = ord(uart.read(1))
        l01 = ord(uart.read(1))
        l02 = ord(uart.read(1))
        l03 = ord(uart.read(1))
        l04 = ord(uart.read(1))
        l05 = ord(uart.read(1))
        l06 = ord(uart.read(1))
        l07 = ord(uart.read(1))
        l08 = ord(uart.read(1))
        l09 = ord(uart.read(1))
        l10 = ord(uart.read(1))
        l11 = ord(uart.read(1))
        l12 = ord(uart.read(1))
        l13 = ord(uart.read(1))
        l14 = ord(uart.read(1))
        l15 = ord(uart.read(1))

        current_time = time.time()
        time_difference = current_time - time_of_last_line
        elapsed_time = current_time - start_time

        time_of_last_line = current_time

        if not header_printed:
            header_printed = True
            print("     TOTAL  DIFFERENCE   0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15")
            print("----------  ----------  --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---")

        print("%10.3f  %10.3f  %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d %03d " %
              (elapsed_time, time_difference, l00, l01, l02, l03, l04, l05, l06, l07, l08, l09, l10, l11, l12, l13, l14, l15))


def main():
    '''
    Parse command line parameters, open the serial port and start the reading
    and parsing process
    '''
    args = parse_commandline()

    print("Skave reader, using %s at %d bps" % (args.tty, args.baudrate))
    print("")

    try:
        uart = serial.Serial(args.tty, args.baudrate)
    except serial.SerialException, error:
        print("%s" % error)
        sys.exit(0)

    try:
        slave_reader(uart)
    except KeyboardInterrupt:
        print("")
        sys.exit(0)


if __name__ == '__main__':
    main()
