;******************************************************************************
;
;   phil-ford-lights.asm
;
;   This file contains the business logic to drive the LEDs for Phil's
;   Ford F150 body.
;
;   The hardware is based on PIC16F1825 and TLC5940. No DC/DC converter is used.
;
;   The TLC5940 IREF is programmed with a 2000 Ohms resistor, which means
;   the maximum LED current is 19.53 mA; each adjustment step is 0.317 mA.
;
;   The following lights are available:
;
;       OUT0    3rd brake light
;       OUT1    Main beam left 
;       OUT2    Main beam right
;       OUT3    Indicators front left
;       OUT4    Indicators rear left
;       OUT5    Parking lights right
;       OUT6    Indicators front right   
;       OUT7    Indicators rear right   
;       OUT8    Parking lights left
;       OUT9    Tail/Brake left
;       OUT10   Tail/Brake right
;       OUT11   (not used)
;       OUT12   Reversing light left
;       OUT13   Reversing light right
;       OUT14   Indicators mirror right
;       OUT15   Indicators mirror left
;
;       PORTA.2 Roof light bar (switched through a transitor; H = on)
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************
    TITLE       Light output logic for Phil's Ford
    RADIX       dec

    #include    hw.tmp

    GLOBAL Init_lights
    GLOBAL Output_lights
    
    
    ; Functions and variables imported from utils.asm
    EXTERN Init_TLC5940    
    EXTERN TLC5940_send
    
    EXTERN temp
    EXTERN light_data
    
    
    ; Functions and variables imported from master.asm
    EXTERN blink_mode
    EXTERN light_mode
    EXTERN drive_mode
    EXTERN setup_mode
    EXTERN startup_mode

    
; Bitfields in variable blink_mode
#define BLINK_MODE_BLINKFLAG 0          ; Toggles with 1.5 Hz
#define BLINK_MODE_HAZARD 1             ; Hazard lights active
#define BLINK_MODE_INDICATOR_LEFT 2     ; Left indicator active
#define BLINK_MODE_INDICATOR_RIGHT 3    ; Right indicator active

; Bitfields in variable light_mode
#define LIGHT_MODE_PARKING 0        ; Parking lights
#define LIGHT_MODE_LOW_BEAM 1       ; Low beam
#define LIGHT_MODE_ROOF 2           ; Roof lamps

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
#define SETUP_MODE_THROTTLE_REVERSE 5
#define SETUP_MODE_NEXT 6
#define SETUP_MODE_CANCEL 7

; Bitfields in variable startup_mode
#define STARTUP_MODE_NEUTRAL 4      ; Waiting before reading ST/TH neutral


#define LED_PARKING_L 8    
#define LED_PARKING_R 5    
#define LED_MAIN_BEAM_L 1
#define LED_MAIN_BEAM_R 2
#define LED_INDICATOR_F_L 3    
#define LED_INDICATOR_F_R 6 
#define LED_INDICATOR_R_L 4    
#define LED_INDICATOR_R_R 7
#define LED_INDICATOR_M_L 15    
#define LED_INDICATOR_M_R 14
#define LED_TAIL_BRAKE_L 9    
#define LED_TAIL_BRAKE_R 10    
#define LED_REVERSE_L 12    
#define LED_REVERSE_R 13    
#define LED_3RD_BRAKE 0    

#define PORT_LED_ROOF_BIT 2

; Since gpasm is not able to use 0.317 we need to calculate with micro-Amps
#define uA_PER_STEP 317

#define VAL_PARKING (20 * 1000 / uA_PER_STEP)
#define VAL_MAIN_BEAM (20 * 1000 / uA_PER_STEP)
#define VAL_TAIL (4 * 1000 / uA_PER_STEP)
#define VAL_BRAKE (20 * 1000 / uA_PER_STEP)
#define VAL_INDICATOR_FRONT (20 * 1000 / uA_PER_STEP)
#define VAL_INDICATOR_REAR (20 * 1000 / uA_PER_STEP)
#define VAL_INDICATOR_MIRROR (20 * 1000 / uA_PER_STEP)
#define VAL_REVERSE (20 * 1000 / uA_PER_STEP)


    
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
    call    Init_TLC5940

    BANKSEL TRISA
    bcf     TRISA, PORT_LED_ROOF_BIT

    call    Clear_light_data
    call    light_parking_on
    call    TLC5940_send
    return


