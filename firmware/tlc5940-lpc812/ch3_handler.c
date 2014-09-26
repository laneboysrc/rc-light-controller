/******************************************************************************
; This function handles CH3 to determine which actions to invoke.
; It is designed for a two-position switch on CH3 (HK-310, GT3B ...). The
; switch can either be momentary (e.g Futaba 4PL) or static (HK-310).
;******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include <globals.h>
#include <reader.h>


static struct {
    unsigned int initialized : 1;
    unsigned int last_state : 1;
    unsigned int transitioned : 1;
} ch3_flags;

static uint8_t ch3_clicks;
static uint16_t ch3_click_counter;

static uint16_t winch_command_repeat_counter;


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

    if (setup_mode != 0) {
        // ====================================
        // Steering servo setup in progress:

        if (ch3_clicks == 1) {
            // 1 click: next setup step
            setup_mode = SETUP_MODE_NEXT;
        }
        else {
            // More than 1 click: cancel setup
            setup_mode = SETUP_MODE_CANCEL;
        }
    }
    else if (winch_mode != 0) {
        // ====================================
        // Winch control in progress:

        switch (ch3_clicks) {
            case 1:
                // 1 click: winch in
                winch_mode = WINCH_MODE_IN;
                winch_command_repeat_counter = 0;
                break;

            case 2:
                // 2 click: winch out
                winch_mode = WINCH_MODE_OUT;
                winch_command_repeat_counter = 0;
                break;

            case 5:
                // 5 click: winch disabled
                winch_mode = WINCH_MODE_DISABLED;
                winch_command_repeat_counter = 0;
                break;

            default:
                // Ignore all other clicks
                break;
        }
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

                // FIXME
                // synchronize_blinking();

                global_flags.blink_hazard = ~global_flags.blink_hazard;
                break;

            case 5:
                if (config.flags.winch_enabled) {
                    winch_mode = WINCH_MODE_IDLE;
                }
                break;

            case 6:
                // --------------------------
                // 7 clicks: Increment sequencer pattern selection
                // incf    light_gimmick_mode, f
                break;

            case 7:
                // --------------------------
                // 7 clicks: Enter channel reversing setup mode
                setup_mode =
                    SETUP_MODE_STEERING_REVERSE | SETUP_MODE_THROTTLE_REVERSE;
                break;

            case 8:
                // --------------------------
                // 8 clicks: Enter steering wheel servo setup mode
                if (config.flags.steering_wheel_servo_output_enabled) {
                    setup_mode = SETUP_MODE_INIT;
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
        if (winch_mode == WINCH_MODE_IN  ||  winch_mode == WINCH_MODE_OUT) {
            winch_mode = WINCH_MODE_IDLE;
            winch_command_repeat_counter = 0;

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


#if 0
Process_ch3_double_click
IFDEF ENABLE_GEARBOX
    BANKSEL gear_mode
    bcf     gear_mode, GEAR_CHANGED_FLAG
ENDIF
    BANKSEL startup_mode
    movf    startup_mode, f
    bz      process_ch3_no_startup
    return

process_ch3_no_startup
    btfsc   flags, CH3_FLAG_INITIALIZED
    goto    process_ch3_initialized

    ; Ignore the potential "toggle" after power on
    bsf     flags, CH3_FLAG_INITIALIZED
    bcf     flags, CH3_FLAG_LAST_STATE
    BANKSEL ch3
    btfss   ch3, CH3_FLAG_LAST_STATE
    return
    BANKSEL flags
    bsf     flags, CH3_FLAG_LAST_STATE
    return

process_ch3_initialized
    BANKSEL ch3
    movfw   ch3
    movwf   temp+1

    ; ch3 is only using bit 0, the same bit as CH3_FLAG_LAST_STATE.
    ; We can therefore use XOR to determine whether ch3 has changed.

    BANKSEL flags
    xorwf   flags, w
    movwf   temp

IFDEF CH3_MOMENTARY
    ; -------------------------------------------------------
    ; Code for CH3 having a momentory signal when pressed (Futaba 4PL)

    ; We only care about the switch transition from CH3_FLAG_LAST_STATE
    ; (set upon initialization) to the opposite position, which is when
    ; we add a click.
    btfsc   temp, CH3_FLAG_LAST_STATE
    goto    process_ch3_potential_click

    ; ch3 is the same as CH3_FLAG_LAST_STATE (idle position), therefore reset
    ; our "transitioned" flag to detect the next transition.
    bcf     flags, CH3_FLAG_TANSITIONED
    goto    process_ch3_click_timeout

process_ch3_potential_click
    ; Did we register this transition already?
    ;   Yes: check for click timeout.
    ;   No: Register transition and add click
    btfsc   flags, CH3_FLAG_TANSITIONED
    goto    process_ch3_click_timeout

    bsf     flags, CH3_FLAG_TANSITIONED
    ;goto   process_ch3_add_click

ELSE
    ; -------------------------------------------------------
    ; Code for CH3 being a two position switch (HK-310, GT3B)

    ; Check whether ch3 has changed with respect to LAST_STATE
    btfss   temp, CH3_FLAG_LAST_STATE
    goto    process_ch3_click_timeout

    bcf     flags, CH3_FLAG_LAST_STATE      ; Store the new ch3 state
    btfsc   temp+1, CH3_FLAG_LAST_STATE     ; temp+1 contains ch3
    bsf     flags, CH3_FLAG_LAST_STATE
    ;goto   process_ch3_add_click

    ; -------------------------------------------------------
ENDIF

process_ch3_add_click
IFDEF ENABLE_WINCH
    ; If the winch is running any movement of CH3 immediately turns off
    ; the winch (without waiting for click timeout!)
    BANKSEL winch_mode
    movlw   WINCH_MODE_IN
    subwf   winch_mode, w
    bz      process_ch3_winch_off

    movlw   WINCH_MODE_OUT
    subwf   winch_mode, w
    bnz     process_ch3_no_winch

process_ch3_winch_off
    movlw   WINCH_MODE_IDLE
    movwf   winch_mode
    clrf    winch_command_repeat_counter

    ; Disable this series of clicks by setting the click count to an unused
    ; high value
    BANKSEL ch3_clicks
    movlw   99
    movwf   ch3_clicks
    movlw   CH3_BUTTON_TIMEOUT
    movwf   ch3_click_counter
    return
ENDIF

process_ch3_no_winch
    BANKSEL ch3_clicks
    incf    ch3_clicks, f
    movlw   CH3_BUTTON_TIMEOUT
    movwf   ch3_click_counter
    return


process_ch3_click_timeout
    movf    ch3_clicks, f           ; Any buttons pending?
    skpnz
    return                          ; No: done

    movf    ch3_click_counter, f    ; Double-click timer expired?
    skpz
    return                          ; No: wait for more buttons


    ;####################################
    ; At this point we have detected one of more clicks and need to
    ; perform the appropriate action.
    ;####################################
    movf    setup_mode, f
    bz      process_ch3_click_no_setup

    ;====================================
    ; Steering servo setup in progress:
    ; 1 click: next setup step
    ; more than 1 click: cancel setup
    decfsz  ch3_clicks, f
    goto    process_ch3_setup_cancel
    bsf     setup_mode, SETUP_MODE_NEXT
    return

process_ch3_setup_cancel
    bsf     setup_mode, SETUP_MODE_CANCEL
    clrf    ch3_clicks
    return


process_ch3_click_no_setup
IFDEF ENABLE_WINCH
    movf    winch_mode, f
    bz      process_ch3_click_no_winch

    ;====================================
    ; Winch control in progress:
    ; 1 click: winch in
    ; 2 click: winch out
    ; 5 click: winch disabled
    decfsz  ch3_clicks, f                   ; 1 click: winch in
    goto    process_ch3_no_winch_in

    movlw   WINCH_MODE_IN
process_ch3_winch_execute
    movwf   winch_mode
    clrf    winch_command_repeat_counter
    goto    process_ch3_click_end

process_ch3_no_winch_in
    decfsz  ch3_clicks, f                   ; 2 clicks: winch out
    goto    process_ch3_no_winch_out

    movlw   WINCH_MODE_OUT
    goto    process_ch3_winch_execute

process_ch3_no_winch_out
    decf    ch3_clicks, f                   ; 3 clicks
    decf    ch3_clicks, f                   ; 4 clicks
    decfsz  ch3_clicks, f                   ; 5 clicks: turn off the winch completely
    goto    process_ch3_click_end

    movlw   WINCH_MODE_DISABLED
    goto    process_ch3_winch_execute
ENDIF


    ;====================================
    ; Normal operation; neither winch nor setup is active
process_ch3_click_no_winch
    decfsz  ch3_clicks, f
    goto    process_ch3_double_click

    ; --------------------------
    ; Single click
IFDEF ENABLE_GEARBOX
    movfw   servo_epl
    movwf   servo
    movlw   (1 << GEAR_1) + (1 << GEAR_CHANGED_FLAG)
    movwf   gear_mode
    movlw   GEARBOX_SWITCH_TIME
    movwf   gearbox_servo_active_counter
    clrf    gearbox_servo_idle_counter
ELSE
    ; Switch light mode up (Parking, Low Beam, Fog, High Beam)
    rlf     light_mode, f
    bsf     light_mode, 0
    movlw   LIGHT_MODE_MASK
    andwf   light_mode, f
ENDIF
    return

process_ch3_double_click
    decfsz  ch3_clicks, f
    goto    process_ch3_triple_click

    ; --------------------------
    ; Double click
IFDEF ENABLE_GEARBOX
    movfw   servo_epr
    movwf   servo
    movlw   (1 << GEAR_2) + (1 << GEAR_CHANGED_FLAG)
    movwf   gear_mode
    movlw   GEARBOX_SWITCH_TIME
    movwf   gearbox_servo_active_counter
    clrf    gearbox_servo_idle_counter
ELSE
    ; Switch light mode down (Parking, Low Beam, Fog, High Beam)
    rrf     light_mode, f
    movlw   LIGHT_MODE_MASK
    andwf   light_mode, f
ENDIF
    return

process_ch3_triple_click
    decfsz  ch3_clicks, f
    goto    process_ch3_quad_click

    ; --------------------------
    ; Triple click: all lights on/off
    movlw   LIGHT_MODE_MASK
    andwf   light_mode, w
    sublw   LIGHT_MODE_MASK
    movlw   LIGHT_MODE_MASK
    skpnz
    movlw   0x00
    movwf   light_mode
    skpnz
    return

process_ch3_quad_click
    decfsz  ch3_clicks, f
    goto    process_ch3_5_click

    ; --------------------------
    ; Quad click: Hazard lights on/off
    clrf    ch3_clicks
    call    Synchronize_blinking
    BANKSEL blink_mode
    movlw   1 << BLINK_MODE_HAZARD
    xorwf   blink_mode, f
    return

process_ch3_5_click
    decfsz  ch3_clicks, f
    goto    process_ch3_6_click

IFDEF ENABLE_WINCH
    BANKSEL winch_mode
    movlw   WINCH_MODE_IDLE
    goto    process_ch3_winch_execute
ELSE
    goto    process_ch3_click_end
ENDIF

process_ch3_6_click
    decfsz  ch3_clicks, f
    goto    process_ch3_7_click

    incf    light_gimmick_mode, f
    return

process_ch3_7_click
    decfsz  ch3_clicks, f
    goto    process_ch3_8_click

    ; --------------------------
    ; 7 clicks: Enter steering channel reverse setup mode
    clrf    ch3_clicks
    movlw   (1 << SETUP_MODE_STEERING_REVERSE) + (1 << SETUP_MODE_THROTTLE_REVERSE)
    movwf   setup_mode
    return

process_ch3_8_click
    decfsz  ch3_clicks, f
    goto    process_ch3_click_end

    ; --------------------------
    ; 8 clicks: Enter steering wheel servo setup mode
    IFDEF   ENABLE_SERVO_SETUP
    movlw   1 << SETUP_MODE_INIT
    movwf   setup_mode
    ENDIF
    return

process_ch3_click_end
    clrf    ch3_clicks
    return
#endif