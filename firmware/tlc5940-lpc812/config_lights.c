#include <globals.h>

// ****************************************************************************
// Default configuration of the light outputs
const CAR_LIGHT_ARRAY_T local_leds = {
    .magic = {
        .magic_value = ROM_MAGIC,
        .type = LOCAL_LEDS,
        .version = CONFIG_VERSION
    },

    .led_count = 16,

    .car_lights = (const CAR_LIGHT_T [16]) {
        // LED 0   Parking / position lights
        {   .light_switch_position[1] = 255,
            .light_switch_position[2] = 255,
            .light_switch_position[3] = 255,
            .light_switch_position[4] = 255
        },

        // LED 1    Parking / position lights
        {   .light_switch_position[1] = 255,
            .light_switch_position[2] = 255,
            .light_switch_position[3] = 255,
            .light_switch_position[4] = 255
        },

        // LED 2    Main beam
        {   .light_switch_position[2] = 255,
            .light_switch_position[3] = 255,
            .light_switch_position[4] = 255
        },

        // LED 3    Main beam
        {   .light_switch_position[2] = 255,
            .light_switch_position[3] = 255,
            .light_switch_position[4] = 255
        },

        // LED 4    High beam
        {.light_switch_position[3] = 255, .light_switch_position[4] = 255},

        // LED 5    High beam
        {.light_switch_position[3] = 255, .light_switch_position[4] = 255},

        // LED 6    Indicator front left
        {.indicator_left = 255},

        // LED 7    Indicator front right
        {.indicator_right = 255},

        // LED 8    Brake/tail light
        {.tail_light = 84, .brake_light = 255},

        // LED 9    Brake/tail light
        {.tail_light = 84, .brake_light = 255},

        // LED 10   Reversing light
        {.reversing_light = 255},

        // LED 11   Reversing light
        {.reversing_light = 255},

        // LED 12   Indicator rear left
        {.indicator_left = 255},

        // LED 13   Indicator rear right
        {.indicator_right = 255},

        // LED 14   3rd brake light
        {.brake_light = 255},

        // LED 15   Roof lights (switched light output)
        {.light_switch_position[4] = 255},

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
