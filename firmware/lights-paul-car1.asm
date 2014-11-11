;******************************************************************************
;
;   lights-paul-car1.asm
;
;
;   This file contains the business logic to drive the LEDs for one of Pauls'
;   cars.
;   Paul contacted us via GMail.
;
;
;
;   The hardware is based on PIC16F1825 and TLC5940. No DC/DC converter is used.
;   It is a master/slave system, similar to our Dingo but based on the
;   PIC16F1825.
;
;   The TLC5940 IREF is programmed with a 2000 Ohms resistor, which means
;   the maximum LED current is 19.53 mA (39.06 / R).
;   Each adjustment step is 0.317 mA.
;
;
;   The following lights are available:
;
;       Master OUT0     Head light front L  (parking = dim!)
;       Master OUT1     Head light front R
;       Master OUT2     High beam front L (mounted in bumper)
;       Master OUT3     High beam front R (mounted in bumper)
;       Master OUT4     Indicator front L
;       Master OUT5     Indicator front R
;       Master OUT6     Tail/Brake/Indicator rear L
;       Master OUT7     Tail/Brake/Indicator rear R
;       Master OUT8     Roof 1
;       Master OUT9     Roof 2
;       Master OUT10    Roof 3
;       Master OUT11    Roof 4
;       Master OUT12    Roof 5
;       Master OUT13    Indicator rear L
;       Master OUT14    Indicator rear R
;       Master OUT15    Tail/Brake L+R
;
;       Slave OUT0      (identical with master!)
;       Slave OUT1
;       Slave OUT2
;       Slave OUT3
;       Slave OUT4
;       Slave OUT5
;       Slave OUT6
;       Slave OUT7
;       Slave OUT8
;       Slave OUT9
;       Slave OUT10
;       Slave OUT11
;       Slave OUT12
;       Slave OUT13
;       Slave OUT14
;       Slave OUT15
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************
    TITLE       Light control logic for Paul's car1
    RADIX       dec

; Resistor determining the maximum LED current on the TLC5940; in Ohms
;###################
#define R_IREF 2000
;###################

    #include    hw.tmp


    GLOBAL Init_lights
    GLOBAL Output_lights


    ; Functions and variables imported from utils.asm
    EXTERN Init_TLC5940
    EXTERN TLC5940_send
    EXTERN UART_send_w

    EXTERN xl
    EXTERN xh
    EXTERN temp
    EXTERN light_data


    ; Functions and variables imported from master.asm
    EXTERN blink_mode
    EXTERN light_mode
    EXTERN drive_mode
    EXTERN setup_mode
    EXTERN startup_mode
    EXTERN servo

#define SLAVE_MAGIC_BYTE 0x87

; Bitfields in variable blink_mode
#define BLINK_MODE_BLINKFLAG 0          ; Toggles with 1.5 Hz
#define BLINK_MODE_HAZARD 1             ; Hazard lights active
#define BLINK_MODE_INDICATOR_LEFT 2     ; Left indicator active
#define BLINK_MODE_INDICATOR_RIGHT 3    ; Right indicator active

; Bitfields in variable light_mode
#define LIGHT_MODE_PARKING 0            ; Head lamps dim beam
#define LIGHT_MODE_MAIN_BEAM 1          ; Head lamps full brightness
#define LIGHT_MODE_HIGH_BEAM 2          ; Additional high beam lights
#define LIGHT_MODE_ROOF 3               ; Additional roof lights

; Bitfields in variable drive_mode
#define DRIVE_MODE_FORWARD 0
#define DRIVE_MODE_BRAKE 1
#define DRIVE_MODE_REVERSE 2

; Bitfields in variable setup_mode
#define SETUP_MODE_INIT 0
#define SETUP_MODE_CENTRE 1
#define SETUP_MODE_LEFT 2
#define SETUP_MODE_RIGHT 3
#define SETUP_MODE_STEERING_REVERSE 4
#define SETUP_MODE_THROTTLE_REVERSE 5
#define SETUP_MODE_NEXT 6
#define SETUP_MODE_CANCEL 7

