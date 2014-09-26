#include <stdint.h>
#include <stdbool.h>
#include <globals.h>

const LIGHT_CONTROLLER_CONFIG_T config = {
    ROM_MAGIC,                  // magic
    0x01,                       // type
    0x01,                       // version

    {                           // flags
        false,                  //   esc_forward_reverse
        false,                  //   ch3_is_momentary

        true,                   //   auto_brake_lights_forward_enabled
        true,                   //   auto_brake_lights_reverse_enabled
        true,                   //   brake_disarm_timeout_enabled

        false,                  //   preprocessor_output_enabled
        true,                   //   steering_wheel_servo_output_enabled
        false,                  //   gearbox_servo_enabled
        false,                  //   winch_enabled
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

    260,                        // ch3_multi_click_timeout

    (1000 / __SYSTICK_IN_MS),   // winch_command_repeat_time
};
