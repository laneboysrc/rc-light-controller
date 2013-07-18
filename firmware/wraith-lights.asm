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
    EXTERN light_gimmick_mode
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


#define SEQUENCER_MODE_IDLE 0
#define SEQUENCER_MODE_GEAR1 1 + 0x80
#define SEQUENCER_MODE_GEAR2 2 + 0x80
#define SEQUENCER_MODE_WINCH_IDLE 3
#define SEQUENCER_MODE_WINCH_IN 4
#define SEQUENCER_MODE_WINCH_OUT 5
#define SEQUENCER_MODE_ROOF_OFF 6
#define SEQUENCER_MODE_ROOF_ON 7



  
;******************************************************************************
; Relocatable variables section
;******************************************************************************
.data_lights UDATA

sequencer_delay_count   res 1
sequencer_mode          res 1
sequencer_count         res 1
seq_leds                res 4

table_l     res 1
table_h     res 1
index_l     res 1
index_h     res 1


;============================================================================
;============================================================================
;============================================================================
.lights CODE


;******************************************************************************
; Init_lights
;******************************************************************************
Init_lights
    BANKSEL sequencer_mode
    clrf    sequencer_mode
    clrf    sequencer_count
    clrf    sequencer_delay_count

    call    Init_TLC5940
    call    Clear_light_data

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

    ; Reset our light_gimmick_mode whenever light mode is not "all lights on"
    BANKSEL light_mode
    movf    light_mode, w
    sublw   LIGHT_MODE_MASK
    skpz
    clrf    light_gimmick_mode


    BANKSEL startup_mode
    movf    startup_mode, f
    bnz     output_lights_startup

    movf    setup_mode, f
    bnz     output_lights_setup

    ; Normal mode here
output_lights_light_mode
    BANKSEL light_mode
    movfw   light_mode
    movwf   temp
    btfsc   temp, LIGHT_MODE_MAIN_BEAM
    call    output_lights_main_beam
    btfsc   temp, LIGHT_MODE_MAIN_BEAM
    call    output_lights_tail

output_lights_drive_mode
    BANKSEL drive_mode
    movfw   drive_mode
    movwf   temp
    btfsc   temp, DRIVE_MODE_BRAKE
    call    output_lights_brake

output_lights_blink_mode
    BANKSEL blink_mode
    btfss   blink_mode, BLINK_MODE_BLINKFLAG
    goto    output_lights_roof_mode
    
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

output_lights_roof_mode
    BANKSEL gear_mode
    btfss   gear_mode, GEAR_CHANGED_FLAG
    goto    output_lights_check_winch    

    btfss   gear_mode, GEAR_1
    goto    output_lights_check_gear_2
    call    output_lights_gear_1
    goto    output_lights_end

output_lights_check_gear_2
    btfsc   gear_mode, GEAR_2
    call    output_lights_gear_2
    goto    output_lights_end

output_lights_check_winch
    BANKSEL winch_mode
    movfw   winch_mode
    bz      output_lights_roof_manual
    
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
    goto    output_lights_end

    
output_lights_roof_manual
    BANKSEL light_gimmick_mode
    movf    light_gimmick_mode, w
    bz      output_lights_roof_manual_no_gimmick

    decf    WREG, f
    skpnz
    goto    output_lights_roof_gimmick1
    decf    WREG, f
    skpnz
    goto    output_lights_roof_gimmick2

    clrf    light_gimmick_mode
    goto    output_lights_roof_manual_no_gimmick

output_lights_roof_gimmick1
    call    output_lights_gimmick1
    goto    output_lights_end

output_lights_roof_gimmick2
    call    output_lights_gimmick2
    goto    output_lights_end
    
