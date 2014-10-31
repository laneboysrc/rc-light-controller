#include <globals.h>

__attribute__ ((section(".light_programs")))
const LIGHT_PROGRAMS_T light_programs = {
    .magic = {
        .magic_value = ROM_MAGIC,
        .type = LIGHT_PROGRAMS,
        .version = CONFIG_VERSION
    },

    .number_of_programs = 7,
    .start = {
        &light_programs.programs[0],
        &light_programs.programs[10],
        &light_programs.programs[20],
        &light_programs.programs[31],
        &light_programs.programs[37],
        &light_programs.programs[46],
        &light_programs.programs[56],
    },

    .programs = {
        0x00000001,
        0x00000000,
        0x0000ffff,
        0x040f0000,
        0x03050000,
        0x030f0800,
        0x03070664,
        0x07000000,
        0x01000002,
        0xfe000000,

        0x00000002,
        0x00000000,
        0x0000ffff,
        0x040f0000,
        0x03010000,
        0x030f0400,
        0x03030264,
        0x07000000,
        0x01000002,
        0xfe000000,

        0x00000020,
        0x00000000,
        0x0000ffcf,
        0x04030000,
        0x040f0600,
        0x03030000,
        0x030d0700,
        0x030f0f00,
        0x03060664,
        0x030e0e64,
        0xfe000000,

        0x00000040,
        0x00000000,
        0x00000030,
        0x04050400,
        0x03050464,
        0xfe000000,

        0x00000004,
        0x00000000,
        0x0000ffff,
        0x040f0000,
        0x03050000,
        0x030d0800,
        0x03070664,
        0x030f0e64,
        0xfe000000,

        0x00000008,
        0x00000000,
        0x0000ffff,
        0x040f0000,
        0x03050000,
        0x030d0700,
        0x030f0f00,
        0x03060664,
        0x030e0e64,
        0xfe000000,

        0x00000010,
        0x00000000,
        0x0000ffff,
        0x040f0000,
        0x03060000,
        0x030e0800,
        0x03070764,
        0x030f0f64,
        0xfe000000,

        0xff000000,
    }
};
