#!/usr/bin/env python
'''

Validate the binary image of the TLC5940/LPC812 based light controller
to ensure it contains the required sections for the configuration tool.

Each section has a magic value 0x6372424c, followed by the section identifier
and a version number.

'''
from __future__ import print_function
import sys
import struct

ROM_MAGIC = 0x6372424c          # LBrc (LANE Boys RC) in little endian
CONFIG_SECTION = 0x01
GAMMA_TABLE = 0x02
LOCAL_MONOCHROME_LEDS = 0x10
LOCAL_RGB_LEDS = 0x11
SLAVE_MONOCHROME_LEDS = 0x20
SLAVE_RGB_LEDS = 0x21


def find_section(content, offset):
    ''' Find the next section in content starting at offset '''
    for i in range(offset, len(content) - 4):
        value = struct.unpack('<I', content[i:i+4])[0]
        if value == ROM_MAGIC:
            section = struct.unpack('<H', content[i+4:i+6])[0]
            version = struct.unpack('<H', content[i+6:i+8])[0]
            return (str(section), version, i)
    return (None, None, None)


def dump_sections(content):
    ''' Find all sections in the image file and dump their name and offset '''
    section_name, version, offset = find_section(content, 0)
    while section_name is not None:
        version = version
        print('Found section "{:s}", version {:d} at offset 0x{:x}'.format(
            section_name, version, offset))
        section_name, version, offset = find_section(content, offset + 1)


def main():
    ''' The application... '''
    try:
        bin_filename = sys.argv[1]
    except IndexError:
        print('usage: {name} <binary image file>'.format(name=sys.argv[0]))
        sys.exit(1)

    try:
        with open(bin_filename, 'rb') as binfile:
            image_content = binfile.read()
            dump_sections(image_content)

    except IOError as error:
        print('ERROR: failed to read light controller image: {}'.format(error))
        sys.exit(1)

if __name__ == "__main__":
    main()
