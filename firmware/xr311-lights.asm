    TITLE       Light tables for the Tamiya XR311
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
#define LIGHT_MODE_LOW_BEAM 1       ; Low beam

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
; Note: the higher 4 bits are used so we can simply "or" it with ch3
; and send it to the slave
#define STARTUP_MODE_NEUTRAL 4      ; Waiting before reading ST/TH neutral

; Bitfields in light_data and light_data+1 sent to the TLC5916
#define LED_LOW_BEAM_RIGHT 7    
#define LED_LOW_BEAM_LEFT 6   
#define LED_REVERSING 5    
#define LED_PARKING 4    
#define LED_TAIL_BRAKE_INDICATOR_LEFT 3    
#define LED_TAIL_BRAKE_INDICATOR_RIGHT 2    
#define LED_INDICATOR_LEFT 1    
#define LED_INDICATOR_RIGHT 0    
   
    
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
    ; Front indicators half brightness until we receive the first command via the UART
    BANKSEL light_data
    clrf    light_data
    clrf    light_data+1
    bsf     light_data, LED_INDICATOR_LEFT    
    bsf     light_data, LED_INDICATOR_RIGHT    
    call    TLC5916_send
    return

;******************************************************************************
; Output_lights
;******************************************************************************
Output_lights
    BANKSEL light_data
    clrf    light_data          ; Clear low brightness data
    clrf    light_data+1        ; Clear full brightness data

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
    btfsc   temp, LIGHT_MODE_PARKING
    bsf     light_data, LED_TAIL_BRAKE_INDICATOR_LEFT
    btfsc   temp, LIGHT_MODE_PARKING
    bsf     light_data, LED_TAIL_BRAKE_INDICATOR_RIGHT

    btfsc   temp, LIGHT_MODE_LOW_BEAM
    bsf     light_data+1, LED_LOW_BEAM_LEFT
    btfsc   temp, LIGHT_MODE_LOW_BEAM
    bsf     light_data+1, LED_LOW_BEAM_RIGHT

    BANKSEL drive_mode
    movfw   drive_mode
    movwf   temp
    BANKSEL light_data
    btfsc   temp, DRIVE_MODE_BRAKE
    bsf     light_data+1, LED_TAIL_BRAKE_INDICATOR_LEFT
    btfsc   temp, DRIVE_MODE_BRAKE
    bsf     light_data+1, LED_TAIL_BRAKE_INDICATOR_RIGHT

    btfsc   temp, DRIVE_MODE_REVERSE
    bsf     light_data+1, LED_REVERSING

    ; Blinking the XR311 is very complicated due to combined tail, brake and
    ; indicator lamp.
    ;
    ; I decided on the following logic:
    ;
    ;                          BLINKFLAG    
    ;                       on          off
    ;  --------------------------------------
    ;  Tail + Brake off     half        off   
    ;  Tail                 half        off
    ;  Brake                full        off
    ;  Tail + Brake         full        half

    BANKSEL blink_mode
    btfsc   blink_mode, BLINK_MODE_HAZARD
    goto    _output_lights_blinking_is_active
    btfsc   blink_mode, BLINK_MODE_INDICATOR_LEFT
    goto    _output_lights_blinking_is_active
    btfss   blink_mode, BLINK_MODE_INDICATOR_RIGHT
    goto    _output_lights_blinking_end

_output_lights_blinking_is_active
    BANKSEL light_data
    bcf     light_data, LED_TAIL_BRAKE_INDICATOR_LEFT
    bcf     light_data, LED_TAIL_BRAKE_INDICATOR_RIGHT
    bcf     light_data+1, LED_TAIL_BRAKE_INDICATOR_LEFT
    bcf     light_data+1, LED_TAIL_BRAKE_INDICATOR_RIGHT

    BANKSEL blink_mode
    btfss   blink_mode, BLINK_MODE_BLINKFLAG
    goto    _output_lights_indicators_off