; Bitfields in variable startup_mode
; Note: the higher 4 bits are used so we can simply "or" it with ch3
; and send it to the slave
#define STARTUP_MODE_NEUTRAL 4      ; Waiting before reading ST/TH neutral

#define LED_M_MAIN_BEAM_L 0
#define LED_M_MAIN_BEAM_R 1
#define LED_M_HIGH_BEAM_L 2
#define LED_M_HIGH_BEAM_R 3
#define LED_M_INDICATOR_F_L 4
#define LED_M_INDICATOR_F_R 5
#define LED_M_TAIL_BRAKE_INDICATOR_R_L 6
#define LED_M_TAIL_BRAKE_INDICATOR_R_R 7
#define LED_M_ROOF_1 8
#define LED_M_ROOF_2 9
#define LED_M_ROOF_3 10
#define LED_M_ROOF_4 11
#define LED_M_ROOF_5 12
#define LED_M_INDICATOR_R_L 13
#define LED_M_INDICATOR_R_R 14
#define LED_M_TAIL_BRAKE_R 15


#define LED_S_MAIN_BEAM_L 0
#define LED_S_MAIN_BEAM_R 1
#define LED_S_HIGH_BEAM_L 2
#define LED_S_HIGH_BEAM_R 3
#define LED_S_INDICATOR_F_L 4
#define LED_S_INDICATOR_F_R 5
#define LED_S_TAIL_BRAKE_INDICATOR_R_L 6
#define LED_S_TAIL_BRAKE_INDICATOR_R_R 7
#define LED_S_ROOF_1 8
#define LED_S_ROOF_2 9
#define LED_S_ROOF_3 10
#define LED_S_ROOF_4 11
#define LED_S_ROOF_5 12
#define LED_S_INDICATOR_R_L 13
#define LED_S_INDICATOR_R_R 14
#define LED_S_TAIL_BRAKE_R 15


; We calculate the LED current per dot-correction step, so that later we can
; use the step size to determine the dot-correction value for a desired
; LED current.
;
; The maximum current is
;       1.24V * 31.05 / R_IREF
; according to the TLC5940 datasheet. 1.24*31.05 is 39.06.
;
; Since gpasm is not able to use fractions we need to work in micro-Amps
#define uA_PER_STEP (39060000 / (R_IREF * 63))

; Convenience value for maximum LED current (6-bit dot-correction value)
#define VAL_FULL 63

#define VAL_PARKING (4 * 1000 / uA_PER_STEP)
#define VAL_MAIN_BEAM VAL_FULL
#define VAL_HIGH_BEAM VAL_FULL
#define VAL_ROOF VAL_FULL
#define VAL_TAIL (4 * 1000 / uA_PER_STEP)
#define VAL_BRAKE VAL_FULL
#define VAL_REVERSING VAL_FULL
#define VAL_INDICATOR_FRONT VAL_FULL
#define VAL_INDICATOR_REAR VAL_FULL



;******************************************************************************
; Relocatable variables section
;******************************************************************************
.data_lights UDATA
light_data_slave    RES     16      ; TLC5940 data we send to the slave


;============================================================================
;============================================================================
;============================================================================
.lights CODE


;******************************************************************************
; Init_lights
;******************************************************************************
Init_lights
    call    Init_TLC5940
    call    Clear_light_data

    ; Light up both front indicators until we receive the first command
    ; from the UART
    movlw   VAL_INDICATOR_FRONT
    BANKSEL light_data_slave
    movwf   light_data_slave + LED_S_INDICATOR_F_R
    movwf   light_data_slave + LED_S_INDICATOR_F_L

    BANKSEL light_data
    movwf   light_data + LED_M_INDICATOR_F_R
    movwf   light_data + LED_M_INDICATOR_F_L

output_lights_end
    call    TLC5940_send
    call    Slave_send
    return


