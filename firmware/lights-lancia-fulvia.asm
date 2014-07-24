;******************************************************************************
;
;   lights-lancia-fulvia.asm
;
;   This file contains the business logic to drive the LEDs for the Italtrading
;   Rally Legends Lancia Fulvia body shell.
;
;   The hardware is based on PIC12F1840 and WS2812. No DC/DC converter is used.
;   All LEDs are PL9823
;
;   The following lights are available:
;
;       LED 1       Left main beam   
;       LED 2       Left high beam
;       LED 3       Right high beam
;       LED 4       Right main beam
;       LED 5       Right indicator / reversing
;       LED 6       Right tail / brake
;       LED 7       Left tail / brake
;       LED 8       Left indicator / reversing
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************
    TITLE       Light control logic for the Rally Legends Lancia Fulvia
    RADIX       dec

    #include    hw.tmp
    
    
    GLOBAL Init_lights
    GLOBAL Output_lights


    ; Functions and variables imported from utils.asm
    EXTERN Clear_light_data
    EXTERN Fill_light_data
    EXTERN xl
    EXTERN temp
    EXTERN light_data

    ; Functions and variables imported from ws2812.asm
    EXTERN Init_WS2812   
    EXTERN WS2812_send
    
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
#define LIGHT_MODE_MAIN_BEAM 0      
#define LIGHT_MODE_HIGH_BEAM 1      

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

#define LED_MAIN_BEAM_L light_data + (3 * 0)
#define LED_MAIN_BEAM_R light_data + (3 * 3)
#define LED_HIGH_BEAM_L light_data + (3 * 1)
#define LED_HIGH_BEAM_R light_data + (3 * 2)
#define LED_TAIL_BRAKE_L light_data + (3 * 6)   
#define LED_TAIL_BRAKE_R light_data + (3 * 5)
#define LED_INDICATOR_REVERSE_L light_data + (3 * 7)   
#define LED_INDICATOR_REVERSE_R light_data + (3 * 4)



#define VAL_MAIN_BEAM 100
#define VAL_HIGH_BEAM 255
#define VAL_TAIL 40
#define VAL_BRAKE 255
#define VAL_REVERSE 40
#define VAL_INDICATOR_R 100     ; Red part of orange
#define VAL_INDICATOR_G 40      ; Green part of orange


  
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
    call    Init_WS2812

    ; Light up main beam, high beam and tail lights until we receive a command
    call    Clear_light_data
    call    output_lights_main_beam
    call    output_lights_high_beam
    call    output_lights_tail
    call    WS2812_send
    
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
    movfw   light_mode
    movwf   temp
    btfsc   temp, LIGHT_MODE_MAIN_BEAM
    call    output_lights_tail
    btfsc   temp, LIGHT_MODE_MAIN_BEAM
    call    output_lights_main_beam
    btfsc   temp, LIGHT_MODE_HIGH_BEAM
    call    output_lights_high_beam

    BANKSEL drive_mode
    btfsc   drive_mode, DRIVE_MODE_BRAKE
    call    output_lights_brake

    BANKSEL drive_mode
    btfsc   drive_mode, DRIVE_MODE_REVERSE
    call    output_lights_reverse

    BANKSEL blink_mode
    btfss   blink_mode, BLINK_MODE_BLINKFLAG
    goto    output_lights_end
    
    movfw   blink_mode
    movwf   temp
    btfsc   temp, BLINK_MODE_HAZARD
    call    output_lights_indicator_left
    btfsc   temp, BLINK_MODE_HAZARD
    call    output_lights_indicator_right
    btfsc   temp, BLINK_MODE_INDICATOR_LEFT
    call    output_lights_indicator_left
    btfsc   temp, BLINK_MODE_INDICATOR_RIGHT
    call    output_lights_indicator_right
    
output_lights_end
    call    WS2812_send
    return


    ;----------
    ; Special handling of initialization phase after power on
output_lights_startup
    btfss   startup_mode, STARTUP_MODE_NEUTRAL
    return
    
    call    output_lights_main_beam
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
    call    output_lights_high_beam
    goto    output_lights_end

