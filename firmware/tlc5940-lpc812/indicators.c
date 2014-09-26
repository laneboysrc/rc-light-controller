/******************************************************************************
; Process_indicators
;
; Implements a sensible indicator algorithm.
;
; To turn on the indicators, throtte and steering must be centered for 2 s,
; then steering must be either left or right >50% for more than 2 s.
;
; Indicators are turned off when:
;   - opposite steering is >30%
;   - steering neutral or opposite for >2s
;******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include <globals.h>
#include <reader.h>

static enum {
    NOT_NEUTRAL = 0,
    NEUTRAL_WAIT,
    BLINK_ARMED,
    BLINK_ARMED_LEFT,
    BLINK_ARMED_RIGHT,
    BLINK_LEFT,
    BLINK_LEFT_WAIT,
    BLINK_RIGHT,
    BLINK_RIGHT_WAIT
} indicator_state = NOT_NEUTRAL;

static uint16_t indicator_timer;


static void synchronize_blinking(void)
{
    if (global_flags.blink_indicator_left || 
        global_flags.blink_indicator_right || 
        global_flags.blink_hazard) {
        return;
    }

    // FIXME blink_timer = BLINK_TIMER_VALUE;
    global_flags.blink_flag = true;
}

static void set_not_neutral(void)
{
    indicator_state = NOT_NEUTRAL;
    global_flags.blink_indicator_left = false;
    global_flags.blink_indicator_right = false;
}


static void set_blink_left(void)
{
    indicator_state = BLINK_LEFT;
    synchronize_blinking();
    global_flags.blink_indicator_left = true;
}


static void set_blink_right(void)
{
    indicator_state = BLINK_RIGHT;
    synchronize_blinking();
    global_flags.blink_indicator_right = true;
}

void toggle_hazard_lights(void)
{
    synchronize_blinking();
    global_flags.blink_hazard = ~global_flags.blink_hazard;
}

void process_indicators(void)
{
    if (global_flags.systick) {
        if (indicator_timer) {
            --indicator_timer;
        }
    }

    if (!global_flags.new_channel_data) {
        return;
    }

    switch (indicator_state) {
        // ---------------------------------
        case NOT_NEUTRAL:
            if (channel[TH].absolute > config.centre_threshold ||
                channel[ST].absolute > config.centre_threshold) {
                return;
            }

            indicator_timer = config.indicator_idle_time_value;
            indicator_state = NEUTRAL_WAIT;
            return;

        // ---------------------------------
        case NEUTRAL_WAIT:
            if (channel[TH].absolute > config.centre_threshold ||
                channel[ST].absolute > config.centre_threshold) {
                set_not_neutral();
                return;
            }

            if (indicator_timer == 0) {
                indicator_state = BLINK_ARMED;
            }
            return;

        // ---------------------------------
        case BLINK_ARMED:
            if (channel[TH].absolute > config.centre_threshold) {
                set_not_neutral();
                return;
            }

            if (channel[ST].absolute < config.blink_threshold) {
                return;
            }

            indicator_timer = config.indicator_idle_time_value;
            indicator_state = (channel[ST].normalized < 0) ?
                BLINK_ARMED_LEFT : BLINK_ARMED_RIGHT;
            return;

        // ---------------------------------
        case BLINK_ARMED_LEFT:
            if (channel[TH].absolute > config.centre_threshold) {
                set_not_neutral();
                return;
            }

            // Note: we are testing here the +/- value of steering
            // to catch if the user quickly changed direction
            if (channel[ST].normalized > -config.blink_threshold) {
                indicator_state = BLINK_ARMED;
                return;
            }

            if (indicator_timer != 0) {
                set_blink_left();
            }
            return;

        // ---------------------------------
        case BLINK_ARMED_RIGHT:
            if (channel[TH].absolute > config.centre_threshold) {
                set_not_neutral();
                return;
            }

            // Note: we are testing here the +/- value of steering
            // to catch if the user quickly changed direction
            if (channel[ST].normalized < config.blink_threshold) {
                indicator_state = BLINK_ARMED;
                return;
            }

            if (indicator_timer == 0) {
                set_blink_right();
            }
            return;

        // ---------------------------------
        case BLINK_LEFT:
            if (channel[ST].normalized > config.blink_threshold) {
                set_not_neutral();
                return;
            }

            if (channel[ST].normalized > -config.blink_threshold) {
                indicator_timer = config.indicator_off_timeout_value;
                indicator_state = BLINK_LEFT_WAIT;
            }
            return;

        // ---------------------------------
        case BLINK_LEFT_WAIT:
            if (channel[ST].normalized > config.blink_threshold) {
                set_not_neutral();
                return;
            }

            if (channel[ST].normalized < -config.blink_threshold) {
                indicator_state = BLINK_LEFT;
                return;
            }

            if (indicator_timer == 0) {
                set_not_neutral();
            }
            return;

        // ---------------------------------
        case BLINK_RIGHT:
            if (channel[ST].normalized < -config.blink_threshold) {
                set_not_neutral();
                return;
            }

            if (channel[ST].normalized < config.blink_threshold) {
                indicator_timer = config.indicator_off_timeout_value;
                indicator_state = BLINK_RIGHT_WAIT;
            }
            return;

        // ---------------------------------
        case BLINK_RIGHT_WAIT:
            if (channel[ST].normalized < -config.blink_threshold) {
                set_not_neutral();
                return;
            }

            if (channel[ST].normalized > config.blink_threshold) {
                indicator_state = BLINK_RIGHT;
                return;
            }

            if (indicator_timer == 0) {
                set_not_neutral();
            }
            return;

        // ---------------------------------
        default:
            indicator_state = NOT_NEUTRAL;
    }
}


