/******************************************************************************
; This function handles CH3 to determine which actions to invoke.
; It is designed for a two-position switch on CH3 (HK-310, GT3B ...). The
; switch can either be momentary (e.g Futaba 4PL) or static (HK-310).
;******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include <globals.h>


static struct {
    unsigned int initialized : 1;
    unsigned int last_state : 1;
    unsigned int transitioned : 1;
} ch3_flags;

static uint8_t ch3_clicks;
static uint16_t ch3_click_counter;



static void process_ch3_click_timeout(void)
{
    if (ch3_clicks == 0) {          // Any clicks pending?
        return;                     // No: nothing to do
    }

    if (ch3_click_counter != 0) {   // Double-click timer expired?
        return;                     // No: wait for more buttons
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
        // Normal operation; neither winch nor setup is active
        switch (ch3_clicks) {
            case 1:
                // --------------------------
                // Single click
                if (config.flags.gearbox_servo_enabled) {
                    // movfw   servo_epl
                    // movwf   servo
                    // movlw   (1 << GEAR_1) + (1 << GEAR_CHANGED_FLAG)
                    // movwf   gear_mode
                    // movlw   GEARBOX_SWITCH_TIME
                    // movwf   gearbox_servo_active_counter
                    // clrf    gearbox_servo_idle_counter
                }
                else {
                    // Switch light mode up (Parking, Low Beam, Fog, High Beam)
                    light_mode <<= 1;
                    light_mode |= 1;
                    light_mode &= config.light_mode_mask;
                }
                break;

            case 2:
                // --------------------------
                // Double click
                if (config.flags.gearbox_servo_enabled) {
                    // movfw   servo_epr
                    // movwf   servo
                    // movlw   (1 << GEAR_2) + (1 << GEAR_CHANGED_FLAG)
                    // movwf   gear_mode
                    // movlw   GEARBOX_SWITCH_TIME
                    // movwf   gearbox_servo_active_counter
                    // clrf    gearbox_servo_idle_counter
                }
                else {
                    // Switch light mode down (Parking, Low Beam, Fog, High Beam)
                    light_mode >>= 1;
                    light_mode &= config.light_mode_mask;
                }
                break;

            case 3:
                // --------------------------
                // Triple click: all lights on/off
                if (light_mode == config.light_mode_mask) {
                    light_mode = 0;
                }
                else {
                    light_mode = config.light_mode_mask;
                }
                break;

            case 4:
                // --------------------------
                // Quad click: Hazard lights on/off
                toggle_hazard_lights();
                break;

            case 5:
                winch_action(ch3_clicks);
                break;

            case 6:
                // --------------------------
                // 7 clicks: Increment sequencer pattern selection
                // incf    light_gimmick_mode, f
                break;

            case 7:
                // --------------------------
                // 7 clicks: Enter channel reversing setup mode
                reversing_setup_action(ch3_clicks);
                break;

            case 8:
                // --------------------------
                // 8 clicks: Enter steering wheel servo setup mode
                if (config.flags.servo_output_enabled) {
                    servo_output_setup_action(ch3_clicks);
                }
                break;

            default:
                break;
        }
    }

    ch3_clicks = 0;
}


static void add_click(void)
{
    if (config.flags.winch_enabled) {
        // If the winch is running any movement of CH3 immediately turns off
        // the winch (without waiting for click timeout!)
        if (abort_winching()) {
            // Disable this series of clicks by setting the click count to an unused
            // high value
            ch3_clicks = 99;
        }
    }

    ++ch3_clicks;
    ch3_click_counter = config.ch3_multi_click_timeout;
}


void process_ch3_clicks(void)
{
    global_flags.gear_changed = 0;

    if (global_flags.systick) {
        if (ch3_click_counter) {
            --ch3_click_counter;
        }
    }

    if (global_flags.startup_mode_neutral) {
        return;
    }

    if (!global_flags.new_channel_data) {
        return;
    }

    if (!ch3_flags.initialized) {
        ch3_flags.initialized = true;
        ch3_flags.last_state = (channel[CH3].normalized > 0) ? true : false;
    }

    if (config.flags.ch3_is_momentary) {
        // Code for CH3 having a momentory signal when pressed (Futaba 4PL)

        // We only care about the switch transition from ch3_flags.last_state
        // (set upon initialization) to the opposite position, which is when
        // we add a click.
        if ((channel[CH3].normalized > 0)  ==  (ch3_flags.last_state)) {
            // CH33 is the same as CH3_FLAG_LAST_STATE (idle position), therefore reset
            // our "transitioned" flag to detect the next transition.
            ch3_flags.transitioned = false;
            process_ch3_click_timeout();
            return;
        }
        else {
            // Did we register this transition already?
            // Yes: check for click timeout.
            // No: Register transition and add click
            if (ch3_flags.transitioned) {
                process_ch3_click_timeout();
                return;
            }
            ch3_flags.transitioned = true;
            add_click();
            return;
        }
    }
    else {
        // Code for CH3 being a two position switch (HK-310, GT3B)

        // Check whether ch3 has changed with respect to LAST_STATE
        if ((channel[CH3].normalized > 0)  ==  (ch3_flags.last_state)) {
            process_ch3_click_timeout();
            return;
        }

        ch3_flags.last_state = (channel[CH3].normalized > 0);
        add_click();
        return;
    }
}


