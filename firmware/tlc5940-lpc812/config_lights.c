#include <stdint.h>
#include <stdbool.h>
#include <globals.h>


// ****************************************************************************
const CAR_LIGHT_T local_monochrome_leds = {
    .magic = {
        .magic_value = ROM_MAGIC,
        .type = LOCAL_MONOCHROME_LEDS,
        .version = CONFIG_VERSION
    },

    .led_type = MONOCHROME,
    .led_count = 16,

    .car_lights = &(const MONOCHROME_CAR_LIGHT_T [16]) {
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
        {.always_on = 0},
    }
};


// ****************************************************************************
const CAR_LIGHT_T slave_monochrome_leds = {
    .magic = {
        .magic_value = ROM_MAGIC,
        .type = SLAVE_MONOCHROME_LEDS,
        .version = CONFIG_VERSION
    },

    .led_type = MONOCHROME,
    .led_count = 16,

    .car_lights = &(const MONOCHROME_CAR_LIGHT_T [16]) {{.always_on = 0}}
};


// ****************************************************************************
const CAR_LIGHT_T local_rgb_leds = {
    .magic = {
        .magic_value = ROM_MAGIC,
        .type = LOCAL_RGB_LEDS,
        .version = CONFIG_VERSION
    },

    .led_type = RGB,
    .led_count = 32,

    .car_lights = &(const RGB_CAR_LIGHT_T [32]) {{.always_on = {0, 0, 0}}}
};


// ****************************************************************************
const CAR_LIGHT_T slave_rgb_leds = {
    .magic = {
        .magic_value = ROM_MAGIC,
        .type = SLAVE_RGB_LEDS,
        .version = CONFIG_VERSION
    },

    .led_type = RGB,
    .led_count = 32,

    .car_lights = &(const RGB_CAR_LIGHT_T [32]) {{.always_on = {0, 0, 0}}}
};
