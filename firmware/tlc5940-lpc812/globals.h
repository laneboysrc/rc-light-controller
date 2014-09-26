#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <stdint.h>


#define __SYSTICK_IN_MS 20

// The ROM_MAGIC marker is used to identify the location of ROM constants when
// parsing a light controller binary by an external tool
#define ROM_MAGIC 0x6372424c                // LBrc (LANE Boys RC) in little endian


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
    unsigned int gear_changed : 1;          // Set when a new gear was selected
} GLOBAL_FLAGS_T;

typedef struct {
    uint32_t magic;
    uint16_t type;
    uint16_t version;

    struct {
        unsigned int esc_forward_reverse : 1;
        unsigned int ch3_is_momentary : 1;

        unsigned int auto_brake_lights_forward_enabled : 1;
        unsigned int auto_brake_lights_reverse_enabled : 1;

        // If ENABLE_BRAKE_DISARM_TIMEOUT is not set, the user has to go for
        // brake, then neutral, before reverse engages. Otherwise reverse
        // engages if the user stays in neutral for a few seconds.
        //
        // Tamiya ESC need this ENABLE_BRAKE_DISARM_TIMEOUT cleared.
        // The China ESC and HPI SC-15WP need ENABLE_BRAKE_DISARM_TIMEOUT set.
        unsigned int brake_disarm_timeout_enabled : 1;

        unsigned int preprocessor_output_enabled : 1;
        unsigned int steering_wheel_servo_output_enabled : 1;
        unsigned int gearbox_servo_enabled : 1;
        unsigned int winch_enabled : 1;
    } flags;

    uint16_t auto_brake_counter_value_forward_min;
    uint16_t auto_brake_counter_value_forward_max;
    uint16_t auto_brake_counter_value_reverse_min;
    uint16_t auto_brake_counter_value_reverse_max;
    uint16_t auto_reverse_counter_value_min;
    uint16_t auto_reverse_counter_value_max;
    uint16_t brake_disarm_counter_value;

    uint16_t indicator_idle_time_value;
    uint16_t indicator_off_timeout_value;

    // Centre threshold defines a range where we consider the servo being
    // centred. In order to prevent "flickering" especially for the brake and
    // reverse light the CENTRE_THRESHOLD_HIGH and CENTRE_THRESHOLD_LOW provide
    // a hysteresis that we apply to the throttle when processing drive_mode.
    uint16_t centre_threshold_low;
    uint16_t centre_threshold;
    uint16_t centre_threshold_high;
    uint16_t blink_threshold;

    uint16_t light_mode_mask;
    uint16_t ch3_multi_click_timeout;
} LIGHT_CONTROLLER_CONFIG_T;

typedef enum {
    SETUP_MODE_OFF = 0,
    SETUP_MODE_INIT = 0x01,
    SETUP_MODE_CENTRE = 0x02,
    SETUP_MODE_LEFT = 0x04,
    SETUP_MODE_RIGHT = 0x08,
    SETUP_MODE_STEERING_REVERSE = 0x10,
    SETUP_MODE_THROTTLE_REVERSE = 0x20,
    SETUP_MODE_NEXT = 0x40,
    SETUP_MODE_CANCEL = 0x80
} SETUP_MODE_T;

typedef enum {
    WINCH_MODE_DISABLED = 0,
    WINCH_MODE_IDLE = 0x01,
    WINCH_MODE_IN = 0x02,
    WINCH_MODE_OUT = 0x04
} WINCH_MODE_T;


// The entropy variable is incremented every mainloop. It can therefore serve
// as a random value in practical RC car application,
// Certainly not suitable for secure implementations...
extern uint32_t entropy;

extern uint16_t light_mode;
extern SETUP_MODE_T setup_mode;
extern WINCH_MODE_T winch_mode;
extern GLOBAL_FLAGS_T global_flags;
extern const LIGHT_CONTROLLER_CONFIG_T config;

#endif // __GLOBALS_H