;******************************************************************************
; Output_lights
;******************************************************************************
Output_lights
    call    Clear_light_data

    BANKSEL startup_mode
    movf    startup_mode, f
    bnz     output_lights_startup

    movf    setup_mode, f
    bnz     output_lights_setup

    ;----------
    ; Normal operation of lights goes here
    BANKSEL light_mode
    btfsc   light_mode, LIGHT_MODE_PARKING
    call    output_lights_tail
    BANKSEL light_mode
    btfsc   light_mode, LIGHT_MODE_PARKING
    call    output_lights_parking
    BANKSEL light_mode
    btfsc   light_mode, LIGHT_MODE_MAIN_BEAM
    call    output_lights_main_beam
    BANKSEL light_mode
    btfsc   light_mode, LIGHT_MODE_HIGH_BEAM
    call    output_lights_high_beam
    BANKSEL light_mode
    btfsc   light_mode, LIGHT_MODE_ROOF
    call    output_lights_roof

    BANKSEL drive_mode
    btfsc   drive_mode, DRIVE_MODE_BRAKE
    call    output_lights_brake

;    BANKSEL drive_mode
;    btfsc   drive_mode, DRIVE_MODE_REVERSE
;    call    output_lights_reverse

    BANKSEL blink_mode
    movf    blink_mode, W
    movwf   temp
    btfss   temp, BLINK_MODE_BLINKFLAG
    goto    output_lights_blink_end
    btfsc   temp, BLINK_MODE_HAZARD
    call    output_lights_indicator_left
    btfsc   temp, BLINK_MODE_HAZARD
    call    output_lights_indicator_right
    btfsc   temp, BLINK_MODE_INDICATOR_LEFT
    call    output_lights_indicator_left
    btfsc   temp, BLINK_MODE_INDICATOR_RIGHT
    call    output_lights_indicator_right

output_lights_blink_end



    ; Blinking the combined tail, brake and indicator lamps
    ;
    ;                          BLINKFLAG
    ;                       on          off
    ;  --------------------------------------
    ;  Tail + Brake off     half        off
    ;  Tail                 half        off
    ;  Brake                full        off
    ;  Tail + Brake         full        half
    ;
    ; Note that an added complication is that for indicators we only want to
    ; affect one side!

    BANKSEL blink_mode
    btfsc   blink_mode, BLINK_MODE_HAZARD
    goto    _output_lights_blinking_hazard_is_active
    btfsc   blink_mode, BLINK_MODE_INDICATOR_LEFT
    goto    _output_lights_blinking_left_is_active
    btfsc   blink_mode, BLINK_MODE_INDICATOR_RIGHT
    goto    _output_lights_blinking_right_is_active
    goto    _output_lights_blinking_end

_output_lights_blinking_hazard_is_active
    BANKSEL light_data
    clrf    light_data + LED_M_TAIL_BRAKE_INDICATOR_R_L
    clrf    light_data + LED_M_TAIL_BRAKE_INDICATOR_R_R
    BANKSEL light_data_slave
    clrf    light_data + LED_S_TAIL_BRAKE_INDICATOR_R_L
    clrf    light_data + LED_S_TAIL_BRAKE_INDICATOR_R_R

    BANKSEL blink_mode
    btfss   blink_mode, BLINK_MODE_BLINKFLAG
    goto    _output_lights_indicators_off
    goto    _output_lights_indicators_on

_output_lights_blinking_left_is_active
    BANKSEL light_data
    clrf    light_data + LED_M_TAIL_BRAKE_INDICATOR_R_L
    BANKSEL light_data_slave
    clrf    light_data + LED_S_TAIL_BRAKE_INDICATOR_R_L

    BANKSEL blink_mode
    btfss   blink_mode, BLINK_MODE_BLINKFLAG
    goto    _output_lights_indicators_off
    goto    _output_lights_indicators_on

