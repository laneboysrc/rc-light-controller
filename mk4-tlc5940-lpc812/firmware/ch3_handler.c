/******************************************************************************

    This function handles CH3 to determine which actions to invoke.

    It is designed for a two-position switch on CH3. The switch can either be
    momentary (e.g Futaba 4PL) or static (HK-310, GT3B ...).
    It also supports direct push-button reading instead of getting the
    CH3 information from a servo or a preprocessor.

;******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include <hal.h>

#include <globals.h>
#include <uart.h>

// This value must be a number higher than any number of clicks we want to
// process.
#define IGNORE_CLICK_COUNT 99


static struct {
    unsigned int last_state : 1;
    unsigned int transitioned : 1;
    unsigned int initialized : 1;
} ch3_flags;

static uint8_t ch3_clicks;
static uint16_t ch3_click_counter;


// ****************************************************************************
static void process_ch3_click_timeout(void)
{
    if (ch3_clicks == 0) {          // Any clicks pending?
        return;                     // No: nothing to do
    }

    if (ch3_click_counter != 0) {   // Double-click timer expired?
        return;                     // No: wait for more buttons
    }

    if (diagnostics_enabled()) {
        uart0_send_cstring("click_timeout\n");
    }

    // ####################################
    // At this point we have detected one of more clicks and need to
    // perform the appropriate action.

    if (global_flags.servo_output_setup != SERVO_OUTPUT_SETUP_OFF) {
        servo_output_setup_action(ch3_clicks);
    }
    else if (global_flags.winch_mode != WINCH_DISABLED) {
        winch_action(ch3_clicks);
    }
    else if (global_flags.reversing_setup != REVERSING_SETUP_OFF) {
        reversing_setup_action(ch3_clicks);
    }
    else {
        // ====================================
        // Normal operation:
        // Neither winch nor setup nor reversing setup is active
        switch (ch3_clicks) {
            case 1:
                // --------------------------
                // Single click
                if (config.flags.gearbox_servo_output) {
                    gearbox_action(ch3_clicks);
                }
                else {
                    light_switch_up();
                }
                break;

            case 2:
                // --------------------------
                // Double click
                if (config.flags.gearbox_servo_output) {
                    gearbox_action(ch3_clicks);
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
                winch_action(ch3_clicks);
                break;

            case 6:
                // --------------------------
                // 6 clicks: Increment sequencer pattern selection
                next_light_sequence();
                break;

            case 7:
                // --------------------------
                // 7 clicks: Enter channel reversing setup mode
                reversing_setup_action(ch3_clicks);
                break;

            case 8:
                // --------------------------
                // 8 clicks: Enter steering wheel servo setup mode
                servo_output_setup_action(ch3_clicks);
                break;

            default:
                break;
        }
    }

    ch3_clicks = 0;
}


// ****************************************************************************
static void add_click(void)
{
    if (diagnostics_enabled()) {
        uart0_send_cstring("add_click\n");
    }

    // If the winch is running any movement of CH3 immediately turns off
    // the winch (without waiting for click timeout!)
    if (abort_winching()) {
        // If winching was aborted disable this series of clicks by setting
        // the click count to an unused high value
        ch3_clicks = IGNORE_CLICK_COUNT;
    }

    // If we have a transmitter with a two position CH3 that uses two buttons
    // to switch between the positions (like the HobbyKing X3S or the Tactic
    // TTC300) then we ignore the first click if the 'down' button is pressed.
    // This way the user can deterministically enter a number of clicks without
    // having to remember if the last command ended with the up or down button.
    //
    // The disadvantage is that always an additional down button press has to
    // be carried out.
    if (config.flags.ch3_is_two_button) {
        if (ch3_click_counter || ch3_flags.last_state) {
            ++ch3_clicks;
        }
    }
    else {
        ++ch3_clicks;
    }
    ch3_click_counter = config.ch3_multi_click_timeout;
}


// ****************************************************************************
void process_ch3_clicks(void)
{
    global_flags.gear_changed = 0;

    if (global_flags.systick) {
        if (ch3_click_counter) {
            --ch3_click_counter;
        }
    }

    // Support for CH3 being a button or switch directly connected to the
    // light controller. Steering and Throttle are still being read from either
    // the servo reader or the uart reader.

    // FIXME: it would be great if this would trigger new_channel_data
    // independently of the other signals, i.e. set it if no other
    // new_channel_data was seen in a certain amount of systicks
    if (config.flags.ch3_is_local_switch) {
        channel[CH3].normalized = hal_gpio_ch3_read() ? -100 : 100;
    }

    if (global_flags.initializing) {
        ch3_flags.initialized = false;
    }

    if (!global_flags.new_channel_data) {
        return;
    }

    if (!ch3_flags.initialized) {
        ch3_flags.initialized = true;
        ch3_flags.last_state = (channel[CH3].normalized > 0) ? true : false;
        return;
    }

    if (config.flags.ch3_is_momentary || config.flags.ch3_is_local_switch) {
        // Code for CH3 having a momentory signal when pressed (Futaba 4PL)

        // We only care about the switch transition from ch3_flags.last_state
        // (set upon initialization) to the opposite position, which is when
        // we add a click.
        if ((channel[CH3].normalized > 0)  !=  (ch3_flags.last_state)) {

            // Did we register this transition already?
            if (!ch3_flags.transitioned) {
                // No: Register transition and add click
                ch3_flags.transitioned = true;
                add_click();
            }
        }
        else {
            ch3_flags.transitioned = false;
        }
    }
    else {
        // Code for CH3 being a two position switch (HK-310, GT3B) or
        // up/down buttons (X3S, Tactic TTC300; config.flags.ch3_is_two_button)

        // Check whether ch3 has changed with respect to LAST_STATE
        if ((channel[CH3].normalized > 0)  !=  (ch3_flags.last_state)) {
            ch3_flags.last_state = (channel[CH3].normalized > 0);
            add_click();
        }
    }

    process_ch3_click_timeout();
}


