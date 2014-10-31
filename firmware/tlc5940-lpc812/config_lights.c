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
        {   .light_switch_position[1] = 255,
            .light_switch_position[2] = 255,
            .light_switch_position[3] = 255,
            .light_switch_position[4] = 255
        },

        // LED 1
        {   .light_switch_position[1] = 255,
            .light_switch_position[2] = 255,
            .light_switch_position[3] = 255,
            .light_switch_position[4] = 255
        },

        // LED 2
        {   .light_switch_position[2] = 255,
            .light_switch_position[3] = 255,
            .light_switch_position[4] = 255
        },

        // LED 3
        {   .light_switch_position[2] = 255,
            .light_switch_position[3] = 255,
            .light_switch_position[4] = 255
        },

        // LED 4
        {.light_switch_position[3] = 255, .light_switch_position[4] = 255},

        // LED 5
        {.light_switch_position[3] = 255, .light_switch_position[4] = 255},

        // LED 6
        {.indicator_left = 255},

        // LED 7
        {.indicator_right = 255},

        // LED 8
        {.light_switch_position[4] = 255},

        // LED 9
        {.brake_light = 255},

        // LED 10
        {.tail_light = 84, .brake_light = 255},

        // LED 11
        {.tail_light = 84, .brake_light = 255},

        // LED 12
        {.reversing_light = 255},

        // LED 13
        {.reversing_light = 255},

        // LED 14
        {.indicator_left = 255},

        // LED 15
        {.indicator_right = 255},
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
