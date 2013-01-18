    TITLE       Light output logic for Phil's Ford
    RADIX       dec

    #include    hw.tmp

    GLOBAL Init_lights
    GLOBAL Output_lights

    
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
#define DRIVE_MODE_REVERSE_BRAKE 4
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
#define STARTUP_MODE_NEUTRAL 4      ; Waiting before reading ST/TH neutral
#define STARTUP_MODE_REVERSING 5    ; Waiting for Forward/Left to obtain direction

    
;******************************************************************************
; Relocatable variables section
;******************************************************************************
.data_lights UDATA
d0 res 1
d1 res 1

;============================================================================
;============================================================================
;============================================================================
.lights CODE

;******************************************************************************
; Init_lights
;******************************************************************************
Init_lights
    call    light_parking_on
    return


;******************************************************************************
; Output_lights
;******************************************************************************
Output_lights
    movf    startup_mode, f
    bnz     _output_lights_startup

    movf    setup_mode, f
    bnz     _output_lights_setup

    btfsc   light_mode, LIGHT_MODE_PARKING
    call    light_parking_on
    btfss   light_mode, LIGHT_MODE_PARKING
    call    light_parking_off

    btfsc   light_mode, LIGHT_MODE_LOW_BEAM
    call    light_main_beam_on
    btfss   light_mode, LIGHT_MODE_LOW_BEAM
    call    light_main_beam_off

    btfsc   light_mode, LIGHT_MODE_ROOF
    call    light_roof_on
    btfss   light_mode, LIGHT_MODE_ROOF
    call    light_roof_off

    ; The brake light and tail light is combined, so turn the low brightness
    ; tail light off when braking is engaged
    btfsc   drive_mode, DRIVE_MODE_BRAKE    
    goto    _output_lights_tail_off    
    btfsc   light_mode, LIGHT_MODE_PARKING
    call    light_tail_on
    btfss   light_mode, LIGHT_MODE_PARKING
_output_lights_tail_off    
    call    light_tail_off

    btfsc   drive_mode, DRIVE_MODE_BRAKE
    call    light_brake_on
    btfss   drive_mode, DRIVE_MODE_BRAKE
    call    light_brake_off

    btfsc   drive_mode, DRIVE_MODE_REVERSE
    call    light_reverse_on
    btfss   drive_mode, DRIVE_MODE_REVERSE
    call    light_reverse_off


    btfss   blink_mode, BLINK_MODE_BLINKFLAG
    goto    _output_lights_indicators_off

    btfsc   blink_mode, BLINK_MODE_HAZARD
    goto    _output_lights_check_indicator
    
    call    light_indicator_left_on
    call    light_indicator_right_on
    goto    _output_lights_blinking_end

_output_lights_check_indicator
    btfsc   blink_mode, BLINK_MODE_INDICATOR_LEFT
    call    light_indicator_left_on
    btfss   blink_mode, BLINK_MODE_INDICATOR_LEFT
    call    light_indicator_left_off
    btfsc   blink_mode, BLINK_MODE_INDICATOR_RIGHT
    call    light_indicator_right_on
    btfss   blink_mode, BLINK_MODE_INDICATOR_RIGHT
    call    light_indicator_right_off
    goto    _output_lights_blinking_end

_output_lights_indicators_off
    call    light_indicator_left_off
    call    light_indicator_right_off

_output_lights_blinking_end
    return

_output_lights_setup
    ; This car does not have a steering wheel servo so we only have to handle
    ; the steering channel reversing setup. 
    ; We light up the left indicator during steering channel reversing.
    btfss   setup_mode, SETUP_MODE_STEERING_REVERSE
    return
    
    call    light_parking_off
    call    light_main_beam_on
    call    light_roof_off
    call    light_tail_off
    call    light_brake_off
    call    light_reverse_off
    call    light_indicator_right_off
    
    call    light_indicator_left_on
    return    

_output_lights_startup
    call    light_parking_off
    call    light_roof_off
    call    light_tail_off
    call    light_brake_off
    call    light_reverse_off
    call    light_indicator_left_off
    call    light_indicator_right_off
    
    call    light_main_beam_on
    return    


;******************************************************************************
light_parking_on
    BANKSEL TRISA
    bcf     PORT_LED_PARKING
    BANKSEL 0
    return

light_parking_off
    BANKSEL TRISA
    bsf     PORT_LED_PARKING
    BANKSEL 0
    return

light_main_beam_on
    BANKSEL TRISA
    bcf     PORT_LED_MAIN_BEAM
    BANKSEL 0
    return

light_main_beam_off
    BANKSEL TRISA
    bsf     PORT_LED_MAIN_BEAM
    BANKSEL 0
    return

light_roof_on
    BANKSEL PORTA
    bsf     PORT_LED_ROOF
    BANKSEL 0
    return

light_roof_off
    BANKSEL PORTA
    bcf     PORT_LED_ROOF
    BANKSEL 0
    return

light_tail_on
    BANKSEL TRISA
    bcf     PORT_LED_TAIL
    BANKSEL 0
    return

light_tail_off
    BANKSEL TRISA
    bsf     PORT_LED_TAIL
    BANKSEL 0
    return

light_brake_on
    BANKSEL TRISA
    bcf     PORT_LED_BRAKE
    bcf     PORT_LED_BRAKE3
    BANKSEL 0
    return

light_brake_off
    BANKSEL TRISA
    bsf     PORT_LED_BRAKE
    bsf     PORT_LED_BRAKE3
    BANKSEL 0
    return

light_indicator_left_on
    BANKSEL TRISA
    bcf     PORT_LED_INDICATOR_LEFT
    BANKSEL 0
    return

light_indicator_left_off
    BANKSEL TRISA
    bsf     PORT_LED_INDICATOR_LEFT
    BANKSEL 0
    return

light_indicator_right_on
    BANKSEL TRISA
    bcf     PORT_LED_INDICATOR_RIGHT
    BANKSEL 0
    return

light_indicator_right_off
    BANKSEL TRISA
    bsf     PORT_LED_INDICATOR_RIGHT
    BANKSEL 0
    return

light_reverse_on
    BANKSEL TRISA
    bcf     PORT_LED_REVERSE
    BANKSEL 0
    return

light_reverse_off
    BANKSEL TRISA
    bsf     PORT_LED_REVERSE
    BANKSEL 0
    return   


    END
