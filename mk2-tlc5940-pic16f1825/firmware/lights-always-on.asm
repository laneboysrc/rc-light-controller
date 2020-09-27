;******************************************************************************
;
;   generic-always-on.asm
;
;   This file contains the business logic to drive the LEDs so that they are 
;   always on. This may be useful for testing.
;
;   The hardware is based on PIC16F1825 and TLC5940. No DC/DC converter is used.
;
;   The TLC5940 IREF is programmed with a 2000 Ohms resistor, which means
;   the maximum LED current is 19.53 mA (39.06 / R). 
;   Each adjustment step is 0.317 mA.
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************
    TITLE       Generic light control logic
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
#define VAL_HALF 31


  
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

    BANKSEL light_data
    movlw   VAL_HALF
    movwf   light_data
    movwf   light_data + 1
    movwf   light_data + 2
    movwf   light_data + 3
    movwf   light_data + 4
    movwf   light_data + 5
    movwf   light_data + 6
    movwf   light_data + 7
    movwf   light_data + 8
    movwf   light_data + 9
    movwf   light_data + 10
    movwf   light_data + 11
    movwf   light_data + 12
    movwf   light_data + 13
    movwf   light_data + 14
    movwf   light_data + 15
    call    TLC5940_send
    return


;******************************************************************************
; Output_lights
;******************************************************************************
Output_lights
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
