;******************************************************************************
;
;   lights-chai-jeep.asm
;
;
;   This file contains the business logic to drive the LEDs for Chai's
;   Axial SCX10 Jeep Wrangler.
;   Chai contacted us via SGCrawlers.
;
;   The brief was to do the same setup of lights as on our Dingo, but
;   with the TLC5940/PIC16F1825 hardware.
;
;   The hardware is based on PIC16F1825 and TLC5940. No DC/DC converter is used.
;   It is a master/slave system.
;
;   The TLC5940 IREF is programmed with a 2000 Ohms resistor, which means
;   the maximum LED current is 19.53 mA (39.06 / R).
;   Each adjustment step is 0.317 mA.
;
;
;   The following lights are available:
;
;       Master OUT0     Fog front L
;       Master OUT1     Fog front R
;       Master OUT2     Fog rear L
;       Master OUT3     Fog rear L
;       Master OUT4     High beam front L
;       Master OUT5     High beam front R
;       Master OUT6
;       Master OUT7
;       Master OUT8
;       Master OUT9
;       Master OUT10
;       Master OUT11
;       Master OUT12
;       Master OUT13
;       Master OUT14
;       Master OUT15
;
;       Slave OUT0      Parking lights front L
;       Slave OUT1      Parking lights front R
;       Slave OUT2      Main beam front L
;       Slave OUT3      Main beam front R
;       Slave OUT4      Indicator front L
;       Slave OUT5      Indicator front R
;       Slave OUT6      Tail / Brake rear L
;       Slave OUT7      Tail / Brake rear R
;       Slave OUT8      Reversing light rear L
;       Slave OUT9      Reversing light rear R
;       Slave OUT10     Indicator rear L
;       Slave OUT11     Indicator rear R
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
    TITLE       Light control logic for the GMade Sawback
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
#define LIGHT_MODE_PARKING 0            ; Parking lights
#define LIGHT_MODE_MAIN_BEAM 1          ; Main beam
#define LIGHT_MODE_FOG 2                ; Fog lights
#define LIGHT_MODE_HIGH_BEAM 3          ; High beam

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


#define LED_M_FOG_F_L 0
#define LED_M_FOG_F_R 1
#define LED_M_FOG_R_L 2
#define LED_M_FOG_R_R 3
#define LED_M_HIGH_BEAM_F_L 4
#define LED_M_HIGH_BEAM_F_R 5

#define LED_S_PARKING_F_L 0
#define LED_S_PARKING_F_R 1
#define LED_S_MAIN_BEAM_F_L 2
#define LED_S_MAIN_BEAM_F_R 3
#define LED_S_INDICATOR_F_L 4
#define LED_S_INDICATOR_F_R 5
#define LED_S_TAIL_BRAKE_R_L 6
#define LED_S_TAIL_BRAKE_R_R 7
#define LED_S_REVERSING_R_L 8
#define LED_S_REVERSING_R_R 9
#define LED_S_INDICATOR_R_L 10
#define LED_S_INDICATOR_R_R 11


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

#define VAL_PARKING VAL_FULL
#define VAL_MAIN_BEAM VAL_FULL
#define VAL_HIGH_BEAM VAL_FULL
#define VAL_FOG_FRONT VAL_FULL
#define VAL_FOG_REAR VAL_FULL
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

    movlw   VAL_FOG_FRONT
    BANKSEL light_data
    movwf   light_data + LED_M_FOG_F_L
    movwf   light_data + LED_M_FOG_F_R

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
    btfsc   light_mode, LIGHT_MODE_FOG
    call    output_lights_fog
    BANKSEL light_mode
    btfsc   light_mode, LIGHT_MODE_HIGH_BEAM
    call    output_lights_high_beam

    BANKSEL drive_mode
    btfsc   drive_mode, DRIVE_MODE_BRAKE
    call    output_lights_brake

    BANKSEL drive_mode
    btfsc   drive_mode, DRIVE_MODE_REVERSE
    call    output_lights_reverse

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
    goto    output_lights_end


    ;----------
    ; Special handling of initialization phase after power on
output_lights_startup
    btfss   startup_mode, STARTUP_MODE_NEUTRAL
    return

    movlw   VAL_MAIN_BEAM
    BANKSEL light_data_slave
    movwf   light_data_slave + LED_S_MAIN_BEAM_F_L
    movwf   light_data_slave + LED_S_MAIN_BEAM_F_R

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
    BANKSEL light_data_slave
    movwf   light_data_slave + LED_S_PARKING_F_L
    movwf   light_data_slave + LED_S_PARKING_F_R
    return

output_lights_main_beam
    movlw   VAL_MAIN_BEAM
    BANKSEL light_data_slave
    movwf   light_data_slave + LED_S_MAIN_BEAM_F_L
    movwf   light_data_slave + LED_S_MAIN_BEAM_F_R
    return

output_lights_fog
    movlw   VAL_FOG_FRONT
    BANKSEL light_data
    movwf   light_data + LED_M_FOG_F_L
    movwf   light_data + LED_M_FOG_F_R

    movlw   VAL_FOG_REAR
    movwf   light_data + LED_M_FOG_R_L
    movwf   light_data + LED_M_FOG_R_R
    return

output_lights_high_beam
    movlw   VAL_HIGH_BEAM
    BANKSEL light_data
    movwf   light_data + LED_M_HIGH_BEAM_F_L
    movwf   light_data + LED_M_HIGH_BEAM_F_R
    return

output_lights_tail
    movlw   VAL_TAIL
    BANKSEL light_data_slave
    movwf   light_data_slave + LED_S_TAIL_BRAKE_R_L
    movwf   light_data_slave + LED_S_TAIL_BRAKE_R_R
    return

output_lights_brake
    movlw   VAL_BRAKE
    BANKSEL light_data_slave
    movwf   light_data_slave + LED_S_TAIL_BRAKE_R_L
    movwf   light_data_slave + LED_S_TAIL_BRAKE_R_R
    return

output_lights_reverse
    movlw   VAL_REVERSING
    BANKSEL light_data_slave
    movwf   light_data_slave + LED_S_REVERSING_R_L
    movwf   light_data_slave + LED_S_REVERSING_R_R
    return

output_lights_indicator_left
    BANKSEL light_data_slave
    movlw   VAL_INDICATOR_FRONT
    movwf   light_data_slave + LED_S_INDICATOR_F_L
    movlw   VAL_INDICATOR_REAR
    movwf   light_data_slave + LED_S_INDICATOR_R_L
    return

output_lights_indicator_right
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
