/******************************************************************************

This function handles the AUX channels to determine which actions to invoke.

Originally it was designed for a single AUX channel, driven by a
two-position switch. The switch can either be momentary (e.g Futaba 4PL) or
static (HK-310, GT3B ...).

It also supports push-buttons directly connected between to the AUX input
and Ground.

Later throughout the project the functionality was extended to support up to
three AUX channels (AUX, AUX2, AUX3). Each AUX channel can be configured for
different actuators (analog input, two-position switch, three-position switch ...)
and can be assigned to different functions (manual light switch, indicators,
gearbox, winch ...)

******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include <hal.h>
#include <globals.h>
#include <printf.h>

// This value must be a number higher than any number of clicks we want to
// process.
#define IGNORE_CLICK_COUNT 99

extern uint8_t light_switch_position;

static bool initialized;

static struct AUX_FLAGS {
    unsigned int last_state : 4;
    // unsigned int transitioned : 1;
    int16_t last_value;
} aux_flags[3];

static uint8_t clicks;
static uint16_t click_counter;


// ****************************************************************************
static void process_click_timeout(void)
{
    if (clicks == 0) {              // Any clicks pending?
        return;                     // No: nothing to do
    }

    if (click_counter != 0) {       // Double-click timer expired?
        return;                     // No: wait for more buttons
    }

    printf("click_timeout\n");

    // If require_extra_click is set, require one more click than usual.
    // This implies that 1 click has no function (except abort winching!).
    // Use-case: Traxxas TRX4 where the gearbox and light controller share
    // one channel. Then the user can switch "slow" to change the gear,
    // and fast (2 or more clicks) to operate the lights.
    if (config.flags2.require_extra_click) {
        --clicks;
    }

    // ####################################
    // At this point we have detected one of more clicks and need to
    // perform the appropriate action.

    if (global_flags.servo_output_setup != SERVO_OUTPUT_SETUP_OFF) {
        servo_output_setup_action(clicks);
    }
    // else if (global_flags.winch_mode != WINCH_DISABLED) {
    //     winch_action(clicks);
    // }
    else if (global_flags.reversing_setup != REVERSING_SETUP_OFF) {
        reversing_setup_action(clicks);
    }
    else {
        // ====================================
        // Normal operation:
        // Neither winch nor setup nor reversing setup is active
        switch (clicks) {
            case 1:
                // --------------------------
                // Single click
                if (config.flags.gearbox_servo_output && !config.flags2.gearbox_light_program_control) {
                    gearbox_action(clicks);
                }
                else {
                    light_switch_up();
                }
                break;

            case 2:
                // --------------------------
                // Double click
                if (config.flags.gearbox_servo_output && !config.flags2.gearbox_light_program_control) {
                    gearbox_action(clicks);
                }
                else {
                    light_switch_down();
                }
                break;

            case 3:
                // --------------------------
                // 3 clicks: all lights on/off
                toggle_light_switch();
                break;

            case 4:
                // --------------------------
                // 4 clicks: Hazard lights on/off
                set_hazard_lights(!global_flags.blink_hazard);
                break;

            // case 5:
            //     // --------------------------
            //     // 5 clicks: Arm/disarm the winch
            //     winch_action(clicks);
            //     break;

            case 6:
                // --------------------------
                // 6 clicks: Increment sequencer pattern selection
                next_light_sequence();
                break;

            case 7:
                // --------------------------
                // 7 clicks: Enter channel reversing setup mode
                reversing_setup_action(clicks);
                break;

            case 8:
                // --------------------------
                // 8 clicks: Enter steering wheel servo setup mode
                servo_output_setup_action(clicks);
                break;

            default:
                break;
        }
    }

    clicks = 0;
}


// ****************************************************************************
static void add_click(void)
{
    printf("click\n");

    // If the winch is running any movement of CH3 immediately turns off
    // the winch (without waiting for click timeout!)
    // if (abort_winching()) {
    //     // If winching was aborted disable this series of clicks by setting
    //     // the click count to an unused high value
    //     clicks = IGNORE_CLICK_COUNT;
    // }

    ++clicks;
    click_counter = config.ch3_multi_click_timeout;
}


// ****************************************************************************
static void multi_function(CHANNEL_T *c, struct AUX_FLAGS *f, AUX_TYPE_T type)
{
    if (f->last_state) {
        if (c->normalized < config.aux_centre_threshold_low) {
            f->last_state = false;

            // If we are dealing with a momentary push button ignore the
            // release of the button
            if (type == MOMENTARY) {
                return;
            }

            // If we have a transmitter with a two position CH3 that uses two
            // buttons to switch between the positions (like the
            // HobbyKing X3S or the Tactic TTC300) then we ignore the first
            // click of the 'down' button is pressed.
            // This way the user can deterministically enter a number of clicks
            // without having to remember if the last command ended with the
            // up or down button.
            //
            // The disadvantage is that always an additional down button press
            // has to be carried out.
            if (type != TWO_POSITION_UP_DOWN || click_counter) {
                add_click();
            }
        }
    }
    else {
        if (c->normalized > config.aux_centre_threshold_high) {
            f->last_state = true;
            add_click();
        }
    }
}


// ****************************************************************************
static void hazard(CHANNEL_T *c, struct AUX_FLAGS *f, AUX_TYPE_T type)
{
    // On/off control for the hazard lights, works with all AUX types but
    // needs special handling for momentary functions

    if (f->last_state) {
        if (c->normalized < config.aux_centre_threshold_low) {
            f->last_state = false;
            if (type != MOMENTARY) {
                set_hazard_lights(OFF);
            }
        }
    }
    else {
        if (c->normalized > config.aux_centre_threshold_high) {
            f->last_state = true;
            if (type != MOMENTARY) {
                set_hazard_lights(ON);
            }
            else {
                set_hazard_lights(!global_flags.blink_hazard);
            }
        }
    }
}


// ****************************************************************************
static void servo(CHANNEL_T *c)
{
    // Direct servo control: put the incoming servo signal value directly
    // on the servo output.

    uint16_t servo_pulse;

    servo_pulse = 1000 + 5 * (c->normalized + 100);
    set_servo_pulse(servo_pulse);
}


// ****************************************************************************
static void manual_indicators(CHANNEL_T *c, struct AUX_FLAGS *f)
{
    int16_t new_value = f->last_value;

    if (c->normalized > config.aux_centre_right_threshold_high) {
        new_value = 100;
    }
    else if (c->normalized < config.aux_left_centre_threshold_low) {
        new_value = -100;
    }
    else if (f->last_value == -100 &&
             c->normalized > config.aux_left_centre_threshold_high) {
        new_value = 0;
    }
    else if (f->last_value == 100 &&
             c->normalized < config.aux_centre_right_threshold_low) {
        new_value = 0;
    }


    if (new_value != f->last_value) {
        f->last_value = new_value;

        if (new_value == 0) {
            set_blink_off();
        }
        else if (new_value > 0) {
            set_blink_right();
        }
        else {
            set_blink_left();
        }
    }
}


// ****************************************************************************
static void light_switch(CHANNEL_T *c, struct AUX_FLAGS *f)
{
/*
    If normalized > current_light_switch_equivalent
      subtract hysteresis to normalized (clamp to -100)
    else
      add hysteresis to normalized (clamp to 100)

    Hysteresis must be smaller than half of one light switch position width

    |     |     |     |     |     |     |
       x     x     x     x     x     x

    total range: 200
    one_light_switch_position = 200 / #light_switch_positions
    max #light_switch_positions = 9
    => min one_light_switch_position = 22
    => we choose hysteresis = one_light_position / 4
    current_light_switch_equivalent =
        current_light_switch_position * one_light_switch_position + one_light_switch_position / 2

    Shall we let the configurator pre-calculate some values?
    We could have an array of 9 values, each for one light switch position center
    Then the code would have nothing to calculate, just compare

    current_light_switch_equivalent = table[current_light_switch_position]

*/


    int8_t value;
    int i;

    if (c->normalized == f->last_value) {
        return;
    }
    f->last_value = c->normalized;

    if (c->normalized > config.light_switch_centers[light_switch_position]) {
        value = c->normalized - config.light_switch_hysteresis;
    }
    else {
        value = c->normalized + config.light_switch_hysteresis;
    }

    for (i = 0; i < config.light_switch_positions; i++) {
        if (value < config.light_switch_centers[i] + config.light_switch_centers[0] + 100) {
            light_switch_position = i;
            return;
        }
    }
}



