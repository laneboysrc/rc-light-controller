#include <stdint.h>
#include <stdbool.h>
#include <globals.h>


// ****************************************************************************
const LIGHT_CONTROLLER_CONFIG_T config = {
    .magic = ROM_MAGIC,
    .type = 0x01,
    .version = 0x01,

    .mode = MASTER_WITH_UART_READER,

    .flags = {
        // If mode is MASTER_WITH_SERVO_READER then all *_output flags are
        // mutually exculsive.
        // If mode is MASTER_WITH_UART_READER then there can be one UART output
        // (slave, preprocessor or winch) and one servo output (steering wheel
        // or gearbox servo)
        .slave_output = false,
        .preprocessor_output = false,
        .winch_output = false,
        .steering_wheel_servo_output = false,
        .gearbox_servo_output = false,

        .esc_forward_reverse = false,
        .ch3_is_local_switch = false,
        .ch3_is_momentary = false,

        .auto_brake_lights_forward_enabled = true,
        .auto_brake_lights_reverse_enabled = true,
        .brake_disarm_timeout_enabled = true,
    },

    .auto_brake_counter_value_forward_min = (500 / __SYSTICK_IN_MS),
    .auto_brake_counter_value_forward_max = (2500 / __SYSTICK_IN_MS),
    .auto_brake_counter_value_reverse_min = (500 / __SYSTICK_IN_MS),
    .auto_brake_counter_value_reverse_max = (2500 / __SYSTICK_IN_MS),
    .auto_reverse_counter_value_min = (800 / __SYSTICK_IN_MS),
    .auto_reverse_counter_value_max = (2000 / __SYSTICK_IN_MS),
    .brake_disarm_counter_value = (1000 / __SYSTICK_IN_MS),

    .blink_counter_value = (333 / __SYSTICK_IN_MS),
    .indicator_idle_time_value = (500 / __SYSTICK_IN_MS),
    .indicator_off_timeout_value = (2000 / __SYSTICK_IN_MS),

    .centre_threshold_low = 8,
    .centre_threshold = 10,
    .centre_threshold_high = 12,
    .blink_threshold = 50,

    .light_switch_positions = 3,

    .initial_endpoint_delta = 250,

    .ch3_multi_click_timeout = (260 / __SYSTICK_IN_MS),

    .winch_command_repeat_time = (1000 / __SYSTICK_IN_MS),

    .baudrate = 115200
};


// ****************************************************************************
const CAR_LIGHT_T local_monochrome_leds = {
    .led_type = MONOCHROME,
    .car_lights = &(const MONOCHROME_CAR_LIGHT_T [16]) {
        // LED 0
        {.always_on = 255, .max_change_per_systick = 1},

        // LED 1
        {.light_switch_position[1] = 255, .light_switch_position[2] = 255},

        // LED 2
        {.light_switch_position[2] = 255},

        // LED 3
        {.tail_light = 255},

        // LED 4
        {.brake_light = 255},

        // LED 5
        {.always_on = 0}, // LED not present...

        // LED 6
        {.tail_light = 85, .brake_light = 255},

        // LED 7
        {.reversing_light = 255},

        // LED 8
        {.indicator_left = 255, .max_change_per_systick = 10},

        // LED 9
        {.indicator_left = 85, .tail_light = 85, .brake_light = 255},

        // LED 10
        {.indicator_right = 255, .max_change_per_systick = 42},

        // LED 11
        {.indicator_right = 85, .tail_light = 85, .brake_light = 255},

        // LED 12
        {.indicator_left = 255, .max_change_per_systick = 20},

        // LED 13
        {.indicator_left = 255, .max_change_per_systick = 10},

        // LED 14
        {.indicator_left = 255, .max_change_per_systick = 8},

        // LED 15
        {.indicator_left = 255, .max_change_per_systick = 8},
    }
};


// ****************************************************************************
const CAR_LIGHT_T slave_monochrome_leds = {
    .led_type = MONOCHROME,
    .car_lights = &(const MONOCHROME_CAR_LIGHT_T [16]) {{0}}
};


// ****************************************************************************
const uint8_t gamma_table[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4,
    4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 8, 8, 8, 9, 9, 10, 10, 10, 11, 11, 12, 12,
    13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 21, 21, 22, 22,
    23, 24, 24, 25, 26, 26, 27, 28, 28, 29, 30, 30, 31, 32, 32, 33, 34, 35, 35,
    36, 37, 38, 38, 39, 40, 41, 41, 42, 43, 44, 45, 46, 46, 47, 48, 49, 50, 51,
    52, 53, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
    70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 86, 87, 88, 89,
    90, 91, 92, 93, 95, 96, 97, 98, 99, 100, 102, 103, 104, 105, 107, 108, 109,
    110, 111, 113, 114, 115, 116, 118, 119, 120, 122, 123, 124, 126, 127, 128,
    129, 131, 132, 134, 135, 136, 138, 139, 140, 142, 143, 145, 146, 147, 149,
    150, 152, 153, 154, 156, 157, 159, 160, 162, 163, 165, 166, 168, 169, 171,
    172, 174, 175, 177, 178, 180, 181, 183, 184, 186, 188, 189, 191, 192, 194,
    195, 197, 199, 200, 202, 204, 205, 207, 208, 210, 212, 213, 215, 217, 218,
    220, 222, 224, 225, 227, 229, 230, 232, 234, 236, 237, 239, 241, 243, 244,
    246, 248, 250, 251, 253, 255
};
