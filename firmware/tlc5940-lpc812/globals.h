#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <stdint.h>

#define __SYSTICK_IN_MS 20

// The ROM_MAGIC marker is used to identify the location of ROM constants when
// parsing a light controller binary by an external tool
#define ROM_MAGIC 0x4c427263                // LBrc (LANE Boys RC) in hex

typedef struct {
    unsigned int systick : 1;               // Set for one mainloop every 20 ms
    unsigned int new_channel_data : 1;      // Set for one mainloop every time servo pulses were received
    unsigned int startup_mode_neutral : 1;
    unsigned int blink_flag : 1;            // Toggles with 1.5 Hz
    unsigned int blink_hazard : 1;          // Hazard lights active
    unsigned int blink_indicator_left : 1;  // Left indicator active
    unsigned int blink_indicator_right : 1; // Right indicator active
    unsigned int forward : 1;               // Set when the car is driving forward
    unsigned int braking : 1;               // Set when the brakes are enganged
    unsigned int reversing : 1;             // Set when the car is reversing
} GLOBAL_FLAGS_T;

extern GLOBAL_FLAGS_T global_flags;


typedef struct {
    uint32_t magic;
    uint16_t type;

    uint16_t auto_brake_counter_value_forward_min;
    uint16_t auto_brake_counter_value_forward_max;
    uint16_t auto_brake_counter_value_reverse_min;
    uint16_t auto_brake_counter_value_reverse_max;
    uint16_t auto_reverse_counter_value_min;
    uint16_t auto_reverse_counter_value_max;
    uint16_t brake_disarm_counter_value;

    // Centre threshold defines a range where we consider the servo being
    // centred. In order to prevent "flickering" especially for the brake and
    // reverse light the CENTRE_THRESHOLD_HIGH and CENTRE_THRESHOLD_LOW provide
    // a hysteresis that we apply to the throttle when processing drive_mode.
    uint16_t centre_threshold_low;
    uint16_t centre_threshold;
    uint16_t centre_threshold_high;
} LIGHT_CONTROLLER_CONFIG_T;

extern const LIGHT_CONTROLLER_CONFIG_T config;


#endif // __GLOBALS_H
