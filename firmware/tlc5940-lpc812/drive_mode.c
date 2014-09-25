/******************************************************************************
; Process_drive_mode
;
; Simulates the state machine in the ESC and updates the variable drive_mode
; accordingly.
;
; Currently programmed for the HPI SC-15WP
;
; +/-10: forward = 0, reverse = 0
; >+10: forward = 1, brake_armed = 1
; <-10:
;   if brake_armed: brake = 1
;   if not brake_armed: reverse = 1, brake = 0
; 2 seconds in Neutral: brake_armed = 0
; Brake -> Neutral: brake = 0, brake_armed = 0
; Reverse -> Neutral: brake = 1 for 2 seconds
;******************************************************************************/
#include <stdio.h>
#include <stdbool.h>

#include <globals.h>
#include <reader.h>

// Centre threshold defines a range where we consider the servo being centred.
// In order to prevent "flickering" especially for the brake and reverse light
// the CENTRE_THRESHOLD_HIGH and CENTRE_THRESHOLD_LOW provide a hysteresis
// that we apply to the throttle when processing drive_mode.
#define CENTRE_THRESHOLD_LOW 8
#define CENTRE_THRESHOLD 10
#define CENTRE_THRESHOLD_HIGH 12

#ifndef AUTO_BRAKE_COUNTER_VALUE_REVERSE_MIN
#define AUTO_BRAKE_COUNTER_VALUE_REVERSE_MIN 12
#endif
#ifndef AUTO_BRAKE_COUNTER_VALUE_REVERSE_MAX
#define AUTO_BRAKE_COUNTER_VALUE_REVERSE_MAX 38
#endif
#ifndef AUTO_BRAKE_COUNTER_VALUE_FORWARD_MIN
#define AUTO_BRAKE_COUNTER_VALUE_FORWARD_MIN 12
#endif
#ifndef AUTO_BRAKE_COUNTER_VALUE_FORWARD_MAX
#define AUTO_BRAKE_COUNTER_VALUE_FORWARD_MAX 38
#endif
#ifndef AUTO_REVERSE_COUNTER_VALUE_MIN
#define AUTO_REVERSE_COUNTER_VALUE_MIN 12
#endif
#ifndef AUTO_REVERSE_COUNTER_VALUE_MAX
#define AUTO_REVERSE_COUNTER_VALUE_MAX 30
#endif

#define BRAKE_DISARM_COUNTER_VALUE (1000 / 20)

static uint32_t throttle_threshold = CENTRE_THRESHOLD_HIGH;
static uint32_t brake_disarm_counter;
static uint32_t auto_brake_counter;
static uint32_t auto_reverse_counter;

static struct {
    unsigned int brake_disarm : 1; 
    unsigned int auto_brake : 1;
    unsigned int auto_reverse : 1;
    unsigned int brake_armed : 1;
} drive_mode;


uint32_t random_min_max(uint32_t min, uint32_t max)
{
    return min;
}


static void throttle_neutral(void) 
{
    throttle_threshold = CENTRE_THRESHOLD_HIGH;
    if (global_flags.forward) {
        global_flags.forward = false;

        // If DISABLE_BRAKE_DISARM_TIMEOUT is defined the user has to go for 
        // brake, then neutral, before reverse engages. Otherwise reverse engages
        // if the user stays in neutral for a few seconds.
        //
        // Tamiya ESC need this DISABLE_BRAKE_DISARM_TIMEOUT defined.
        // The China ESC and HPI SC-15WP need DISABLE_BRAKE_DISARM_TIMEOUT undefined.     
#ifndef DISABLE_BRAKE_DISARM_TIMEOUT
        drive_mode.brake_disarm = true;
        brake_disarm_counter = BRAKE_DISARM_COUNTER_VALUE;
#endif

#ifndef DISABLE_AUTO_BRAKE_LIGHTS_FORWARD   
        global_flags.braking = true;
        // The time the brake lights stay on after going back to neutral 
        // is random
        auto_brake_counter = random_min_max(
            AUTO_BRAKE_COUNTER_VALUE_FORWARD_MIN,
            AUTO_BRAKE_COUNTER_VALUE_FORWARD_MAX);
#endif
    }
    else if (global_flags.reversing) {
        if (!drive_mode.auto_reverse) {
            drive_mode.auto_reverse = true;
            auto_reverse_counter = random_min_max(
            AUTO_REVERSE_COUNTER_VALUE_MIN,
            AUTO_REVERSE_COUNTER_VALUE_MAX);

#ifndef DISABLE_AUTO_BRAKE_LIGHTS_REVERSE   
            global_flags.braking = true;
            auto_brake_counter = random_min_max(
                AUTO_BRAKE_COUNTER_VALUE_REVERSE_MIN,
                AUTO_BRAKE_COUNTER_VALUE_REVERSE_MAX);
#endif    
        }
    }
    else if (global_flags.braking) {
        if (!drive_mode.auto_brake) {
            drive_mode.brake_armed = false;
            global_flags.braking = false;
        }
    }
}


static void throttle_brake_or_reverse(void)
{
    if (!drive_mode.brake_armed) {
        global_flags.reversing = true;
        global_flags.braking = false;
        global_flags.forward = false;
        drive_mode.auto_reverse = false;
    }
    else {
        global_flags.braking = true;
        global_flags.forward = false;
        global_flags.reversing = false;
    }
}


static void throttle_not_neutral(void)
{
    throttle_threshold = CENTRE_THRESHOLD_LOW;
    drive_mode.auto_brake = false;
    drive_mode.brake_disarm = false;
    
    if (channel[TH].normalized < 0) {
        throttle_brake_or_reverse();
    }
    else {
        global_flags.forward = true;
        global_flags.reversing = false;
        global_flags.braking = false;
#ifndef ESC_FORWARD_REVERSE
        drive_mode.brake_armed = true;        
#endif         
    }
}


void process_drive_mode(void)
{
    if (global_flags.soft_timer) {
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

    if (!global_flags.new_channel_data) {
        return;
    }
        
    if (channel[TH].absolute < throttle_threshold) {
        // We are in neutral
        throttle_neutral();
    }
    else {
        throttle_not_neutral();
    }    
}