_output_lights_blinking_right_is_active
    BANKSEL light_data
    clrf    light_data + LED_M_TAIL_BRAKE_INDICATOR_R_R
    BANKSEL light_data_slave
    clrf    light_data + LED_S_TAIL_BRAKE_INDICATOR_R_R

    BANKSEL blink_mode
    btfss   blink_mode, BLINK_MODE_BLINKFLAG
    goto    _output_lights_indicators_off
;   goto    _output_lights_indicators_on

_output_lights_indicators_on
    BANKSEL blink_mode
    movfw   blink_mode
    movwf   temp
    BANKSEL light_data
    btfsc   temp, BLINK_MODE_HAZARD
    call    output_lights_indicator_left
    btfsc   temp, BLINK_MODE_HAZARD
    call    output_lights_indicator_right
    btfsc   temp, BLINK_MODE_INDICATOR_LEFT
    call    output_lights_indicator_left
    btfsc   temp, BLINK_MODE_INDICATOR_RIGHT
    call    output_lights_indicator_right

    BANKSEL drive_mode
    btfss   drive_mode, DRIVE_MODE_BRAKE
    goto    _output_lights_combined_indicators_half

    BANKSEL blink_mode
    movfw   blink_mode
    movwf   temp
    movlw   VAL_INDICATOR_REAR
    BANKSEL light_data
    btfsc   temp, BLINK_MODE_HAZARD
    movwf   light_data + LED_M_TAIL_BRAKE_INDICATOR_R_L
    btfsc   temp, BLINK_MODE_HAZARD
    movwf   light_data + LED_M_TAIL_BRAKE_INDICATOR_R_R
    btfsc   temp, BLINK_MODE_INDICATOR_LEFT
    movwf   light_data + LED_M_TAIL_BRAKE_INDICATOR_R_L
    btfsc   temp, BLINK_MODE_INDICATOR_RIGHT
    movwf   light_data + LED_M_TAIL_BRAKE_INDICATOR_R_R
    BANKSEL light_data_slave
    btfsc   temp, BLINK_MODE_HAZARD
    movwf   light_data + LED_S_TAIL_BRAKE_INDICATOR_R_L
    btfsc   temp, BLINK_MODE_HAZARD
    movwf   light_data + LED_S_TAIL_BRAKE_INDICATOR_R_R
    btfsc   temp, BLINK_MODE_INDICATOR_LEFT
    movwf   light_data + LED_S_TAIL_BRAKE_INDICATOR_R_L
    btfsc   temp, BLINK_MODE_INDICATOR_RIGHT
    movwf   light_data + LED_S_TAIL_BRAKE_INDICATOR_R_R
    goto    _output_lights_blinking_end

_output_lights_indicators_off
    BANKSEL drive_mode
    btfss   drive_mode, DRIVE_MODE_BRAKE
    goto    _output_lights_blinking_end
    btfss   light_mode, LIGHT_MODE_MAIN_BEAM
    goto    _output_lights_blinking_end

_output_lights_combined_indicators_half
    BANKSEL blink_mode
    movfw   blink_mode
    movwf   temp
    movlw   VAL_TAIL
    BANKSEL light_data
    btfsc   temp, BLINK_MODE_HAZARD
    movwf   light_data + LED_M_TAIL_BRAKE_INDICATOR_R_L
    btfsc   temp, BLINK_MODE_HAZARD
    movwf   light_data + LED_M_TAIL_BRAKE_INDICATOR_R_R
    btfsc   temp, BLINK_MODE_INDICATOR_LEFT
    movwf   light_data + LED_M_TAIL_BRAKE_INDICATOR_R_L
    btfsc   temp, BLINK_MODE_INDICATOR_RIGHT
    movwf   light_data + LED_M_TAIL_BRAKE_INDICATOR_R_R
    BANKSEL light_data_slave
    btfsc   temp, BLINK_MODE_HAZARD
    movwf   light_data + LED_S_TAIL_BRAKE_INDICATOR_R_L
    btfsc   temp, BLINK_MODE_HAZARD
    movwf   light_data + LED_S_TAIL_BRAKE_INDICATOR_R_R
    btfsc   temp, BLINK_MODE_INDICATOR_LEFT
    movwf   light_data + LED_S_TAIL_BRAKE_INDICATOR_R_L
    btfsc   temp, BLINK_MODE_INDICATOR_RIGHT
    movwf   light_data + LED_S_TAIL_BRAKE_INDICATOR_R_R