output_lights_roof_manual_no_gimmick
    ; Turn off any endless sequence by setting the sequencer_count to one,
    ; stopping the sequence after the current loop.
    ;
    ; This allows that gear sequences work well, but at the same time we can
    ; turn off gimmick sequences
    ;BANKSEL sequencer_count
    ;movf    sequencer_count, f
    ;skpnz   
    ;incf    sequencer_count
    
    BANKSEL light_mode
    btfss   light_mode, LIGHT_MODE_ROOF
    call    output_lights_roof_off
    BANKSEL light_mode
    btfsc   light_mode, LIGHT_MODE_ROOF
    call    output_lights_roof_on
;   goto    output_lights_end


output_lights_end
    call    Sequencer
    
    ; Push the sequencer LED values into the actual LED output
    BANKSEL seq_leds
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
;    goto    output_lights_execute    

output_lights_execute    
    call    TLC5940_send
    return


output_lights_startup
    btfss   startup_mode, STARTUP_MODE_NEUTRAL
    return
    
    movlw   VAL_ROOF
    movwf   light_data + LED_ROOF_1
    movwf   light_data + LED_ROOF_4
    goto    output_lights_execute    


output_lights_setup
    btfsc   setup_mode, SETUP_MODE_CENTRE
    call    output_lights_setup_centre
    BANKSEL setup_mode
    btfsc   setup_mode, SETUP_MODE_LEFT
    call    output_lights_setup_left
    BANKSEL setup_mode
    btfsc   setup_mode, SETUP_MODE_RIGHT
    call    output_lights_setup_right
    BANKSEL setup_mode
    btfsc   setup_mode, SETUP_MODE_STEERING_REVERSE 
    call    output_lights_indicator_left
    BANKSEL setup_mode
    btfsc   setup_mode, SETUP_MODE_THROTTLE_REVERSE 
    call    output_lights_main_beam
    goto    output_lights_execute    


output_lights_setup_centre
    return
    
output_lights_setup_left
output_lights_indicator_left
    BANKSEL light_data
    movlw   VAL_INDICATOR_FRONT
    movwf   light_data + LED_INDICATOR_F_L
    return

output_lights_setup_right
output_lights_indicator_right
    BANKSEL light_data
    movlw   VAL_INDICATOR_FRONT
    movwf   light_data + LED_INDICATOR_F_R
    return

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

;----------------------------
; Sequencer related light output functions
    
output_lights_roof_on
    movlw   SEQUENCER_MODE_ROOF_ON
    call    Sequencer_prepare
    btfss   WREG, 0                 ; Modal sequence or same sequence running? 
    return                          ; Yes: don't disturb

    BANKSEL table_l    
    movlw   HIGH table_roof_on
    movwf   table_h
    movlw   LOW table_roof_on
    movwf   table_l

    movlw   1                       ; Run this sequence once
    call    Sequencer_start
    return

output_lights_roof_off
    movlw   SEQUENCER_MODE_ROOF_OFF
    call    Sequencer_prepare
    btfss   WREG, 0                 ; Modal sequence or same sequence running? 
    return                          ; Yes: don't disturb

    BANKSEL table_l    
    movlw   HIGH table_roof_off
    movwf   table_h
    movlw   LOW table_roof_off
    movwf   table_l

    movlw   1                       ; Run this sequence once
    call    Sequencer_start
    return
    
output_lights_winch_idle
    movlw   SEQUENCER_MODE_WINCH_IDLE
    call    Sequencer_prepare
    btfss   WREG, 0                 ; Modal sequence or same sequence running? 
    return                          ; Yes: don't disturb

    BANKSEL table_l    
    movlw   HIGH running_light_table
    movwf   table_h
    movlw   LOW running_light_table
    movwf   table_l

    movlw   0                       ; Run this sequence forever
    call    Sequencer_start
    return
    
output_lights_winch_in
    movlw   SEQUENCER_MODE_WINCH_IN
    call    Sequencer_prepare
    btfss   WREG, 0                 ; Modal sequence or same sequence running? 
    return                          ; Yes: don't disturb

    BANKSEL table_l    
    movlw   HIGH night_rider_table
    movwf   table_h
    movlw   LOW night_rider_table
    movwf   table_l

    movlw   0                       ; Run this sequence forever
    call    Sequencer_start
    return
    