// ****************************************************************************
static void disable_outputs(CHANNEL_T *c)
{
/*
    Allows turning all LED outputs off. Requested by a customer.

    If the assigned AUX channel has a value of >50 (>1750 us) then the
    all LED outputs get disabled in lights.c at a very low level.
    Note that the light controller operates normally in the background,
    the disable function just turns all light outputs off.
*/

    global_flags.outputs_disabled = false;
    if (c->normalized > 50) {
        global_flags.outputs_disabled = true;
    }
}



// ****************************************************************************
static void handle_aux_channel(CHANNEL_T *c, struct AUX_FLAGS *f, AUX_TYPE_T type, AUX_FUNCTION_T function)
{
    // Map legacy 3-channel settings to new AUX settings
    if (!config.flags2.multi_aux) {
        if (config.flags.ch3_is_momentary) {
            type = MOMENTARY;
        }
        else if (config.flags.ch3_is_two_button) {
            type = TWO_POSITION_UP_DOWN;
        }
        else {
            type = TWO_POSITION;
        }
    }

    switch (function) {
        case MULTI_FUNCTION:
            multi_function(c, f, type);
            break;

        case HAZARD:
            hazard(c, f, type);
            break;

        case SERVO:
            servo(c);
            break;

        case INDICATORS:
            manual_indicators(c, f);
            break;

        case LIGHT_SWITCH:
            light_switch(c, f);
            break;

        case DISABLE_OUTPUTS:
            disable_outputs(c);
            break;

        // case WINCH:
        case GEARBOX:
        case NOT_USED:
        default:
            break;
    }
}


// ****************************************************************************
void process_aux(void)
{
    if (global_flags.initializing) {
        initialized = false;
    }

    if (global_flags.systick) {
        if (click_counter) {
            --click_counter;
        }

        // Support for CH3 being a button or switch directly connected to the
        // light controller. Steering and Throttle are still being read from either
        // the servo reader or the uart reader.
        if (HAL_switch_triggered()) {
            add_click();
        }
    }

    if (global_flags.new_channel_data && !global_flags.shelf_queen_mode) {
        if (!initialized) {
            initialized = true;
            aux_flags[0].last_state = (channel[AUX].normalized > 0) ? true : false;
            aux_flags[1].last_state = (channel[AUX2].normalized > 0) ? true : false;
            aux_flags[2].last_state = (channel[AUX3].normalized > 0) ? true : false;
            return;
        }

        handle_aux_channel(&channel[AUX], &aux_flags[0], config.aux_type, config.aux_function);
        if (config.flags2.multi_aux) {
            handle_aux_channel(&channel[AUX2], &aux_flags[1], config.aux2_type, config.aux2_function);
            handle_aux_channel(&channel[AUX3], &aux_flags[2], config.aux3_type, config.aux3_function);
        }
    }

    process_click_timeout();
}


