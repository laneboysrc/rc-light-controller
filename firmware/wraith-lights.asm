;******************************************************************************
;
;   wraith-lights.asm
;
;   This file contains the business logic to drive the LEDs for Patrick's
;   Wraith.
;
;   The hardware is based on PIC16F1825 and TLC5940. No DC/DC converter is used.
;
;   The TLC5940 IREF is programmed with a 2000 Ohms resistor, which means
;   the maximum LED current is 19.53 mA; each adjustment step is 0.317 mA.
;
;   The following lights are available:
;
;       OUT0    
;       OUT1    
;       OUT2    
;       OUT3    
;       OUT4    
;       OUT5    
;       OUT6    Front left
;       OUT7    Front right
;       OUT8    Indicator left
;       OUT9    Indicator right
;       OUT10   Tail/Brake left
;       OUT11   Tail/Brake right
;       OUT12   Roof 1 (left)
;       OUT13   Roof 2
;       OUT14   Roof 3
;       OUT15   Roof 4 (right)
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************
    TITLE       Light control logic for Patrick's Wraith
    RADIX       dec

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
    EXTERN winch_mode
    EXTERN gear_mode
    EXTERN servo


; Bitfields in variable blink_mode
#define BLINK_MODE_BLINKFLAG 0          ; Toggles with 1.5 Hz
#define BLINK_MODE_HAZARD 1             ; Hazard lights active
#define BLINK_MODE_INDICATOR_LEFT 2     ; Left indicator active
#define BLINK_MODE_INDICATOR_RIGHT 3    ; Right indicator active
#define BLINK_MODE_SOFTTIMER 7          ; Is 1 for one mainloop when the soft 
                                        ; timer triggers (every 65.536ms)

; Bitfields in variable light_mode
#define LIGHT_MODE_MAIN_BEAM 0      ; Main beam
#define LIGHT_MODE_ROOF      1      ; Roof light bar

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

; Values for winch_mode
#define WINCH_MODE_DISABLED 0
#define WINCH_MODE_IDLE 1
#define WINCH_MODE_IN 2 
#define WINCH_MODE_OUT 3

; Bitfields in variable gear_mode
#define GEAR_1 0
#define GEAR_2 1
#define GEAR_CHANGED_FLAG 7

; Bitfields in variable startup_mode
; Note: the higher 4 bits are used so we can simply "or" it with ch3
; and send it to the slave
#define STARTUP_MODE_NEUTRAL 4      ; Waiting before reading ST/TH neutral

#define LED_MAIN_BEAM_L 6
#define LED_MAIN_BEAM_R 7
#define LED_INDICATOR_F_L 8    
#define LED_INDICATOR_F_R 9 
#define LED_TAIL_BRAKE_L 10    
#define LED_TAIL_BRAKE_R 11    
#define LED_ROOF_1 12               ; right-most light (= left-most when viewed from the front!)    
#define LED_ROOF_2 13    
#define LED_ROOF_3 14    
#define LED_ROOF_4 15    

; Since gpasm is not able to use 0.317 we need to calculate with micro-Amps
#define uA_PER_STEP 317

#define VAL_MAIN_BEAM (20 * 1000 / uA_PER_STEP)
#define VAL_INDICATOR_FRONT (20 * 1000 / uA_PER_STEP)
#define VAL_TAIL (7 * 1000 / uA_PER_STEP)
#define VAL_BRAKE (20 * 1000 / uA_PER_STEP)
#define VAL_ROOF (20 * 1000 / uA_PER_STEP)


  
;******************************************************************************
; Relocatable variables section
;******************************************************************************
.data_lights UDATA

sequencer_delay_count res 1

table_l     res 1
table_h     res 1
index_l     res 1
index_h     res 1

seq_leds    res 4


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

    call    Sequencer_start

    ; Light up both front indicators until we receive the first command 
    ; from the UART
    BANKSEL light_data
    movlw   VAL_INDICATOR_FRONT
    movwf   light_data + LED_INDICATOR_F_R
    movwf   light_data + LED_INDICATOR_F_L
    call    TLC5940_send
    return


