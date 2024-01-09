#!/usr/bin/env python3
'''

Tool for creating a JavaScript string containing the content of a given
text file.

This is useful for embedding text (and also Intel-hex files) in a
JavaScript application.

'''
import argparse
import os
import sys


def parse_commandline():
    ''' Command line option parsing '''

    parser = argparse.ArgumentParser(
        description='''\
Tool for creating a JavaScript string containing the content of a text file.''')

    parser.add_argument("text_file", nargs=1, type=argparse.FileType('rt'),
        help="the filename of the text file format")

    parser.add_argument("var_name", nargs=1,
        help="the JavaScript variable name to assign the text to")

    return parser.parse_args()


def text2js(args):
    ''' Dump all lines as JavaScript string '''

    print("// Auto-generated file. Do not modify.")
    print("//")
    print("// Generated from file {file} by {tool}".format(
        tool=os.path.basename(sys.argv[0]),
        file=args.text_file[0].name))

    print("var {} =".format(args.var_name[0]))

    for line in args.text_file[0]:
        print('"{l}\\n" +'.format(
            l=line.replace('\\', '\\\\').replace('"', '\\"').rstrip()))

    print('"";')


def main():
    ''' The application... '''
    args = parse_commandline()
    text2js(args)


if __name__ == "__main__":
    main()