;   goto    _output_lights_blinking_end

_output_lights_blinking_end


    goto    output_lights_end


    ;----------
    ; Special handling of initialization phase after power on
output_lights_startup
    btfss   startup_mode, STARTUP_MODE_NEUTRAL
    return

    movlw   VAL_MAIN_BEAM
    BANKSEL light_data_slave
    movwf   light_data_slave + LED_S_MAIN_BEAM_L
    movwf   light_data_slave + LED_S_MAIN_BEAM_R

    BANKSEL light_data
    movwf   light_data + LED_M_MAIN_BEAM_L
    movwf   light_data + LED_M_MAIN_BEAM_R
    goto    output_lights_end


    ;----------
    ; Special handling of various setup functions
output_lights_setup
    btfsc   setup_mode, SETUP_MODE_CENTRE
    goto    output_lights_setup_centre
    btfsc   setup_mode, SETUP_MODE_LEFT
    goto    output_lights_setup_left
    btfsc   setup_mode, SETUP_MODE_RIGHT
    goto    output_lights_setup_right

    btfsc   setup_mode, SETUP_MODE_STEERING_REVERSE
    call    output_lights_indicator_left
    BANKSEL setup_mode  ; Since we do a "call" before we need to reset the bank!
    btfsc   setup_mode, SETUP_MODE_THROTTLE_REVERSE
    call    output_lights_main_beam
    goto    output_lights_end

output_lights_setup_centre
    call    output_lights_indicator_left
    call    output_lights_indicator_right
    goto    output_lights_end

output_lights_setup_left
    call    output_lights_indicator_left
    goto    output_lights_end

output_lights_setup_right
    call    output_lights_indicator_right
    goto    output_lights_end


output_lights_parking
    movlw   VAL_PARKING
    BANKSEL light_data
    movwf   light_data + LED_M_MAIN_BEAM_L
    movwf   light_data + LED_M_MAIN_BEAM_R
    BANKSEL light_data_slave
    movwf   light_data_slave + LED_S_MAIN_BEAM_L
    movwf   light_data_slave + LED_S_MAIN_BEAM_R
    return

output_lights_main_beam
    movlw   VAL_MAIN_BEAM
    BANKSEL light_data
    movwf   light_data + LED_M_MAIN_BEAM_L
    movwf   light_data + LED_M_MAIN_BEAM_R
    BANKSEL light_data_slave
    movwf   light_data_slave + LED_S_MAIN_BEAM_L
    movwf   light_data_slave + LED_S_MAIN_BEAM_R
    return

output_lights_high_beam
    movlw   VAL_HIGH_BEAM
    BANKSEL light_data
    movwf   light_data + LED_M_HIGH_BEAM_L
    movwf   light_data + LED_M_HIGH_BEAM_R
    BANKSEL light_data_slave
    movwf   light_data_slave + LED_S_HIGH_BEAM_L
    movwf   light_data_slave + LED_S_HIGH_BEAM_R
    return

output_lights_roof
    movlw   VAL_ROOF
    BANKSEL light_data
    movwf   light_data + LED_M_ROOF_1
    movwf   light_data + LED_M_ROOF_2
    movwf   light_data + LED_M_ROOF_3
    movwf   light_data + LED_M_ROOF_4
    movwf   light_data + LED_M_ROOF_5
    BANKSEL light_data_slave
    movwf   light_data_slave + LED_S_ROOF_1
    movwf   light_data_slave + LED_S_ROOF_2
    movwf   light_data_slave + LED_S_ROOF_3
    movwf   light_data_slave + LED_S_ROOF_4
    movwf   light_data_slave + LED_S_ROOF_5
    return

