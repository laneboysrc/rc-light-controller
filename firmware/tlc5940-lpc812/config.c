#include <stdint.h>
#include <stdbool.h>
#include <globals.h>

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

    .light_mode_mask = 0x0f,

    .ch3_multi_click_timeout = (260 / __SYSTICK_IN_MS),

    .winch_command_repeat_time = (1000 / __SYSTICK_IN_MS),

    .baudrate = 115200
};
