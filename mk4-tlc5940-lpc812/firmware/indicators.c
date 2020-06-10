/******************************************************************************

    Implements a sensible indicator algorithm.

    To turn on the indicators, throtte and steering must be centered for 2 s,
    then steering must be either left or right >50% for more than 2 s.

    Indicators are turned off when:
      - opposite steering is >30%
      - steering neutral or opposite for >2s

;******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include <globals.h>
#include <printf.h>


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
static uint16_t blink_counter;


// ****************************************************************************
static void synchronize_blinking(void)
{
    if (global_flags.blink_indicator_left ||
        global_flags.blink_indicator_right ||
        global_flags.blink_hazard) {
        return;
    }

    blink_counter = config.blink_counter_value;
    global_flags.blink_flag = true;
}


// ****************************************************************************
void set_blink_off(void)
{
    indicator_state = NOT_NEUTRAL;
    if (global_flags.blink_indicator_left || global_flags.blink_indicator_right) {
        fprintf(STDOUT_DEBUG, "indicator off\n");
    }

    global_flags.blink_indicator_left = false;
    global_flags.blink_indicator_right = false;
}


// ****************************************************************************
void set_blink_left(void)
{
    synchronize_blinking();
    indicator_state = BLINK_LEFT;
    global_flags.blink_indicator_left = true;
    global_flags.blink_indicator_right = false;
    fprintf(STDOUT_DEBUG, "indicator left\n");
}


// ****************************************************************************
void set_blink_right(void)
{
    synchronize_blinking();
    indicator_state = BLINK_RIGHT;
    global_flags.blink_indicator_left = false;
    global_flags.blink_indicator_right = true;
    fprintf(STDOUT_DEBUG, "indicator right\n");
}


// ****************************************************************************
void toggle_hazard_lights(void)
{
    synchronize_blinking();
    global_flags.blink_hazard = ~global_flags.blink_hazard;
    fprintf(STDOUT_DEBUG, "hazard %d\n", global_flags.blink_hazard);
}


// ****************************************************************************
void process_indicators(void)
{
    bool throttle_neutral;

    if (global_flags.systick) {
        if (indicator_timer) {
            --indicator_timer;
        }

        if (blink_counter == 0) {
            global_flags.blink_flag = ~global_flags.blink_flag;
            if (global_flags.blink_flag) {
                blink_counter = config.blink_counter_value;
            }
            else {
                blink_counter = config.blink_counter_value_dark;
            }
        }
        else {
            --blink_counter;
        }
    }

    if (!global_flags.new_channel_data) {
        return;
    }

    if (config.aux_function == INDICATORS ||
        config.aux2_function == INDICATORS ||
        config.aux3_function == INDICATORS ) {
        return;
    }

    if (config.flags2.indicators_while_driving) {
        // Simulate throttle being in neutral when indicators shall not depend
        // whether driving or not
        throttle_neutral = true;
    }
    else {
        throttle_neutral = (channel[TH].absolute <= config.centre_threshold_high);
    }

    switch (indicator_state) {
        // ---------------------------------
        case NOT_NEUTRAL:
            if (channel[ST].absolute > config.centre_threshold_low) {
                return;
            }

            if (!config.flags2.indicators_while_driving  &&
                channel[TH].absolute > config.centre_threshold_low) {
                return;
            }

            indicator_timer = config.indicator_idle_time_value;
            indicator_state = NEUTRAL_WAIT;
            return;

        // ---------------------------------
        case NEUTRAL_WAIT:
            if (channel[ST].absolute > config.centre_threshold_high) {
                set_blink_off();
                return;
            }

            if (!throttle_neutral) {
                set_blink_off();
                return;
            }

            if (indicator_timer == 0) {
                indicator_state = BLINK_ARMED;
            }
            return;

        // ---------------------------------
        case BLINK_ARMED:
            if (!throttle_neutral) {
                set_blink_off();
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
            if (!throttle_neutral) {
                set_blink_off();
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
            if (!throttle_neutral) {
                set_blink_off();
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
                set_blink_off();
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
                set_blink_off();
                return;
            }

            if (channel[ST].normalized < -config.blink_threshold) {
                indicator_state = BLINK_LEFT;
                return;
            }

            if (indicator_timer == 0) {
                set_blink_off();
            }
            return;

        // ---------------------------------
        case BLINK_RIGHT:
            if (channel[ST].normalized < -config.blink_threshold) {
                set_blink_off();
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
                set_blink_off();
                return;
            }

            if (channel[ST].normalized > config.blink_threshold) {
                indicator_state = BLINK_RIGHT;
                return;
            }

            if (indicator_timer == 0) {
                set_blink_off();
            }
            return;

        // ---------------------------------
        default:
            indicator_state = NOT_NEUTRAL;
    }
}


