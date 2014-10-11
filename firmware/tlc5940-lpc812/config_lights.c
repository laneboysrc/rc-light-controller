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
        RUN_WHEN_LIGHT_SWITCH_POSITION_2,
        LED_USED(15),
        OPCODE_SET(15, 15, 0),
        OPCODE_WAIT(40),
        OPCODE_SET(15, 15, 255),
        OPCODE_WAIT(40),
        OPCODE_GOTO(FIRST_OPCODE_OFFSET),
        OPCODE_END_OF_PROGRAM,

        RUN_WHEN_INITIALIZING,
        0x00000000,
        LED_USED(4) + LED_USED(5) + LED_USED(6) + LED_USED(7),
        OPCODE_SET(4, 7, 50),
        OPCODE_WAIT(1100),
        OPCODE_SET(4, 7, 150),
        OPCODE_WAIT(60),
        OPCODE_GOTO(FIRST_OPCODE_OFFSET),
        OPCODE_END_OF_PROGRAM,

        OPCODE_END_OF_PROGRAMS
    }
};

