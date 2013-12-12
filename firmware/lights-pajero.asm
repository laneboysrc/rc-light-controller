;******************************************************************************
;
;   pajero-lights.asm
;
;   This file contains the business logic to drive the LEDs for a Tamiya
;   CC-01 Pajero shell.
;   The vehicle in question belongs to a user who contacted me on GMail on
;   2013-02-04 (same person as the Subaru).
;
;   The original hardware is used in a single master configuration where
;   the light controller directly reads the servo signals using Y-cables.
;
;   All 8 light outputs are used:
;
;       OUT0    Parking and tail lights
;       OUT1    Main beam (city driving)
;       OUT2    Fog lights
;       OUT3    High beam (cross-country driving)
;       OUT4    Brake lights
;       OUT5    Reversing lights
;       OUT6    Indicators left
;       OUT7    Indicators right
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************
    TITLE       Light tables for the Pajero shell (GMail user request from 2013-02-04)
    RADIX       dec

    #include    hw.tmp
    
    
    GLOBAL Init_lights
    GLOBAL Output_lights

    
    ; Functions and variables imported from utils.asm
    EXTERN TLC5916_send

    EXTERN temp
    EXTERN light_data

    
    ; Functions and variables imported from master.asm
    EXTERN blink_mode
    EXTERN light_mode
    EXTERN drive_mode
    EXTERN setup_mode
    EXTERN startup_mode
    EXTERN servo


; Bitfields in variable blink_mode
#define BLINK_MODE_BLINKFLAG 0          ; Toggles with 1.5 Hz
#define BLINK_MODE_HAZARD 1             ; Hazard lights active
#define BLINK_MODE_INDICATOR_LEFT 2     ; Left indicator active
#define BLINK_MODE_INDICATOR_RIGHT 3    ; Right indicator active

; Bitfields in variable light_mode
#define LIGHT_MODE_PARKING 0        ; Parking lights
#define LIGHT_MODE_MAIN_BEAM 1      ; Main beam (city driving)
#define LIGHT_MODE_FOG 2            ; Fog lights
#define LIGHT_MODE_HIGH_BEAM 3      ; High beam (cross-country driving)

; Bitfields in variable drive_mode
#define DRIVE_MODE_FORWARD 0 
#define DRIVE_MODE_BRAKE 1 
#define DRIVE_MODE_REVERSE 2
#define DRIVE_MODE_BRAKE_ARMED 3
#define DRIVE_MODE_AUTO_BRAKE 4
#define DRIVE_MODE_BRAKE_DISARM 5

; Bitfields in variable setup_mode
#define SETUP_MODE_INIT 0
#define SETUP_MODE_CENTRE 1
#define SETUP_MODE_LEFT 2
#define SETUP_MODE_RIGHT 3
#define SETUP_MODE_STEERING_REVERSE 4
#define SETUP_MODE_NEXT 6
#define SETUP_MODE_CANCEL 7

; Bitfields in variable startup_mode
; Note: the higher 4 bits are used so we can simply "or" it with ch3
; and send it to the slave
#define STARTUP_MODE_NEUTRAL 4      ; Waiting before reading ST/TH neutral

; Bitfields in light_data sent to the TLC5916
#define LED_PARKING 0    
#define LED_MAIN_BEAM 1
#define LED_FOG 2    
#define LED_HIGH_BEAM 3
#define LED_BRAKE 4    
#define LED_REVERSING 5    
#define LED_INDICATOR_LEFT 6    
#define LED_INDICATOR_RIGHT 7    
   
    
;******************************************************************************
; Relocatable variables section
;******************************************************************************
.data_lights UDATA


;============================================================================
;============================================================================
;============================================================================
.lights CODE

;******************************************************************************
; Init_lights
;******************************************************************************
Init_lights
    ; Parking lights until we get the first servo pulse train
    BANKSEL light_data
    clrf    light_data
    bsf     light_data, LED_PARKING
    call    TLC5916_send
    return


;******************************************************************************
; Output_lights
;******************************************************************************
Output_lights
    BANKSEL light_data
    clrf    light_data          ; Clear output data to start cleanly

    BANKSEL startup_mode
    movf    startup_mode, f
    bnz     output_lights_startup

    movf    setup_mode, f
    bnz     output_lights_setup

    BANKSEL light_mode
    movfw   light_mode
    movwf   temp
    BANKSEL light_data
    btfsc   temp, LIGHT_MODE_PARKING
    bsf     light_data, LED_PARKING
    btfsc   temp, LIGHT_MODE_MAIN_BEAM
    bsf     light_data, LED_MAIN_BEAM
    btfsc   temp, LIGHT_MODE_FOG
    bsf     light_data, LED_FOG
    btfsc   temp, LIGHT_MODE_HIGH_BEAM
    bsf     light_data, LED_HIGH_BEAM

    BANKSEL drive_mode
    movfw   drive_mode
    movwf   temp
    BANKSEL light_data
    btfsc   temp, DRIVE_MODE_BRAKE
    bsf     light_data, LED_BRAKE
    btfsc   temp, DRIVE_MODE_REVERSE
    bsf     light_data, LED_REVERSING


    BANKSEL blink_mode
    btfss   blink_mode, BLINK_MODE_BLINKFLAG
    goto    output_lights_execute
    
    movfw   blink_mode
    movwf   temp
    BANKSEL light_data
    btfsc   temp, BLINK_MODE_HAZARD
    bsf     light_data, LED_INDICATOR_LEFT
    btfsc   temp, BLINK_MODE_HAZARD
    bsf     light_data, LED_INDICATOR_RIGHT
    btfsc   temp, BLINK_MODE_INDICATOR_LEFT
    bsf     light_data, LED_INDICATOR_LEFT
    btfsc   temp, BLINK_MODE_INDICATOR_RIGHT
    bsf     light_data, LED_INDICATOR_RIGHT

    goto    output_lights_execute


output_lights_startup
    BANKSEL light_data
    bsf     light_data, LED_MAIN_BEAM
    goto    output_lights_execute


output_lights_setup
    BANKSEL setup_mode
    btfsc   setup_mode, SETUP_MODE_CENTRE
    goto    output_lights_setup_centre
    btfsc   setup_mode, SETUP_MODE_LEFT
    goto    output_lights_setup_right
    btfsc   setup_mode, SETUP_MODE_RIGHT
    goto    output_lights_setup_right
    btfss   setup_mode, SETUP_MODE_STEERING_REVERSE 
    return

    BANKSEL light_data
    bsf     light_data, LED_INDICATOR_LEFT
    goto    output_lights_execute

    
output_lights_setup_centre
    bsf     light_data, LED_INDICATOR_RIGHT
    bsf     light_data, LED_INDICATOR_LEFT
    goto    output_lights_execute


output_lights_setup_left
    BANKSEL light_data
    bsf     light_data, LED_INDICATOR_LEFT
    goto    output_lights_execute

    
output_lights_setup_right
    BANKSEL light_data
    bsf     light_data, LED_INDICATOR_RIGHT
output_lights_execute    
    call    TLC5916_send
    return


    END