;******************************************************************************
; Output_lights
;******************************************************************************
Output_lights
    call    Clear_light_data
    call    output_lights_tail

    call    Sequencer
    goto    output_lights_execute    



    BANKSEL startup_mode
    movf    startup_mode, f
    bnz     output_lights_startup

    movf    setup_mode, f
    bnz     output_lights_setup

    ; Normal mode here
    BANKSEL light_mode
    movfw   light_mode
    movwf   temp
    btfsc   temp, LIGHT_MODE_MAIN_BEAM
    call    output_lights_main_beam
    btfsc   temp, LIGHT_MODE_MAIN_BEAM
    call    output_lights_tail


    BANKSEL gear_mode
    btfss   gear_mode, GEAR_CHANGED_FLAG
    goto    output_lights_check_winch    

    BANKSEL gear_mode
    btfsc   gear_mode, GEAR_1
    call    output_lights_gear_1
    BANKSEL gear_mode
    btfsc   gear_mode, GEAR_2
    call    output_lights_gear_1
    goto    output_lights_drive_mode


output_lights_check_winch
    BANKSEL winch_mode
    movfw   winch_mode
    bnz     output_lights_winch
    btfsc   temp, LIGHT_MODE_ROOF
    call    output_lights_roof
    goto    output_lights_drive_mode
    
output_lights_winch  
    BANKSEL winch_mode
    movfw   winch_mode
    sublw   WINCH_MODE_IDLE
    skpnz
    call    output_lights_winch_idle        

    BANKSEL winch_mode
    movfw   winch_mode
    sublw   WINCH_MODE_IN
    skpnz
    call    output_lights_winch_in

    BANKSEL winch_mode
    movfw   winch_mode
    sublw   WINCH_MODE_OUT
    skpnz
    call    output_lights_winch_out

output_lights_drive_mode
    BANKSEL drive_mode
    movfw   drive_mode
    movwf   temp
    btfsc   temp, DRIVE_MODE_BRAKE
    call    output_lights_brake

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
    goto    output_lights_execute    


output_lights_startup
    btfss   startup_mode, STARTUP_MODE_NEUTRAL
    return
    
    movlw   VAL_MAIN_BEAM
    movwf   light_data + LED_ROOF_1
    movwf   light_data + LED_ROOF_4
    goto    output_lights_execute    


output_lights_setup
    btfsc   setup_mode, SETUP_MODE_CENTRE
    goto    output_lights_setup_centre
    btfsc   setup_mode, SETUP_MODE_LEFT
    goto    output_lights_setup_right
    btfsc   setup_mode, SETUP_MODE_RIGHT
    goto    output_lights_setup_right
    btfsc   setup_mode, SETUP_MODE_STEERING_REVERSE 
    call    output_lights_indicator_left
    btfsc   setup_mode, SETUP_MODE_THROTTLE_REVERSE 
    call    output_lights_main_beam

output_lights_execute    
    call    TLC5940_send
    return

output_lights_setup_centre
output_lights_setup_left
output_lights_setup_right
    return


output_lights_main_beam
    BANKSEL light_data
    movlw   VAL_MAIN_BEAM
    movwf   light_data + LED_MAIN_BEAM_L
    movwf   light_data + LED_MAIN_BEAM_R
    return
    
output_lights_roof
    BANKSEL light_data
    movlw   VAL_ROOF
    movwf   light_data + LED_ROOF_1
    movwf   light_data + LED_ROOF_2
    movwf   light_data + LED_ROOF_3
    movwf   light_data + LED_ROOF_4
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
    
output_lights_indicator_left
    BANKSEL light_data
    movlw   VAL_INDICATOR_FRONT
    movwf   light_data + LED_INDICATOR_F_L
    return
    
output_lights_indicator_right
    BANKSEL light_data
    movlw   VAL_INDICATOR_FRONT
    movwf   light_data + LED_INDICATOR_F_R
    return
    
output_lights_winch_idle
    BANKSEL light_data
    movlw   VAL_ROOF
    movwf   light_data + LED_ROOF_4
    return
    
output_lights_winch_in
    BANKSEL light_data
    movlw   VAL_ROOF
    movwf   light_data + LED_ROOF_2
    movwf   light_data + LED_ROOF_3
    return
    