output_lights_winch_out
    movlw   SEQUENCER_MODE_WINCH_OUT
    call    Sequencer_prepare
    btfss   WREG, 0                 ; Modal sequence or same sequence running? 
    return                          ; Yes: don't disturb

    BANKSEL table_l    
    movlw   HIGH scan_right_table
    movwf   table_h
    movlw   LOW scan_right_table
    movwf   table_l

    movlw   0                       ; Run this sequence forever
    call    Sequencer_start
    return

output_lights_gear_1
    movlw   SEQUENCER_MODE_GEAR1    ; High priority sequence
    call    Sequencer_prepare
    btfss   WREG, 0                 ; Modal sequence or same sequence running? 
    return                          ; Yes: don't disturb

    BANKSEL table_l    
    movlw   HIGH table_gear
    movwf   table_h
    movlw   LOW table_gear
    movwf   table_l

    movlw   1                       ; Run this sequence once
    call    Sequencer_start
    return

output_lights_gear_2
    movlw   SEQUENCER_MODE_GEAR2    ; High priority sequence
    call    Sequencer_prepare
    btfss   WREG, 0                 ; Modal sequence or same sequence running? 
    return                          ; Yes: don't disturb

    BANKSEL table_l    
    movlw   HIGH table_gear
    movwf   table_h
    movlw   LOW table_gear
    movwf   table_l

    movlw   2                       ; Run this sequence twice
    call    Sequencer_start
    return
    
output_lights_gimmick1
    movlw   SEQUENCER_MODE_WINCH_IN
    call    Sequencer_prepare
    btfss   WREG, 0                 ; Modal sequence or same sequence running? 
    return                          ; Yes: don't disturb

    BANKSEL table_l    
    movlw   HIGH night_rider_table
    movwf   table_h
    movlw   LOW night_rider_table
    movwf   table_l

    movlw   0                       ; Run this sequence forever
    call    Sequencer_start
    return
    
output_lights_gimmick2
    movlw   SEQUENCER_MODE_WINCH_IN
    call    Sequencer_prepare
    btfss   WREG, 0                 ; Modal sequence or same sequence running? 
    return                          ; Yes: don't disturb

    BANKSEL table_l    
    movlw   HIGH night_rider_table
    movwf   table_h
    movlw   LOW night_rider_table
    movwf   table_l

    movlw   0                       ; Run this sequence forever
    call    Sequencer_start
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
; Sequencer_prepare
;
; This function provides business logic to support two different types of
; tables:
;
;   - High-priority tables that can not be interrupted by normal tables
;     Another high priority table can interrupt. Triggering the currently
;     running high priority table again causes it to start from the beginning.
;
;   - Normal tables that run if no high priority table is active.
;     If the same normal priority table is triggered nothing happens, the 
;     table continues without restart.
;
; Each sequencer_mode value indicates a table to use. If the bit 7 is set
; it indicates a high priority table.
;
; This function returns 0 in W if a high priority table is active or the
; currently running table is the same as a given normal priority table.
; It returns 1 in W if a high priority table is requested or the given 
; table differs from the currently running one.
;******************************************************************************
Sequencer_prepare
    BANKSEL sequencer_mode      

    btfsc   WREG, 7                 ; Modal sequence requested?
    goto    sequencer_prepare_do    ; Yes: always allow!
    
    btfsc   sequencer_mode, 7       ; Modal sequence running? 
    retlw   0                       ; Yes: don't disturb

    xorwf   sequencer_mode, w       ; Is this sequence already running?
    skpnz   
    retlw   0                       ; Yes: do nothing

    xorwf   sequencer_mode, w       ; Undo previous XOR 
sequencer_prepare_do
    movwf   sequencer_mode
    retlw   1


