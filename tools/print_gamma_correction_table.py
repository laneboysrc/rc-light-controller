#!/usr/bin/env python
'''
Gamma correction for LEDs
'''

from __future__ import print_function
import sys
from decimal import Decimal, ROUND_HALF_UP

def gamma_correction(level, gamma):
    ''' Calculate gamma corrected 8-bit value '''

    return Decimal(255 * pow(float(level) / 255, gamma)).to_integral_value(
        rounding=ROUND_HALF_UP)

def main():
    ''' Main application '''

    try:
        gamma = float(sys.argv[1])

    except ValueError:
        print("usage: {} [gamma_as_float]".format(sys.argv[0]))
        sys.exit(1)

    except IndexError:
        gamma = 2.2

    print('const uint8_t gamma_table[] = {')

    for level in range(256):
        step = gamma_correction(level, gamma)
        print("{step:d}, ".format(level=level, step=int(step)), end='')
        level += 1

    print('\n};')

main()