_output_lights_indicators_on
    BANKSEL blink_mode
    movfw   blink_mode
    movwf   temp
    BANKSEL light_data
    btfsc   temp, BLINK_MODE_HAZARD
    bsf     light_data+1, LED_INDICATOR_LEFT
    btfsc   temp, BLINK_MODE_HAZARD
    bsf     light_data+1, LED_INDICATOR_RIGHT
    btfsc   temp, BLINK_MODE_INDICATOR_LEFT
    bsf     light_data+1, LED_INDICATOR_LEFT
    btfsc   temp, BLINK_MODE_INDICATOR_RIGHT
    bsf     light_data+1, LED_INDICATOR_RIGHT

    BANKSEL drive_mode
    btfss   drive_mode, DRIVE_MODE_BRAKE
    goto    _output_lights_combined_indicators_half

    BANKSEL light_data
    btfsc   temp, BLINK_MODE_HAZARD
    bsf     light_data+1, LED_TAIL_BRAKE_INDICATOR_LEFT
    btfsc   temp, BLINK_MODE_HAZARD
    bsf     light_data+1, LED_TAIL_BRAKE_INDICATOR_RIGHT
    btfsc   temp, BLINK_MODE_INDICATOR_LEFT
    bsf     light_data+1, LED_TAIL_BRAKE_INDICATOR_LEFT
    btfsc   temp, BLINK_MODE_INDICATOR_RIGHT
    bsf     light_data+1, LED_TAIL_BRAKE_INDICATOR_RIGHT
    goto    _output_lights_blinking_end    

_output_lights_indicators_off
    BANKSEL drive_mode
    btfss   drive_mode, DRIVE_MODE_BRAKE
    goto    _output_lights_blinking_end
    btfss   light_mode, LIGHT_MODE_PARKING
    goto    _output_lights_blinking_end

_output_lights_combined_indicators_half
    BANKSEL blink_mode
    movfw   blink_mode
    movwf   temp
    BANKSEL light_data
    btfsc   temp, BLINK_MODE_HAZARD
    bsf     light_data, LED_TAIL_BRAKE_INDICATOR_LEFT
    btfsc   temp, BLINK_MODE_HAZARD
    bsf     light_data, LED_TAIL_BRAKE_INDICATOR_RIGHT
    btfsc   temp, BLINK_MODE_INDICATOR_LEFT
    bsf     light_data, LED_TAIL_BRAKE_INDICATOR_LEFT
    btfsc   temp, BLINK_MODE_INDICATOR_RIGHT
    bsf     light_data, LED_TAIL_BRAKE_INDICATOR_RIGHT
;   goto    _output_lights_blinking_end

_output_lights_blinking_end
    ; Turn off half brightness if full brightness is requested
    BANKSEL light_data
    btfsc   light_data+1, LED_TAIL_BRAKE_INDICATOR_LEFT
    bcf     light_data, LED_TAIL_BRAKE_INDICATOR_LEFT
    btfsc   light_data+1, LED_TAIL_BRAKE_INDICATOR_RIGHT
    bcf     light_data, LED_TAIL_BRAKE_INDICATOR_RIGHT

    goto    output_lights_execute


output_lights_startup
    BANKSEL startup_mode
    btfss   startup_mode, STARTUP_MODE_NEUTRAL
    return

    BANKSEL light_data
    bsf     light_data, LED_LOW_BEAM_LEFT
    bsf     light_data, LED_LOW_BEAM_RIGHT
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
    bsf     light_data, LED_LOW_BEAM_LEFT
    goto    output_lights_execute
    
output_lights_setup_centre
    bsf     light_data+1, LED_INDICATOR_RIGHT
;   bsf     light_data+1, LED_INDICATOR_LEFT
;   goto    output_lights_setup_execute

output_lights_setup_left
    BANKSEL light_data
    bsf     light_data+1, LED_INDICATOR_LEFT
    goto    output_lights_execute
    
output_lights_setup_right
    BANKSEL light_data
    bsf     light_data+1, LED_INDICATOR_RIGHT

output_lights_execute    
    call    TLC5916_send
    return


    END