;******************************************************************************
; Sequencer_start
;
; Convenicene function to start (or re-start) a new sequence run.
;******************************************************************************
Sequencer_start
    BANKSEL sequencer_count
    movwf   sequencer_count
    
    movfw   table_l
    movwf   index_l
    movfw   table_h
    movwf   index_h
    clrf    sequencer_delay_count
    clrf    seq_leds + 0
    clrf    seq_leds + 1
    clrf    seq_leds + 2
    clrf    seq_leds + 3
    return


;******************************************************************************
; Sequencer
;
; This is a table based light sequencer for 4 LEDs (roof lights on the Wraith).
; The idea behind is to support different repeating light patterns that can
; be selected when needed.
;
; The entries in the tables are byte based. Each entry can be one of the 
; following commands:
;
;    - Set the brightness of an LED to a given value
;    - Delay for a given time
;    - End of table
;
; The "end of table" command has the unique value of 0x7f.
;
; If bit 7 is set the command is an LED command:
;   - Bits 5,6 define the LED number in the range 0..3
;   - Bits 0..4 define the brightness value from 0..31, which is multiplied by
;     two before the value is sent to the TLC5940.
;
; If bit 7 is cleared the command defines a delay before further table 
; commands are executed. Bits 0..7 define the delay in soft timer units. If
; the delay is 0 then table processing continues in the next mainloop run
; (usually synchronized to the servo signals).
; With a soft timer triggering every 65.536 ms a maximum delay of 8.2 s can be
; achieved.
;
; sequencer_count defines how often the table is run. A value of 0 means
; that the table is run forever, any other value indicates the number of times
; the table is looped.
;
; Note that the sequencer executes table commands until
;   - A delay command is encountered
;   - The table has been executed sequencer_count times
; This means you can accidentally halt the software by not having any
; delay commands in your table and setting sequencer_count to 0!
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
    movf    sequencer_delay_count, f
    skpz
    return
    
sequencer_loop
    movf    sequencer_mode, f           ; Any sequence active?
    skpnz
    return                              ; No: stop now.

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

sequencer_at_end
    BANKSEL table_l
    movfw   table_l
    movwf   index_l
    movfw   table_h
    movwf   index_h

    movfw   sequencer_count             ; Endless loop requested? (count = 0)
    skpnz       
    goto    sequencer_loop              ; Continue processing from the first 
                                        ; item in the table

    decfsz  sequencer_count, f
    goto    sequencer_loop              ; Next loop if not done

    clrf    sequencer_mode
    return
    
sequencer_not_at_end
    xorlw   0x7f                        ; Restore the original delay value
    BANKSEL sequencer_delay_count
    movwf   sequencer_delay_count
    RETURN

    
;******************************************************************************
; Lookup_table
;
; Returns  W = *index++
;******************************************************************************
Lookup_table
    BANKSEL index_l
    movfw   index_h
    movwf   PCLATH
    movfw   index_l
    incf    index_l, f          ; Post-increment
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
#define NIGHT_RIDER_DELAY b'00000001'

#define LED1    b'10000000'
#define LED2    b'10100000'
#define LED3    b'11000000'
#define LED4    b'11100000'