output_lights_winch_out
    BANKSEL light_data
    movlw   VAL_ROOF
    movwf   light_data + LED_ROOF_1
    movwf   light_data + LED_ROOF_4
    return

output_lights_gear_1
    BANKSEL light_data
    movlw   VAL_ROOF
    movwf   light_data + LED_ROOF_4
    return

output_lights_gear_2
    BANKSEL light_data
    movlw   VAL_ROOF
    movwf   light_data + LED_ROOF_4
    movwf   light_data + LED_ROOF_3
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


;******************************************************************************
;******************************************************************************
Sequencer_start
    BANKSEL index_l
    movlw   LOW scan_right_table
    movwf   table_l
    movwf   index_l
    movlw   HIGH scan_right_table
    movwf   table_h
    movwf   index_h
    clrf    sequencer_delay_count
    clrf    seq_leds + 0
    clrf    seq_leds + 1
    clrf    seq_leds + 2
    clrf    seq_leds + 3
    return


;******************************************************************************
;******************************************************************************
Sequencer
    BANKSEL blink_mode
    btfss   blink_mode, BLINK_MODE_SOFTTIMER
    goto    sequencer_softtimer_not_triggered

    BANKSEL sequencer_delay_count
    movfw   sequencer_delay_count
    skpz
    decf    sequencer_delay_count, f
    
sequencer_softtimer_not_triggered
    BANKSEL sequencer_delay_count
    movfw   sequencer_delay_count
    bz      sequencer_loop

sequencer_output    
    ; Push our shadow LED values into the actual LED output
    movfw   seq_leds + 0
    BANKSEL light_data
    movwf   light_data + LED_ROOF_1
    BANKSEL seq_leds
    movfw   seq_leds + 1
    BANKSEL light_data
    movwf   light_data + LED_ROOF_2
    BANKSEL seq_leds
    movfw   seq_leds + 2
    BANKSEL light_data
    movwf   light_data + LED_ROOF_3
    BANKSEL seq_leds
    movfw   seq_leds + 3
    BANKSEL light_data
    movwf   light_data + LED_ROOF_4
    return
    
sequencer_loop
    call    Lookup_table                ; Get a value from the current table
    btfss   WREG, 7                     ; 0 = Delay value, 1 = LED value
    goto    sequencer_got_delay

    ;---------------------------------    
    ; Process LED value
    movwf   temp
    andlw   0x1f                        ; LED value = 2x lower 5 bits
    lslf    WREG, f

    ; Set the LED value to the variable corresponding to the respective LED
    ; number (bits 5 and 6 of the table value we saved in temp)
    BANKSEL seq_leds
    btfsc   temp, 6                     
    goto    sequencer_light_3_and_4    
    btfss   temp, 5
    movwf   seq_leds + 0
    btfsc   temp, 5
    movwf   seq_leds + 1
    goto    sequencer_loop              ; Process the next item in the table...

sequencer_light_3_and_4
    btfss   temp, 5
    movwf   seq_leds + 2
    btfsc   temp, 5
    movwf   seq_leds + 3
    goto    sequencer_loop

    ;---------------------------------    
    ; Process Delay value and END OF TABLE
sequencer_got_delay
    xorlw   0x7f                        ; Check for END OF TABLE (0x7f)
    bnz     sequencer_not_at_end

    BANKSEL table_l
    movfw   table_l
    movwf   index_l
    movfw   table_h
    movwf   index_h
    goto    sequencer_loop              ; Continue processing from the first 
                                        ; item in the table

sequencer_not_at_end
    xorlw   0x7f                        ; Restore the original delay value
    BANKSEL sequencer_delay_count
    movwf   sequencer_delay_count
    goto    sequencer_output    
    
;******************************************************************************
; Equivalent to: W = *index++
;******************************************************************************
Lookup_table
    BANKSEL index_l
    movfw   index_h
    movwf   PCLATH
    movfw   index_l
    incf    index_l, f          ; Post increment
    skpnz
    incf    index_h, f
    movwf   PCL                 ; Get the value and return
    
