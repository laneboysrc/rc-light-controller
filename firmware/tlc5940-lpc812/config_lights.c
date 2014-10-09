#include <stdint.h>
#include <stdbool.h>
#include <globals.h>


const SETUP_LIGHTS_T setup_lights = {
    .magic = {
        .magic_value = ROM_MAGIC,
        .type = SETUP_LIGHTS,
        .version = CONFIG_VERSION
    },

    .no_signal = {255},
    .initializing = {0, 255},
    .reverse_setup_steering = {0, 0, 255},
    .reverse_setup_throttle = {0, 0, 0, 255},
    .servo_setup_left = {0, 0, 0, 0, 255},
    .servo_setup_centre = {0, 0, 0, 0, 0, 255},
    .servo_setup_right = {0, 0, 0, 0, 0, 0, 255}
};


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
        &light_programs.programs[8]
    },

    .programs = {
        0x00000001,
        0x0000000f,
        OPCODE_SET + (3 << 16) + (0 << 8) + 0,
        OPCODE_WAIT + (250 / __SYSTICK_IN_MS),
        OPCODE_SET + (3 << 16) + (0 << 8) + 255,
        OPCODE_WAIT + (250 / __SYSTICK_IN_MS),
        OPCODE_GOTO + 2,
        OPCODE_END_OF_PROGRAM,
        0x00000001,
        0x000000f0,
        OPCODE_SET + (7 << 16) + (4 << 8) + 50,
        OPCODE_WAIT + (1100 / __SYSTICK_IN_MS),
        OPCODE_SET + (7 << 16) + (4 << 8) + 125,
        OPCODE_WAIT + (60 / __SYSTICK_IN_MS),
        OPCODE_GOTO + 2,
        OPCODE_END_OF_PROGRAM,
        OPCODE_END_OF_PROGRAMS
    }
};

