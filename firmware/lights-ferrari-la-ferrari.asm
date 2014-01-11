;******************************************************************************
;
;   lights-ferrari-la-ferrari.asm
;
;   This file contains the business logic to drive the LEDs for the Tamiya
;   Ferrari La Ferrari.
;
;   The hardware is based on PIC16F1825 and TLC5940. No DC/DC converter is used.
;
;   The TLC5940 IREF is programmed with a 2000 Ohms resistor, which means
;   the maximum LED current is 19.53 mA (39.06 / R). 
;   Each adjustment step is 0.317 mA.
;
;
;   The following lights are available:
;
;       OUT0    Brake/Tail left
;       OUT1    Brake/Tail right
;       OUT2    Main beam left
;       OUT3    Main beam right
;       OUT4    
;       OUT5    
;       OUT6    
;       OUT7    
;       OUT8    
;       OUT9    
;       OUT10   
;       OUT11   
;       OUT12   
;       OUT13   
;       OUT14   
;       OUT15   
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************
    TITLE       Light control logic for the Tamiya Ferrari La Ferrari
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


; Bitfields in variable blink_mode
#define BLINK_MODE_BLINKFLAG 0          ; Toggles with 1.5 Hz
#define BLINK_MODE_HAZARD 1             ; Hazard lights active
#define BLINK_MODE_INDICATOR_LEFT 2     ; Left indicator active
#define BLINK_MODE_INDICATOR_RIGHT 3    ; Right indicator active

; Bitfields in variable light_mode
#define LIGHT_MODE_MAIN_BEAM 0      ; Main beam

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

#define LED_TAIL_BRAKE_L 0    
#define LED_TAIL_BRAKE_R 1    
#define LED_MAIN_BEAM_L 2
#define LED_MAIN_BEAM_R 3


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

#define VAL_MAIN_BEAM VAL_FULL
#define VAL_TAIL (4 * 1000 / uA_PER_STEP)
#define VAL_BRAKE VAL_FULL


  
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
    call    Clear_light_data

    ; Light up both front indicators until we receive the first command 
    ; from the UART
    BANKSEL light_data
    movlw   VAL_TAIL
    movwf   light_data + LED_TAIL_BRAKE_L
    movwf   light_data + LED_TAIL_BRAKE_R
    call    TLC5940_send
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

    BANKSEL drive_mode
    movfw   drive_mode
    movwf   temp
    btfsc   temp, DRIVE_MODE_BRAKE
    call    output_lights_brake
    
output_lights_end
    call    TLC5940_send
    return


    ;----------
    ; Special handling of initialization phase after power on
output_lights_startup
    btfss   startup_mode, STARTUP_MODE_NEUTRAL
    return
    
    BANKSEL light_data
    movlw   VAL_MAIN_BEAM
    movwf   light_data + LED_MAIN_BEAM_L
    movwf   light_data + LED_MAIN_BEAM_R
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
    nop
    BANKSEL setup_mode  ; Since we do a "call" before we need to reset the bank!
    btfsc   setup_mode, SETUP_MODE_THROTTLE_REVERSE 
    call    output_lights_main_beam
    goto    output_lights_end

output_lights_setup_centre
    goto    output_lights_end

output_lights_setup_left
    goto    output_lights_end

output_lights_setup_right
    goto    output_lights_end


output_lights_main_beam
    BANKSEL light_data
    movlw   VAL_MAIN_BEAM
    movwf   light_data + LED_MAIN_BEAM_L
    movwf   light_data + LED_MAIN_BEAM_R
    return

output_lights_tail
    BANKSEL light_data
    movlw   VAL_TAIL
    movwf   light_data + LED_TAIL_BRAKE_L
    movwf   light_data + LED_TAIL_BRAKE_R
    return
    
output_lights_brake
    BANKSEL light_data
    movlw   VAL_BRAKE
    movwf   light_data + LED_TAIL_BRAKE_L
    movwf   light_data + LED_TAIL_BRAKE_R
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