#define OFF     b'00000000'
#define ON      b'00011111'
#define HALF    b'00001111'
#define QUARTER b'00000111'
#define DIM     b'00000011'

    retlw   LED1 + HALF
    retlw   NIGHT_RIDER_DELAY
    retlw   LED1 + ON
    retlw   NIGHT_RIDER_DELAY
    retlw   LED2 + HALF
    retlw   NIGHT_RIDER_DELAY
    retlw   LED1 + HALF
    retlw   LED2 + ON
    retlw   NIGHT_RIDER_DELAY
    retlw   LED1 + QUARTER
    retlw   LED3 + HALF
    retlw   NIGHT_RIDER_DELAY
    retlw   LED1 + DIM
    retlw   LED2 + HALF
    retlw   LED3 + ON
    retlw   NIGHT_RIDER_DELAY
    retlw   LED1 + OFF
    retlw   LED2 + QUARTER
    retlw   LED4 + HALF
    retlw   NIGHT_RIDER_DELAY
    retlw   LED2 + DIM
    retlw   LED3 + HALF
    retlw   LED4 + ON
    retlw   NIGHT_RIDER_DELAY
    retlw   LED2 + OFF
    retlw   LED3 + DIM
    retlw   NIGHT_RIDER_DELAY
    retlw   LED3 + OFF
    retlw   LED4 + HALF
    retlw   NIGHT_RIDER_DELAY
    retlw   LED4 + QUARTER
    retlw   NIGHT_RIDER_DELAY
    retlw   LED4 + DIM
    retlw   NIGHT_RIDER_DELAY
    retlw   LED4 + OFF
    retlw   NIGHT_RIDER_DELAY * 2

    retlw   LED4 + HALF
    retlw   NIGHT_RIDER_DELAY
    retlw   LED4 + ON
    retlw   NIGHT_RIDER_DELAY
    retlw   LED3 + HALF
    retlw   NIGHT_RIDER_DELAY
    retlw   LED4+ HALF
    retlw   LED3+ ON
    retlw   NIGHT_RIDER_DELAY
    retlw   LED4 + QUARTER
    retlw   LED2+ HALF
    retlw   NIGHT_RIDER_DELAY
    retlw   LED4 + DIM
    retlw   LED3 + HALF
    retlw   LED2 + ON
    retlw   NIGHT_RIDER_DELAY
    retlw   LED4 + OFF
    retlw   LED3 + QUARTER
    retlw   LED1 + HALF
    retlw   NIGHT_RIDER_DELAY
    retlw   LED3 + DIM
    retlw   LED2 + HALF
    retlw   LED1 + ON
    retlw   NIGHT_RIDER_DELAY
    retlw   LED3 + OFF
    retlw   LED2 + DIM
    retlw   NIGHT_RIDER_DELAY
    retlw   LED2 + OFF
    retlw   LED1 + HALF
    retlw   NIGHT_RIDER_DELAY
    retlw   LED1 + QUARTER
    retlw   NIGHT_RIDER_DELAY
    retlw   LED1 + DIM
    retlw   NIGHT_RIDER_DELAY
    retlw   LED1 + OFF
    retlw   NIGHT_RIDER_DELAY * 2

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


table_roof_on
    retlw   b'10011111'     ; LED 1 (left-most) on full brightness
    retlw   b'10111111'     ; LED 2 on
    retlw   b'11011111'     ; LED 3 on
    retlw   b'11111111'     ; LED 4 on
    retlw   b'01111111'     ; END OF TABLE


table_roof_off
                            ; (no command, all LEDs off for new sequences!)
    retlw   b'01111111'     ; END OF TABLE


;******************************************************************************
; One glowing flash from the middle outwards
table_gear
    retlw   b'10100111'     ; LED 2 dim
    retlw   b'11000111'     ; LED 3 dim
    retlw   b'00000001'     ; Delay 65ms
    retlw   b'10111111'     ; LED 2 on
    retlw   b'11011111'     ; LED 3 on
    retlw   b'10000111'     ; LED 1 (left-most) dim
    retlw   b'11100111'     ; LED 4 dim
    retlw   b'00000001'     ; Delay 65ms
    retlw   b'10011111'     ; LED 1 on
    retlw   b'11111111'     ; LED 4 on
    retlw   b'00000011'     ; Delay 65ms
    retlw   b'10000111'     ; LED 1 dim
    retlw   b'11100111'     ; LED 4 dim
    retlw   b'00000001'     ; Delay 65ms
    retlw   b'10100111'     ; LED 2 dim
    retlw   b'11000111'     ; LED 3 dim
    retlw   b'10000000'     ; LED 1 off
    retlw   b'11100000'     ; LED 4 off
    retlw   b'00000001'     ; Delay 65ms
    retlw   b'10100000'     ; LED 2 off
    retlw   b'11000000'     ; LED 3 off
    retlw   b'00000001'     ; Delay 65ms
    retlw   b'01111111'     ; END OF TABLE
    
    END
