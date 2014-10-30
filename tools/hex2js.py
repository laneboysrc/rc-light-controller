#!/usr/bin/env python
'''

Tool for creating a JavaScript string containing a Intel-hex file.

This is useful for embedding a Intel-hex file in a JavaScript application.

'''
from __future__ import print_function
import argparse


def parse_commandline():
    ''' Command line option parsing '''
    parser = argparse.ArgumentParser(
        description='''\
Tool for creating a JavaScript string containing a Intel-hex file.''')

    parser.add_argument("hex_file", nargs=1, type=argparse.FileType('rb'),
        help="the filename of the light controller firmware in Intel-hex "
             "format")

    return parser.parse_args()


def hex2js(args):
    ''' Dump all lines as JavaScript string '''

    print("var default_firmware_image =")

    for line in args.hex_file[0]:
        print('"{l}\\n" +'.format(l=line.strip()))

    print('"";')

def main():
    ''' The application... '''
    args = parse_commandline()
    hex2js(args)


if __name__ == "__main__":
    main()
