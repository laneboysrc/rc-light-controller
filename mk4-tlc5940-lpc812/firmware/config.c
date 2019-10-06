#include <stdint.h>
#include <stdbool.h>

#include <globals.h>


// ****************************************************************************
const LIGHT_CONTROLLER_CONFIG_T config = {
    .magic = {
        .magic_value = ROM_MAGIC,
        .type = CONFIG_SECTION,
        .version = CONFIG_VERSION
    },

    .firmware_version = 21,

    .mode = MASTER_WITH_UART_READER,
    .esc_mode = ESC_FORWARD_BRAKE_REVERSE_TIMEOUT,

    .flags = {
        // If mode is MASTER_WITH_SERVO_READER then all *_output flags are
        // mutually exculsive.
        // If mode is MASTER_WITH_UART_READER then
        // there can be one UART output (slave, preprocessor or winch) and
        // one servo output (steering wheel or gearbox servo)
        .slave_output = false,
        .preprocessor_output = false,
        .winch_output = false,
        .steering_wheel_servo_output = true,
        .gearbox_servo_output = false,

        .ch3_is_local_switch = false,
        .ch3_is_momentary = false,
        .ch3_is_two_button = false,

        .auto_brake_lights_forward_enabled = true,
        .auto_brake_lights_reverse_enabled = true,
    },

    .flags2 = {
        .multi_aux = false,
        .shelf_queen_mode = true,
        .us_style_combined_lights = true,
    },

    .auto_brake_counter_value_forward_min = (500 / __SYSTICK_IN_MS),
    .auto_brake_counter_value_forward_max = (2500 / __SYSTICK_IN_MS),
    .auto_brake_counter_value_reverse_min = (500 / __SYSTICK_IN_MS),
    .auto_brake_counter_value_reverse_max = (2500 / __SYSTICK_IN_MS),
    .auto_reverse_counter_value_min = (800 / __SYSTICK_IN_MS),
    .auto_reverse_counter_value_max = (2000 / __SYSTICK_IN_MS),
    .brake_disarm_counter_value = (1000 / __SYSTICK_IN_MS),

    .blink_counter_value = (340 / __SYSTICK_IN_MS),
    .blink_counter_value_dark = (340 / __SYSTICK_IN_MS),

    .indicator_idle_time_value = (500 / __SYSTICK_IN_MS),
    .indicator_off_timeout_value = (2000 / __SYSTICK_IN_MS),

    .centre_threshold_low = 8,
    .centre_threshold_high = 12,
    .blink_threshold = 30,

    .light_switch_positions = 5,
    .initial_light_switch_position = 0,

    .initial_endpoint_delta = 250,

    .ch3_multi_click_timeout = (500 / __SYSTICK_IN_MS),

    .winch_command_repeat_time = (1000 / __SYSTICK_IN_MS),

    .baudrate = 115200,
    .no_signal_timeout = (500 / __SYSTICK_IN_MS),

    .number_of_gears = 2,
    .gearbox_servo_active_time = (1000 / __SYSTICK_IN_MS),
    .gearbox_servo_idle_time = (9000 / __SYSTICK_IN_MS),

    .servo_pulse_min = 600,
    .servo_pulse_max = 2500,

    .startup_time = (2000 / __SYSTICK_IN_MS),

    .aux_type = TWO_POSITION,
    .aux_function = MULTI_FUNCTION,

    .aux2_type = TWO_POSITION,
    .aux2_function = NOT_USED,

    .aux3_type = TWO_POSITION,
    .aux3_function = NOT_USED,

    .light_switch_centers = {-80, -40, 0, 40, 80, 0, 0, 0, 0},
    .light_switch_hysteresis = 40 / 4,
};


// ****************************************************************************
// Gamma 2.2, created with tools/print_gamma_correction_table.py
const GAMMA_TABLE_T gamma_table = {
    .magic = {
        .magic_value = ROM_MAGIC,
        .type = GAMMA_TABLE,
        .version = CONFIG_VERSION
    },

    .gamma_value = "1.8",
    .gamma_table = {
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3,
        4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 8, 8, 8, 9, 9, 10, 10, 10, 11, 11,
        12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 21,
        21, 22, 22, 23, 24, 24, 25, 26, 26, 27, 28, 28, 29, 30, 30, 31, 32, 32,
        33, 34, 35, 35, 36, 37, 38, 38, 39, 40, 41, 41, 42, 43, 44, 45, 46, 46,
        47, 48, 49, 50, 51, 52, 53, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
        64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81,
        82, 83, 84, 86, 87, 88, 89, 90, 91, 92, 93, 95, 96, 97, 98, 99, 100,
        102, 103, 104, 105, 107, 108, 109, 110, 111, 113, 114, 115, 116, 118,
        119, 120, 122, 123, 124, 126, 127, 128, 129, 131, 132, 134, 135, 136,
        138, 139, 140, 142, 143, 145, 146, 147, 149, 150, 152, 153, 154, 156,
        157, 159, 160, 162, 163, 165, 166, 168, 169, 171, 172, 174, 175, 177,
        178, 180, 181, 183, 184, 186, 188, 189, 191, 192, 194, 195, 197, 199,
        200, 202, 204, 205, 207, 208, 210, 212, 213, 215, 217, 218, 220, 222,
        224, 225, 227, 229, 230, 232, 234, 236, 237, 239, 241, 243, 244, 246,
        248, 250, 251, 253, 255
    }
};
