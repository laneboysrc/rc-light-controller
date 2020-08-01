/******************************************************************************

    Simulates the state machine in the ESC and updates the variable drive_mode
    accordingly.

    +/-10: forward = 0, reverse = 0
    >+10: forward = 1, brake_armed = 1
    <-10:
      if brake_armed: brake = 1
      if not brake_armed: reverse = 1, brake = 0
    2 seconds in Neutral: brake_armed = 0
    Brake -> Neutral: brake = 0, brake_armed = 0
    Reverse -> Neutral: brake = 1 for 2 seconds

******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include <globals.h>
#include <printf.h>


static uint16_t throttle_threshold = 0xffff;    // Signify uninitialized value
static uint16_t brake_disarm_counter;
static uint16_t auto_brake_counter;
static uint16_t auto_reverse_counter;

static struct {
    unsigned int brake_disarm : 1;
    unsigned int auto_brake : 1;
    unsigned int auto_reverse : 1;
    unsigned int brake_armed : 1;
} drive_mode;


// ****************************************************************************
void throttle_neutral(void)
{
    throttle_threshold = config.centre_threshold_high;
    if (global_flags.forward) {
        global_flags.forward = false;

        // Handle ESC where reverse can be engaged after a certain time in
        // neutral
        if (config.esc_mode == ESC_FORWARD_BRAKE_REVERSE_TIMEOUT) {
            drive_mode.brake_disarm = true;
            brake_disarm_counter = config.brake_disarm_counter_value;
        }

        if (config.flags.auto_brake_lights_forward_enabled) {
            global_flags.braking = true;
            // The time the brake lights stay on after going back to neutral
            // is random
            drive_mode.auto_brake = true;
            auto_brake_counter = random_min_max(
                config.auto_brake_counter_value_forward_min,
                config.auto_brake_counter_value_forward_max);
        }
    }
    else if (global_flags.reversing) {
        if (!drive_mode.auto_reverse) {
            drive_mode.auto_reverse = true;
            auto_reverse_counter = random_min_max(
                config.auto_reverse_counter_value_min,
                config.auto_reverse_counter_value_max);

            if (config.flags.auto_brake_lights_reverse_enabled) {
                global_flags.braking = true;
                drive_mode.auto_brake = true;
                auto_brake_counter = random_min_max(
                    config.auto_brake_counter_value_reverse_min,
                    config.auto_brake_counter_value_reverse_max);
            }
        }
    }
    else if (global_flags.braking) {
        if (!drive_mode.auto_brake) {
            drive_mode.brake_armed = false;
            global_flags.braking = false;
        }
    }
}


// ****************************************************************************
static void throttle_brake_or_reverse(void)
{
    if (config.esc_mode == ESC_FORWARD_BRAKE  ||  drive_mode.brake_armed) {
        global_flags.braking = true;
        global_flags.forward = false;
        global_flags.reversing = false;
    }
    else {
        global_flags.reversing = true;
        global_flags.braking = false;
        global_flags.forward = false;
        drive_mode.auto_reverse = false;
    }
}


// ****************************************************************************
static void throttle_not_neutral(void)
{
    throttle_threshold = config.centre_threshold_low;
    drive_mode.auto_brake = false;
    drive_mode.brake_disarm = false;

    if (channel[TH].normalized < 0) {
        throttle_brake_or_reverse();
    }
    else {
        global_flags.forward = true;
        global_flags.reversing = false;
        global_flags.braking = false;
        if (config.esc_mode != ESC_FORWARD_REVERSE) {
           drive_mode.brake_armed = true;
        }
    }
}


// ****************************************************************************
void process_drive_mode(void)
{
    static GLOBAL_FLAGS_T last_flags;

    if (global_flags.systick) {
        if (drive_mode.brake_disarm) {
            if (--brake_disarm_counter == 0) {
                drive_mode.brake_disarm = false;
                drive_mode.brake_armed = false;
            }
        }

        if (drive_mode.auto_brake) {
            if (--auto_brake_counter == 0) {
                drive_mode.auto_brake = false;
                global_flags.braking = false;
            }
        }

        if (drive_mode.auto_reverse) {
            if (--auto_reverse_counter == 0) {
                drive_mode.auto_reverse = false;
                global_flags.reversing = false;
            }
        }
    }

    if (global_flags.new_channel_data) {
        // Initialization as the compile complains that config.* is not static.
        if (throttle_threshold == 0xffff) {
            throttle_threshold = config.centre_threshold_high;
        }

        if (channel[TH].absolute < throttle_threshold) {
            // We are in neutral
            throttle_neutral();
        }
        else {
            throttle_not_neutral();
        }
    }

    if (last_flags.braking != global_flags.braking) {
        last_flags.braking = global_flags.braking;
        printf("brake %d\n", global_flags.braking);
    }

    if (last_flags.reversing != global_flags.reversing) {
        last_flags.reversing = global_flags.reversing;
        printf("reverse %d\n", global_flags.reversing);
    }
}