output_lights_setup_centre
    goto    output_lights_end

output_lights_setup_left
    goto    output_lights_end

output_lights_setup_right
    goto    output_lights_end


output_lights_main_beam
    movlw   LOW LED_MAIN_BEAM_L
    movwf   FSR0L
    movlw   HIGH LED_MAIN_BEAM_L
    movwf   FSR0H
    movlw   VAL_MAIN_BEAM - 10
    movwi   FSR0++
    movlw   VAL_MAIN_BEAM
    movwi   FSR0++
    movwi   FSR0++
    movlw   LOW LED_MAIN_BEAM_R
    movwf   FSR0L
    movlw   HIGH LED_MAIN_BEAM_R
    movwf   FSR0H
    movlw   VAL_MAIN_BEAM - 10
    movwi   FSR0++
    movlw   VAL_MAIN_BEAM
    movwi   FSR0++
    movwi   FSR0++
    return

output_lights_high_beam
    movlw   LOW LED_HIGH_BEAM_L
    movwf   FSR0L
    movlw   HIGH LED_HIGH_BEAM_L
    movwf   FSR0H
    movlw   VAL_HIGH_BEAM - 10
    movwi   FSR0++
    movlw   VAL_HIGH_BEAM
    movwi   FSR0++
    movwi   FSR0++
    movlw   LOW LED_HIGH_BEAM_R
    movwf   FSR0L
    movlw   HIGH LED_HIGH_BEAM_R
    movwf   FSR0H
    movlw   VAL_HIGH_BEAM - 10
    movwi   FSR0++
    movlw   VAL_HIGH_BEAM
    movwi   FSR0++
    movwi   FSR0++
    return

output_lights_tail
    movlw   VAL_TAIL
    movwf   FSR1L
_output_lights_tail_brake    
    movlw   LOW LED_TAIL_BRAKE_L
    movwf   FSR0L
    movlw   HIGH LED_TAIL_BRAKE_L
    movwf   FSR0H
    movfw   FSR1L
    movwi   FSR0++          ; Red only ...
    movlw   LOW LED_TAIL_BRAKE_R
    movwf   FSR0L
    movlw   HIGH LED_TAIL_BRAKE_R
    movwf   FSR0H
    movfw   FSR1L
    movwi   FSR0++          ; Red only ...
    return
    
output_lights_brake
    movlw   VAL_BRAKE
    movwf   FSR1L
    goto    _output_lights_tail_brake
    
output_lights_reverse
    movlw   LOW LED_INDICATOR_REVERSE_L
    movwf   FSR0L
    movlw   HIGH LED_INDICATOR_REVERSE_L
    movwf   FSR0H
    movlw   VAL_REVERSE - 10        ; Less RED for a cooler color temperature
    movwi   FSR0++
    movlw   VAL_REVERSE
    movwi   FSR0++
    movwi   FSR0++
    movlw   LOW LED_INDICATOR_REVERSE_R
    movwf   FSR0L
    movlw   HIGH LED_INDICATOR_REVERSE_R
    movwf   FSR0H
    movlw   VAL_REVERSE - 10
    movwi   FSR0++
    movlw   VAL_REVERSE
    movwi   FSR0++
    movwi   FSR0++
    return
    
output_lights_indicator_left
    movlw   LOW LED_INDICATOR_REVERSE_L
    movwf   FSR0L
    movlw   HIGH LED_INDICATOR_REVERSE_L
    movwf   FSR0H
    moviw   FSR0
    addlw   VAL_INDICATOR_R     ; Add orange to potential reverse light
    movwi   FSR0++
    moviw   FSR0
    addlw   VAL_INDICATOR_G
    movwi   FSR0++
    return

output_lights_indicator_right
    movlw   LOW LED_INDICATOR_REVERSE_R
    movwf   FSR0L
    movlw   HIGH LED_INDICATOR_REVERSE_R
    movwf   FSR0H
    moviw   FSR0
    addlw   VAL_INDICATOR_R
    movwi   FSR0++
    moviw   FSR0
    addlw   VAL_INDICATOR_G
    movwi   FSR0++
    return

    
    END
