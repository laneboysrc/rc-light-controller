#!/usr/bin/env python3
'''

preprocessor-reader.py

This utility reads the output of an RC light controller pre-proessor
and displays it in human readable form.

The serial port where the pre-processor is connected can be specified
on the command line.

Author:         Werner Lane
E-mail:         laneboysrc@gmail.com

'''

import argparse
import serial
import sys
import time


CH3_BIT = 0
STARTUP_MODE_NEUTRAL = 4
MULTI_AUX = 3

def parse_commandline():
    ''' Read the Pre-processor output for diagnostic purpose. '''
    parser = argparse.ArgumentParser(
        description="Read the Pre-processor output for diagnostic purpose.")

    parser.add_argument("-b", "--baudrate", type=int, default=115200,
        help='Baudrate to use. Default is 115200.')

    parser.add_argument("tty", nargs="?", default="/dev/ttyUSB0",
        help="Serial port to use. ")

    return parser.parse_args()


def make_signed(value):
    '''
    Convert the unsigned value into a signed value
    '''
    value = ord(value)
    if value > 127:
        return -(256 - value)
    return value


def preprocessor_reader(uart):
    '''
    Read from the UART, parse the preprocessor protocol (3 and 5-channel
    variants), and print the result.
    '''

    time_of_last_line = start_time = time.time()
    header_printed = False

    while True:
        sync = 0x00
        while sync != 0x87:
            sync = ord(uart.read(1))
            if sync != 0x87:
                print("Out of sync: %x" % sync)

        steering = make_signed(uart.read(1))
        throttle = make_signed(uart.read(1))
        mode = ord(uart.read(1))
        if mode & (1 << MULTI_AUX):
            aux = make_signed(uart.read(1))
            aux2 = make_signed(uart.read(1))
            aux3 = make_signed(uart.read(1))

        ch3 = mode & (1 << CH3_BIT)

        message = ""
        if mode & (1 << STARTUP_MODE_NEUTRAL):
            message += 'STARTUP_MODE_NEUTRAL'

        current_time = time.time()
        time_difference = current_time - time_of_last_line
        elapsed_time = current_time - start_time

        time_of_last_line = current_time

        if mode & (1 << MULTI_AUX):
            if not header_printed:
                print("     TOTAL  DIFFERENCE   ST   TH  3  AUX AUX2 AUX3")
                print("----------  ----------  ---- ---- - ---- ---- ----")

            print("%10.3f  %10.3f  %+04d %+04d %d %+04d %+04d %+04d %s" %
                  (elapsed_time, time_difference, steering, throttle, ch3, aux,
                   aux2, aux3, message))
        else:
            if not header_printed:
                print("     TOTAL  DIFFERENCE   ST   TH  3")
                print("----------  ----------  ---- ---- -")
            print("%10.3f  %10.3f  %+04d %+04d %d %s" %
                  (elapsed_time, time_difference, steering, throttle, ch3, message))

        header_printed = True


def main():
    '''
    Parse command line parameters, open the serial port and start the reading
    and parsing process
    '''
    args = parse_commandline()

    print("Pre-processor reader, using %s at %d bps" % (args.tty, args.baudrate))
    print("")

    try:
        uart = serial.Serial(args.tty, args.baudrate)
    except serial.SerialException as error:
        print("%s" % error)
        sys.exit(0)

    try:
        preprocessor_reader(uart)
    except KeyboardInterrupt:
        print("")
        sys.exit(0)


if __name__ == '__main__':
    main()