;******************************************************************************
; Output_lights
;******************************************************************************
Output_lights
    call    Clear_light_data

    BANKSEL startup_mode
    movf    startup_mode, f
    bnz     _output_lights_startup

    movf    setup_mode, f
    bnz     _output_lights_setup

    BANKSEL light_mode
    btfsc   light_mode, LIGHT_MODE_PARKING
    call    light_parking_on

    BANKSEL light_mode
    btfsc   light_mode, LIGHT_MODE_PARKING
    call    light_tail_on

    BANKSEL light_mode
    btfsc   light_mode, LIGHT_MODE_LOW_BEAM
    call    light_main_beam_on

    BANKSEL light_mode
    btfsc   light_mode, LIGHT_MODE_ROOF
    call    light_roof_on
    BANKSEL light_mode
    btfss   light_mode, LIGHT_MODE_ROOF
    call    light_roof_off

    BANKSEL drive_mode
    btfsc   drive_mode, DRIVE_MODE_BRAKE
    call    light_brake_on

    BANKSEL drive_mode
    btfsc   drive_mode, DRIVE_MODE_REVERSE
    call    light_reverse_on

    BANKSEL blink_mode
    btfss   blink_mode, BLINK_MODE_BLINKFLAG
    goto    _output_lights_indicators_off

    btfss   blink_mode, BLINK_MODE_HAZARD
    goto    _output_lights_check_indicator
    
    call    light_indicator_left_on
    call    light_indicator_right_on
    goto    _output_lights_blinking_end

_output_lights_check_indicator
    BANKSEL blink_mode
    btfsc   blink_mode, BLINK_MODE_INDICATOR_LEFT
    call    light_indicator_left_on
    BANKSEL blink_mode
    btfsc   blink_mode, BLINK_MODE_INDICATOR_RIGHT
    call    light_indicator_right_on

_output_lights_indicators_off
_output_lights_blinking_end
_output_lights_execute    
    call    TLC5940_send
    return


_output_lights_setup
    ; This car does not have a steering wheel servo so we only have to handle
    ; the steering and throttle channel reversing setup. 
    ; We light up the left indicator during steering channel reversing and the 
    ; main beam during throttle reversing.
    BANKSEL setup_mode
    btfsc   setup_mode, SETUP_MODE_STEERING_REVERSE
    call    light_indicator_left_on
    BANKSEL setup_mode
    btfsc   setup_mode, SETUP_MODE_THROTTLE_REVERSE
    call    light_main_beam_on
    goto    _output_lights_execute


_output_lights_startup
    call    light_main_beam_on
    goto    _output_lights_execute


;******************************************************************************
light_parking_on
    BANKSEL light_data
    movlw   VAL_PARKING
    movwf   light_data + LED_PARKING_L
    movwf   light_data + LED_PARKING_R
    return

light_main_beam_on
    BANKSEL light_data
    movlw   VAL_MAIN_BEAM
    movwf   light_data + LED_MAIN_BEAM_L
    movwf   light_data + LED_MAIN_BEAM_R
    return

light_roof_on
    BANKSEL LATA
    bsf     LATA, PORT_LED_ROOF_BIT
    return

light_roof_off
    BANKSEL LATA
    bcf     LATA, PORT_LED_ROOF_BIT
    return

light_tail_on
    BANKSEL light_data
    movlw   VAL_TAIL
    movwf   light_data + LED_TAIL_BRAKE_L
    movwf   light_data + LED_TAIL_BRAKE_R
    return

light_brake_on
    BANKSEL light_data
    movlw   VAL_BRAKE
    movwf   light_data + LED_TAIL_BRAKE_L
    movwf   light_data + LED_TAIL_BRAKE_R
    movwf   light_data + LED_3RD_BRAKE
    return

light_indicator_left_on
    BANKSEL light_data
    movlw   VAL_INDICATOR_FRONT
    movwf   light_data + LED_INDICATOR_F_L
    movlw   VAL_INDICATOR_REAR
    movwf   light_data + LED_INDICATOR_R_L
    movlw   VAL_INDICATOR_MIRROR
    movwf   light_data + LED_INDICATOR_M_L
    return

light_indicator_right_on
    BANKSEL light_data
    movlw   VAL_INDICATOR_FRONT
    movwf   light_data + LED_INDICATOR_F_R
    movlw   VAL_INDICATOR_REAR
    movwf   light_data + LED_INDICATOR_R_R
    movlw   VAL_INDICATOR_MIRROR
    movwf   light_data + LED_INDICATOR_M_R
    return

light_reverse_on
    BANKSEL light_data
    movlw   VAL_REVERSE
    movwf   light_data + LED_REVERSE_L
    movwf   light_data + LED_REVERSE_R
    return


;******************************************************************************
; Clear_light_data
;
; Clear all light_data variables, i.e. by default all lights are off.
;******************************************************************************
Clear_light_data
    movlw   HIGH light_data
    movwf   FSR0H
    movlw   LOW light_data
    movwf   FSR0L
    movlw   16          ; There are 16 bytes in light_data
    movwf   temp
    clrw   
clear_light_data_loop
    movwi   FSR0++    
    decfsz  temp, f
    goto    clear_light_data_loop
    return

    END
