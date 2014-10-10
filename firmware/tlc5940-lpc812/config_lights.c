#include <stdint.h>
#include <stdbool.h>
#include <globals.h>


// ****************************************************************************
const CAR_LIGHT_ARRAY_T local_leds = {
    .magic = {
        .magic_value = ROM_MAGIC,
        .type = LOCAL_LEDS,
        .version = CONFIG_VERSION
    },

    .led_count = 16,

    .car_lights = (const CAR_LIGHT_T [16]) {
        // LED 0
        {.always_on = 255, .features = {.max_change_per_systick = 3}},

        // LED 1
        {.light_switch_position[1] = 255, .light_switch_position[2] = 255},

        // LED 2
        {.light_switch_position[2] = 255},

        // LED 3
        {.tail_light = 255, .features = {
                .max_change_per_systick = 37,
                .reduction_percent = 20,
                .indicator_left = 1,
            },
        },

        // LED 4
        {.brake_light = 255},

        // LED 5
        {.always_on = 0}, // LED not present...

        // LED 6
        {.tail_light = 85, .brake_light = 255},

        // LED 7
        {.reversing_light = 255},

        // LED 8
        {.indicator_left = 255, .features = {.max_change_per_systick = 37}},

        // LED 9
        {.indicator_right = 255, .features = {.max_change_per_systick = 37}},

        // LED 10
        {.always_on = 0},

        // LED 11
        {.indicator_left = 85, .tail_light = 85, .brake_light = 255},

        // LED 12
        {.indicator_right = 85, .tail_light = 85, .brake_light = 255},

        // LED 13
        {.always_on = 0},

        // LED 14
        {.always_on = 0},

        // LED 15
        {.always_on = 0}
    }
};


// ****************************************************************************
const CAR_LIGHT_ARRAY_T slave_leds = {
    .magic = {
        .magic_value = ROM_MAGIC,
        .type = SLAVE_LEDS,
        .version = CONFIG_VERSION
    },

    .led_count = 16,

    .car_lights = (const CAR_LIGHT_T [16]) {{.always_on = 0}}
};


// ****************************************************************************
__attribute__ ((section(".light_programs")))
const LIGHT_PROGRAMS_T light_programs = {
    .magic = {
        .magic_value = ROM_MAGIC,
        .type = LIGHT_PROGRAMS,
        .version = CONFIG_VERSION
    },

    .number_of_programs = 2,
    .start = {
        &light_programs.programs[0],
        &light_programs.programs[9]
    },

    .programs = {
        RUN_WHEN_NORMAL_OPERATION,
        RUN_WHEN_HAZARD,
        0x00008000,
        OPCODE_SET + (15 << 16) + (15 << 8) + 0,
        OPCODE_WAIT + (40 / __SYSTICK_IN_MS),
        OPCODE_SET + (15 << 16) + (15 << 8) + 255,
        OPCODE_WAIT + (40 / __SYSTICK_IN_MS),
        OPCODE_GOTO + FIRST_OPCODE_OFFSET,
        OPCODE_END_OF_PROGRAM,

        RUN_WHEN_INITIALIZING,
        0x00000000,
        0x000000f0,
        OPCODE_SET + (7 << 16) + (4 << 8) + 50,
        OPCODE_WAIT + (1100 / __SYSTICK_IN_MS),
        OPCODE_SET + (7 << 16) + (4 << 8) + 125,
        OPCODE_WAIT + (60 / __SYSTICK_IN_MS),
        OPCODE_GOTO + FIRST_OPCODE_OFFSET,
        OPCODE_END_OF_PROGRAM,
        OPCODE_END_OF_PROGRAMS
    }
};

