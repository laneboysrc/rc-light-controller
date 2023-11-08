#!/usr/bin/env python3
'''

Validate the binary image of the TLC5940/LPC812 based light controller
to ensure it contains the required sections for the configuration tool.

Each section has a magic value 0x6372424c, followed by the section identifier
and a version number.

'''
import sys
import struct
import argparse
from intelhex import IntelHex, HexRecordError

ROM_MAGIC = 0x6372424c          # LBrc (LANE Boys RC) in little endian

SECTIONS = {0x01: "Configuration", 0x02: "Gamma table", 0x30: "Light programs",
    0x10: "Local LEDs", 0x20: "Slave LEDs"}

MAX_FILE_SIZE = 16 * 1024       # 16 kBytes FLASH size of the LCP812


def parse_commandline():
    ''' Command line option parsing '''
    parser = argparse.ArgumentParser(
        description='''\
Validate the binary image of the TLC5940/LPC8xx based light controller
to ensure it contains the required sections for the configuration tool.

Each section has a magic value 0x6372424c, followed by the section identifier
and a version number.''')

    parser.add_argument("-v", "--verbose", action='store_true',
        help='Print info about sections found and their location.')

    parser.add_argument("image_file", nargs=1, type=argparse.FileType('rt'),
        help="the filename of the light controller binary image")

    return parser.parse_args()


def find_section(content, offset):
    ''' Find the next section in content starting at offset '''
    for i in range(offset, len(content) - 4):
        value = struct.unpack('<I', content[i:i+4])[0]
        if value == ROM_MAGIC:
            section = struct.unpack('<H', content[i+4:i+6])[0]
            version = struct.unpack('<H', content[i+6:i+8])[0]
            try:
                section_name = SECTIONS[section]
            except KeyError:
                section_name = '{s} (0x{s:x})'.format(s=section)
            return (section_name, version, i)
    return (None, None, None)


def dump_sections(args):
    ''' Find all sections in the image file and dump their name and offset '''
    try:
        hexfile = IntelHex()
        hexfile.fromfile(args.image_file[0], format='hex')
        content = hexfile.tobinarray()

    except HexRecordError:
        # Not a valid HEX file, so assume we are dealing with a binary image
        args.image_file[0].seek(0)
        content = args.image_file[0].read(MAX_FILE_SIZE + 1)


    if len(content) > MAX_FILE_SIZE:
        print('ERROR: Binary image file larger than 16 kBytes')
        sys.exit(1)


    available_sections = dict()
    for section_name in SECTIONS.values():
        available_sections[section_name] = 0

    error_found = False

    section_name, version, offset = find_section(content, 0)
    while section_name is not None:
        try:
            available_sections[section_name] += 1
            if args.verbose:
                print('Found "{:s}", version {:d} at offset 0x{:x}'.format(
                    section_name, version, offset))
        except KeyError:
            print('ERROR: Unknown section {}'.format(section_name))
            error_found = True

        section_name, version, offset = find_section(content, offset + 1)

    for section_name in available_sections:
        if available_sections[section_name] == 0:
            error_found = True
            print('ERROR: Section "{}" is missing'.format(section_name))
        elif available_sections[section_name] > 1:
            error_found = True
            print('ERROR: Multiple "{}" sections found'.format(section_name))

    if error_found:
        sys.exit(1)


def main():
    ''' The application... '''
    args = parse_commandline()
    dump_sections(args)


if __name__ == "__main__":
    main()
