#******************************************************************************
#
#   preprocessor-reader.py
#
#   This utility reads the output of an RC light controller pre-proessor
#   and displays it in human readable form.
#
#   The serial port where the pre-processor is connected can be specified
#   on the command line.
#
#******************************************************************************
#
#   Author:         Werner Lane
#   E-mail:         laneboysrc@gmail.com
#
#******************************************************************************

import serial
import sys

STARTUP_MODE_NEUTRAL = 4


def preprocessor_reader(port):
    try:
        s = serial.Serial(port, 38400)
    except serial.SerialException, e:
        print("Unable to open port %s.\nError message: %s" % (port, e))
        sys.exit(0)

    while True:
        sync = 0x00
        while sync != 0x87:
            sync = ord(s.read(1))
            if sync != 0x87:
                print("Out of sync: %x" % sync)

        st = ord(s.read(1))
        th = ord(s.read(1))
        ch3 = ord(s.read(1))

        if st > 127:
            st = -(256 - st)
        if th > 127:
            th = -(256 - th)

        message = ""
        if ch3 & (1 << STARTUP_MODE_NEUTRAL):
            message += 'STARTUP_MODE_NEUTRAL'
        ch3 = ch3 & 0x0f

        print("%+04d %+04d %d %s" % (st, th, ch3, message))


if __name__ == '__main__':
    try:
        port = sys.argv[1]
    except IndexError:
        port = '/dev/ttyUSB0'

    try:
        preprocessor_reader(port)
    except KeyboardInterrupt:
        print("")
        sys.exit(0)
