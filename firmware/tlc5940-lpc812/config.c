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
        {.always_on = 63},

        // LED 1
        {.light_switch_position[1] = 63, .light_switch_position[2] = 63},

        // LED 2
        {.light_switch_position[2] = 63},

        // LED 3
        {.tail_light = 63},

        // LED 4
        {.brake_light = 63},

        // LED 5
        {.always_on = 0}, // LED not present...

        // LED 6
        {.tail_light = 5, .brake_light = 63},

        // LED 7
        {.reversing_light = 63},

        // LED 8
        {.indicator_left = 63},

        // LED 9
        {.indicator_right = 63},

        // LED 10
        {.indicator_left = 5, .tail_light = 5, .brake_light = 63},

        // LED 11
        {.indicator_right = 5, .tail_light = 5, .brake_light = 63},
    }
};

// ****************************************************************************
const CAR_LIGHT_T slave_monochrome_leds = {
    .led_type = MONOCHROME,
    .car_lights = &(const MONOCHROME_CAR_LIGHT_T [16]) {{0}}
};
