#!/usr/bin/env python
'''
Gamma correction for LEDs
'''

from __future__ import print_function

def gamma_correction(level, gamma):
    ''' Calculate gamma corrected 8-bit value '''
    return int(255 * pow(float(level) / 255, gamma))

def main():
    ''' Main application '''
    gamma = 2.2
    level = 0
    print('const uint8_t gamma_table[] = {')
    while level < 256:
        step = gamma_correction(level, gamma)
        print("{step:d}, ".format(level=level, step=step), end='')
        level += 1

    print('\n};')

main()
