#!/usr/bin/env python3

'''
Progran that decodes an exported binary file from a capture with the Saleae
logic analyzer.

Use "logic" from Salaea and export only the serial port channel into .bin
format

    python i-bus_decoder_for_saleae_format.py i-bus.bin >decoded.txt

The decoded packets data can be used in Python or JavaScript.
'''

import array
import struct
import sys
from collections import namedtuple

TYPE_DIGITAL = 0
TYPE_ANALOG = 1
expected_version = 0

DigitalData = namedtuple('DigitalData', ('initial_state', 'begin_time', 'end_time', 'num_transitions', 'transition_times'))

bit_time_us = 1 / 115200 * 1000000

def parse_digital(f):
    # Parse header
    identifier = f.read(8)
    if identifier != b"<SALEAE>":
        raise Exception("Not a saleae file")

    version, datatype = struct.unpack('=ii', f.read(8))

    if version != expected_version or datatype != TYPE_DIGITAL:
        raise Exception("Unexpected data type: {}".format(datatype))

    # Parse digital-specific data
    initial_state, begin_time, end_time, num_transitions = struct.unpack('=iddq', f.read(28))

    # Parse transition times
    transition_times = array.array('d')
    transition_times.fromfile(f, num_transitions)

    return DigitalData(initial_state, begin_time, end_time, num_transitions, transition_times)


def decode_ibus(data):
    state = 'WAIT_FOR_FIRST_PACKET'
    line_state = data.initial_state
    last_transition_time = data.begin_time

    print('ibus_packets = [')

    for time in data.transition_times:
        diff_us = (time-last_transition_time) * 1000000
        bits = int(round(diff_us / bit_time_us, 0))

        if state == 'WAIT_FOR_FIRST_PACKET':
            if bits > 10 and line_state == 1:
                print('  [', end='')
                data = 0
                data_bits = 0
                state = 'GET_BYTE'

        elif state == 'GET_BYTE':
            for _ in range(bits):
                data = (data >> 1) | line_state << 8
                data_bits += 1
                if data_bits == 9:
                    print(f'0x{(data>>1):02x}, ', end='')
                    data = 0
                    data_bits = 0
                    if line_state == 0:
                        state = 'WAIT_FOR_START'
                    break

            if bits > 10 and line_state == 1:
                print('],\n  [', end='')
                data = 0
                data_bits = 0

        elif state == 'WAIT_FOR_START':
            if line_state == 1:
                state = 'GET_BYTE'

        line_state = 0 if line_state else 1
        last_transition_time = time

    if state == 'WAIT_FOR_START':
        print(']')
    print(']')

if __name__ == '__main__':
    filename = sys.argv[1]

    with open(filename, 'rb') as f:
        data = parse_digital(f)
        decode_ibus(data)