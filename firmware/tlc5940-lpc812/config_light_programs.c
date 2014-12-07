// Auto-generated file. Do not modify.
// Source: /home/werner/projects/rc-light-controller/firmware/tlc5940-lpc812/light_programs/generic.light_program

#include <globals.h>

__attribute__ ((section(".light_programs")))
const LIGHT_PROGRAMS_T light_programs = {
    .magic = {
        .magic_value = ROM_MAGIC,
        .type = LIGHT_PROGRAMS,
        .version = CONFIG_VERSION
    },

    .number_of_programs = 4,
    .start = {
        &light_programs.programs[0],
        &light_programs.programs[9],
        &light_programs.programs[18],
        &light_programs.programs[33],
    },

    .programs = {
        0x00000001,
        0x00000000,
        0xffffffff,
        0x051f0000,
        0x031f0000,
        0x03070664,
        0x07000000,
        0x01000000,
        0xfe000000,

        0x00000002,
        0x00000000,
        0xffffffff,
        0x051f0000,
        0x031f0000,
        0x03030264,
        0x07000000,
        0x01000000,
        0xfe000000,

        0x00000060,
        0x00000000,
        0xffffffff,
        0x07000000,
        0x051f0000,
        0x031f0000,
        0x68000000,
        0x01000007,
        0x03060664,
        0x030c0c64,
        0x70000000,
        0x01000000,
        0x03050464,
        0x01000000,
        0xfe000000,

        0x0000001c,
        0x00000000,
        0xffffffff,
        0x07000000,
        0x051f0000,
        0x031f0000,
        0xa2000000,
        0x0100000a,
        0xa4000000,
        0x0100000d,
        0x03070664,
        0x030d0c64,
        0x01000000,
        0x03060664,
        0x030c0c64,
        0x01000000,
        0x03070764,
        0x030d0d64,
        0x01000000,
        0xfe000000,

        0xff000000,
    }
};
