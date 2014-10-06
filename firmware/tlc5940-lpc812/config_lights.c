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