running_light_table
    retlw   b'10011111'     ; LED 1 (left-most) on full brightness
    retlw   b'00000010'     ; Delay 65ms (1 softtimer)
    retlw   b'10000000'     ; LED 1 off
    retlw   b'10111111'     ; LED 2 on
    retlw   b'00000010'     ; Delay 65ms
    retlw   b'10100000'     ; LED 2 off
    retlw   b'11011111'     ; LED 3 on
    retlw   b'00000010'     ; Delay 65ms
    retlw   b'11000000'     ; LED 3 off
    retlw   b'11111111'     ; LED 4 on
    retlw   b'00000010'     ; Delay 65ms
    retlw   b'11100000'     ; LED 4 off
    retlw   b'11011111'     ; LED 3 on
    retlw   b'00000010'     ; Delay 65ms
    retlw   b'11000000'     ; LED 3 off
    retlw   b'10111111'     ; LED 2 on
    retlw   b'00000010'     ; Delay 65ms
    retlw   b'10100000'     ; LED 2 off
    retlw   b'01111111'     ; END OF TABLE

night_rider_table
    retlw   b'10011111'     ; LED 1 (left-most) on full brightness
    retlw   b'00000001'     ; Delay 65ms (1 softtimer)
    retlw   b'10000011'     ; LED 1 half
    retlw   b'10111111'     ; LED 2 on
    retlw   b'00000001'     ; Delay 65ms
    retlw   b'10000000'     ; LED 1 off
    retlw   b'10100011'     ; LED 2 half
    retlw   b'11011111'     ; LED 3 on
    retlw   b'00000001'     ; Delay 65ms
    retlw   b'10100000'     ; LED 2 off
    retlw   b'11000011'     ; LED 3 half
    retlw   b'11111111'     ; LED 4 on
    retlw   b'00000001'     ; Delay 65ms
    retlw   b'11100011'     ; LED 4 half
    retlw   b'11011111'     ; LED 3 on
    retlw   b'00000001'     ; Delay 65ms
    retlw   b'11100000'     ; LED 4 off
    retlw   b'11000011'     ; LED 3 half
    retlw   b'10111111'     ; LED 2 on
    retlw   b'00000001'     ; Delay 65ms
    retlw   b'11000000'     ; LED 3 off
    retlw   b'10100011'     ; LED 2 half
    retlw   b'01111111'     ; END OF TABLE


scan_right_table
    retlw   b'10011111'     ; LED 1 (left-most) on full brightness
    retlw   b'00000001'     ; Delay 65ms (1 softtimer)
    retlw   b'10001111'     ; LED 1 dim
    retlw   b'10111111'     ; LED 2 on
    retlw   b'00000001'     ; Delay 65ms
    retlw   b'10000111'     ; LED 1 dim
    retlw   b'10101111'     ; LED 2 dim
    retlw   b'11011111'     ; LED 3 on
    retlw   b'00000001'     ; Delay 65ms
    retlw   b'10000011'     ; LED 1 dim
    retlw   b'10100111'     ; LED 2 dim
    retlw   b'11001111'     ; LED 3 dim
    retlw   b'11111111'     ; LED 4 on
    retlw   b'00000001'     ; Delay 65ms
    retlw   b'10000001'     ; LED 1 dim
    retlw   b'10100011'     ; LED 2 dim
    retlw   b'11000111'     ; LED 3 dim
    retlw   b'11101111'     ; LED 4 dim
    retlw   b'00000001'     ; Delay 65ms
    retlw   b'10000000'     ; LED 1 off
    retlw   b'10100001'     ; LED 2 dim
    retlw   b'11000011'     ; LED 3 dim
    retlw   b'11100111'     ; LED 4 dim
    retlw   b'00000001'     ; Delay 65ms
    retlw   b'10100000'     ; LED 2 off
    retlw   b'11000001'     ; LED 3 dim
    retlw   b'11100011'     ; LED 4 dim
    retlw   b'00000001'     ; Delay 65ms
    retlw   b'11000000'     ; LED 3 off
    retlw   b'11100001'     ; LED 4 dim
    retlw   b'00000001'     ; Delay 65ms
    retlw   b'11100000'     ; LED 4 off
    retlw   b'00000111'     ; Delay 65ms
    retlw   b'01111111'     ; END OF TABLE
    
    END
