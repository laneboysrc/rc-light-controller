#include <stdint.h>
#include <stdbool.h>
#include <globals.h>

const LIGHT_CONTROLLER_CONFIG_T config = {
    ROM_MAGIC,                  // magic
    0x01,                       // type
    0x01,                       // version

    MASTER_WITH_UART_READER,    // mode

    {                           // flags
        // If mode is MASTER_WITH_SERVO_READER  then all flags are mutually
        // exculsive.
        // If mode is MASTER_WITH_UART_READER then there can be one UART output
        // (slave, preprocessor or winch) and one servo output (steering wheel
        // or gearbox servo)
        false,                  // slave_output
        false,                  // preprocessor_output
        false,                  // winch_output
        false,                  // steering_wheel_servo_output
        false,                  // gearbox_servo_output

        false,                  // esc_forward_reverse
        false,                  // ch3_is_momentary

        true,                   // auto_brake_lights_forward_enabled
        true,                   // auto_brake_lights_reverse_enabled
        true,                   // brake_disarm_timeout_enabled
    },

    (800 / __SYSTICK_IN_MS),    // auto_brake_counter_value_forward_min
    (2500 / __SYSTICK_IN_MS),   // auto_brake_counter_value_forward_min
    (800 / __SYSTICK_IN_MS),    // auto_brake_counter_value_reverse_min
    (2500 / __SYSTICK_IN_MS),   // auto_brake_counter_value_reverse_min
    (800 / __SYSTICK_IN_MS),    // auto_reverse_counter_value_min
    (2000 / __SYSTICK_IN_MS),   // auto_reverse_counter_value_max
    (1000 / __SYSTICK_IN_MS),   // brake_disarm_counter_value

    (333 / __SYSTICK_IN_MS),    // blink_counter_value
    (500 / __SYSTICK_IN_MS),    // indicator_idle_time_value
    (2000 / __SYSTICK_IN_MS),   // indicator_off_timeout_value

    8,                          // centre_threshold_low
    10,                         // centre_threshold
    12,                         // centre_threshold_high
    50,                         // blink_threshold

    0x0f,                       // light_mode_mask

    (260 / __SYSTICK_IN_MS),    // ch3_multi_click_timeout

    (1000 / __SYSTICK_IN_MS),   // winch_command_repeat_time

    115200                       // baudrate of the UART
};
