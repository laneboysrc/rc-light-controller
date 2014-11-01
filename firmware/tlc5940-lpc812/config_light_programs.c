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
        0x0000ffff,
        0x050f0000,
        0x030f0000,
        0x03070664,
        0x07000000,
        0x01000000,
        0xfe000000,

        0x00000002,
        0x00000000,
        0x0000ffff,
        0x050f0000,
        0x030f0000,
        0x03030264,
        0x07000000,
        0x01000000,
        0xfe000000,

        0x00000060,
        0x00000000,
        0x0000ffff,
        0x07000000,
        0x050f0000,
        0x030f0000,
        0x68000000,
        0x01000007,
        0x03060664,
        0x030e0e64,
        0x70000000,
        0x01000000,
        0x03050464,
        0x01000000,
        0xfe000000,

        0x0000001c,
        0x00000000,
        0x0000ffff,
        0x07000000,
        0x050f0000,
        0x030f0000,
        0xa2000000,
        0x0100000a,
        0xa4000000,
        0x0100000d,
        0x03070664,
        0x030f0e64,
        0x01000000,
        0x03060664,
        0x030e0e64,
        0x01000000,
        0x03070764,
        0x030f0f64,
        0x01000000,
        0xfe000000,

        0xff000000,
    }
};
