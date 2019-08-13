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


static bool initialized;

static struct AUX_FLAGS {
    unsigned int last_state : 4;
    unsigned int transitioned : 1;
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

    fprintf(STDOUT_DEBUG, "click_timeout\n");


    // ####################################
    // At this point we have detected one of more clicks and need to
    // perform the appropriate action.

    if (global_flags.servo_output_setup != SERVO_OUTPUT_SETUP_OFF) {
        servo_output_setup_action(clicks);
    }
    else if (global_flags.winch_mode != WINCH_DISABLED) {
        winch_action(clicks);
    }
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
                if (config.flags.gearbox_servo_output) {
                    gearbox_action(clicks);
                }
                else {
                    light_switch_up();
                }
                break;

            case 2:
                // --------------------------
                // Double click
                if (config.flags.gearbox_servo_output) {
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
                toggle_hazard_lights();
                break;

            case 5:
                // --------------------------
                // 5 clicks: Arm/disarm the winch
                winch_action(clicks);
                break;

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
    fprintf(STDOUT_DEBUG, "add_click\n");

    // If the winch is running any movement of CH3 immediately turns off
    // the winch (without waiting for click timeout!)
    if (abort_winching()) {
        // If winching was aborted disable this series of clicks by setting
        // the click count to an unused high value
        clicks = IGNORE_CLICK_COUNT;
    }

    ++clicks;
    click_counter = config.ch3_multi_click_timeout;
}



// ****************************************************************************
static void multi_function(CHANNEL_T *c, struct AUX_FLAGS *f, AUX_TYPE_T type)
{
    if (type == MOMENTARY) {
        // Code for AUX having a momentory signal when pressed

        // We only care about the switch transition from aux_flags.last_state
        // (set upon initialization) to the opposite position, which is when
        // we add a click.
        if ((c->normalized > 0)  !=  (f->last_state)) {

            // Did we register this transition already?
            if (!f->transitioned) {
                // No: Register transition and add click
                f->transitioned = true;
                add_click();
            }
        }
        else {
            f->transitioned = false;
        }
    }
    else {
        // Code for AUX being a two position switch (HK-310, GT3B) or
        // up/down buttons (X3S, Tactic TTC300; config.flags.ch3_is_two_button)

        // Check whether ch3 has changed with respect to LAST_STATE
        if ((c->normalized > 0)  !=  (f->last_state)) {
            f->last_state = (c->normalized > 0);

            // If we have a transmitter with a two position CH3 that uses two buttons
            // to switch between the positions (like the HobbyKing X3S or the Tactic
            // TTC300) then we ignore the first click if the 'down' button is pressed.
            // This way the user can deterministically enter a number of clicks without
            // having to remember if the last command ended with the up or down button.
            //
            // The disadvantage is that always an additional down button press has to
            // be carried out.
            if (type == TWO_POSITION_UP_DOWN) {
                if (click_counter || f->last_state) {
                    add_click();
                }
                else {
                    // If the winch is running any movement of AUX immediately
                    // turns off the winch (without waiting for click timeout!)
                    if (abort_winching()) {
                        // If winching was aborted disable this series of clicks
                        // by setting the click count to an unused high value
                        clicks = IGNORE_CLICK_COUNT;
                    }
                }
            }
            else {
                add_click();
            }
        }
    }
}


// ****************************************************************************
static void hazard(CHANNEL_T *c, struct AUX_FLAGS *f, AUX_TYPE_T type)
{
    // On/off control for the hazard lights, works with all AUX types but
    // needs special handling for momentary functions

    if (f->last_state) {
        if (c->normalized < -AUX_HYSTERESIS) {
            f->last_state = false;
            if (global_flags.blink_hazard && type != MOMENTARY) {
                toggle_hazard_lights();
            }
        }
    }
    else {
        if (c->normalized > AUX_HYSTERESIS) {
            f->last_state = true;
            if (!global_flags.blink_hazard || type == MOMENTARY) {
                toggle_hazard_lights();
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

    if (f->last_value == -100) {
        if (c->normalized > 33 + AUX_HYSTERESIS) {
            new_value = 100;
        }
        else if (c->normalized > -33 + AUX_HYSTERESIS) {
            new_value = 0;
        }
    }
    else if (f->last_value == 0) {
        if (c->normalized > 33 + AUX_HYSTERESIS) {
            new_value = 100;
        }
        else if (c->normalized < -33 + AUX_HYSTERESIS) {
            new_value = -100;
        }
    }
    else if (f->last_value == 100) {
        if (c->normalized < -33 + AUX_HYSTERESIS) {
            new_value = -100;
        }
        else if (c->normalized < 33 + AUX_HYSTERESIS) {
            new_value = 0;
        }
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

        case WINCH:
        case GEARBOX:
        case LIGHT_SWITCH:
            break;

        case NOT_USED:
        default:
            break;
    }
}


// ****************************************************************************
void process_aux(void)
{
    global_flags.gear_changed = 0;

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

    if (global_flags.new_channel_data) {
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