output_lights_tail
    movlw   VAL_TAIL
    BANKSEL light_data
    movwf   light_data + LED_M_TAIL_BRAKE_R
    movwf   light_data + LED_M_TAIL_BRAKE_INDICATOR_R_L
    movwf   light_data + LED_M_TAIL_BRAKE_INDICATOR_R_R
    BANKSEL light_data_slave
    movwf   light_data_slave + LED_S_TAIL_BRAKE_R
    movwf   light_data_slave + LED_S_TAIL_BRAKE_INDICATOR_R_L
    movwf   light_data_slave + LED_S_TAIL_BRAKE_INDICATOR_R_R
    return

output_lights_brake
    movlw   VAL_BRAKE
    BANKSEL light_data
    movwf   light_data + LED_M_TAIL_BRAKE_R
    movwf   light_data + LED_M_TAIL_BRAKE_INDICATOR_R_L
    movwf   light_data + LED_M_TAIL_BRAKE_INDICATOR_R_R
    BANKSEL light_data_slave
    movwf   light_data_slave + LED_S_TAIL_BRAKE_R
    movwf   light_data_slave + LED_S_TAIL_BRAKE_INDICATOR_R_L
    movwf   light_data_slave + LED_S_TAIL_BRAKE_INDICATOR_R_R
    return

;output_lights_reverse
;    movlw   VAL_REVERSING
;    BANKSEL light_data_slave
;    movwf   light_data_slave + LED_S_REVERSING_L
;    movwf   light_data_slave + LED_S_REVERSING_R
;    return

output_lights_indicator_left
    BANKSEL light_data
    movlw   VAL_INDICATOR_FRONT
    movwf   light_data + LED_M_INDICATOR_F_L
    movlw   VAL_INDICATOR_REAR
    movwf   light_data + LED_M_INDICATOR_R_L
    BANKSEL light_data_slave
    movlw   VAL_INDICATOR_FRONT
    movwf   light_data_slave + LED_S_INDICATOR_F_L
    movlw   VAL_INDICATOR_REAR
    movwf   light_data_slave + LED_S_INDICATOR_R_L
    return

output_lights_indicator_right
    BANKSEL light_data
    movlw   VAL_INDICATOR_FRONT
    movwf   light_data + LED_M_INDICATOR_F_R
    movlw   VAL_INDICATOR_REAR
    movwf   light_data + LED_M_INDICATOR_R_R
    BANKSEL light_data_slave
    movlw   VAL_INDICATOR_FRONT
    movwf   light_data_slave + LED_S_INDICATOR_F_R
    movlw   VAL_INDICATOR_REAR
    movwf   light_data_slave + LED_S_INDICATOR_R_R
    return


;******************************************************************************
; Slave_send
;
; Send the magic byte followed by the 16 bytes of light_data_slave[]
;******************************************************************************
Slave_send
    movlw   SLAVE_MAGIC_BYTE
    call    UART_send_w

    movlw   HIGH light_data_slave
    movwf   FSR0H
    movlw   LOW light_data_slave
    movwf   FSR0L

    movlw   16
    movwf   temp
slave_send_loop
    moviw   FSR0++
    call    UART_send_w
    decfsz  temp, f
    goto    slave_send_loop


;******************************************************************************
; Clear_light_data
;
; Clear all light_data variables, i.e. by default all lights are off.
;******************************************************************************
Clear_light_data
    movlw   HIGH light_data_slave
    movwf   FSR0H
    movlw   LOW light_data_slave
    movwf   FSR0L
    call    clear_light_data_start

    movlw   HIGH light_data
    movwf   FSR0H
    movlw   LOW light_data
    movwf   FSR0L
;   goto    clear_light_data_start

clear_light_data_start
    movlw   16          ; There are 16 bytes in light_data / light_data_slave
    movwf   temp
    clrw
clear_light_data_loop
    movwi   FSR0++
    decfsz  temp, f
    goto    clear_light_data_loop

    return

    END
