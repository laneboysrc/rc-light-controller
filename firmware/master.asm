;******************************************************************************
;
;   master.asm
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************
    TITLE       RC Light Controller - Master
    LIST        r=dec
    RADIX       dec

#define INCLUDE_CONFIG
    #include    hw.tmp

    
    GLOBAL blink_mode
    GLOBAL light_mode
    GLOBAL drive_mode
    GLOBAL setup_mode
    GLOBAL startup_mode
    GLOBAL servo


    ; Functions imported from <car>-lights.asm
    EXTERN Init_lights
    EXTERN Output_lights


    ; Functions and variables imported from utils.asm
    EXTERN Min
    EXTERN Max
    EXTERN Add_y_to_x
    EXTERN Sub_y_from_x
    EXTERN If_x_eq_y
    EXTERN If_y_lt_x
    EXTERN Min_x_z
    EXTERN Max_x_z
    EXTERN Mul_xl_by_w
    EXTERN Div_x_by_y
    EXTERN Mul_x_by_100
    EXTERN Div_x_by_4
    EXTERN Mul_x_by_6
    EXTERN Add_x_and_780    
    EXTERN UART_send_w
    EXTERN Random_min_max

    EXTERN xl
    EXTERN xh
    EXTERN yl
    EXTERN yh
    EXTERN zl
    EXTERN zh
    EXTERN temp


    ; Functions and variables imported from *_reader.asm
    EXTERN Read_all_channels
    EXTERN Init_reader
    
    EXTERN steering            
    EXTERN steering_abs       
    EXTERN steering_reverse
    EXTERN throttle            
    EXTERN throttle_abs       
    EXTERN throttle_reverse
    EXTERN ch3                 
     
     
    ; Functions and variables imported from servo-output.asm
    EXTERN Init_servo_output
    EXTERN Make_servo_pulse     

; All timings below are referenced to the soft timer running at 65.536 ms
    
#define CH3_BUTTON_TIMEOUT 6    ; Time in which we accept double-click of CH3
#define BLINK_COUNTER_VALUE 5   ; 5 * 65.536 ms = ~333 ms = ~1.5 Hz
IFNDEF AUTO_BRAKE_COUNTER_VALUE_REVERSE_MIN
#define AUTO_BRAKE_COUNTER_VALUE_REVERSE_MIN 12
ENDIF
IFNDEF AUTO_BRAKE_COUNTER_VALUE_REVERSE_MAX
#define AUTO_BRAKE_COUNTER_VALUE_REVERSE_MAX 38
ENDIF
IFNDEF AUTO_BRAKE_COUNTER_VALUE_FORWARD_MIN
#define AUTO_BRAKE_COUNTER_VALUE_FORWARD_MIN 12
ENDIF
IFNDEF AUTO_BRAKE_COUNTER_VALUE_FORWARD_MAX
#define AUTO_BRAKE_COUNTER_VALUE_FORWARD_MAX 38
ENDIF
IFNDEF AUTO_REVERSE_COUNTER_VALUE_MIN
#define AUTO_REVERSE_COUNTER_VALUE_MIN 12
ENDIF
IFNDEF AUTO_REVERSE_COUNTER_VALUE_MAX
#define AUTO_REVERSE_COUNTER_VALUE_MAX 30
ENDIF

#define BRAKE_DISARM_COUNTER_VALUE 15        ; 15 * 65.536 ms = ~1 s
#define INDICATOR_STATE_COUNTER_VALUE 8      ; 8 * 65.536 ms = ~0.5 s
#define INDICATOR_STATE_COUNTER_VALUE_OFF 30 ; ~2 s

; Bitfields in variable flags
#define CH3_FLAG_LAST_STATE 0           ; Must be bit 0!
#define CH3_FLAG_TANSITIONED 1
#define CH3_FLAG_INITIALIZED 2
#define SOFT_TIMER_POSTSCALER 3

; Bitfields in variable blink_mode
#define BLINK_MODE_BLINKFLAG 0          ; Toggles with 1.5 Hz
#define BLINK_MODE_HAZARD 1             ; Hazard lights active
#define BLINK_MODE_INDICATOR_LEFT 2     ; Left indicator active
#define BLINK_MODE_INDICATOR_RIGHT 3    ; Right indicator active

; Bitfields in variable drive_mode
#define DRIVE_MODE_FORWARD 0 
#define DRIVE_MODE_BRAKE 1 
#define DRIVE_MODE_REVERSE 2
#define DRIVE_MODE_BRAKE_ARMED 3
#define DRIVE_MODE_AUTO_BRAKE 4
#define DRIVE_MODE_BRAKE_DISARM 5
#define DRIVE_MODE_AUTO_REVERSE 6

#define CENTRE_THRESHOLD 10
#define STEERING_BLINK_THRESHOLD 50
#define STEERING_BLINK_OFF_THRESHOLD 30

#define EEPROM_MAGIC1 0x13
#define EEPROM_MAGIC2 0x42

#define EEPROM_ADR_MAGIC1 0      
#define EEPROM_ADR_SERVO_EPL 1
#define EEPROM_ADR_SERVO_CENTRE 2
#define EEPROM_ADR_SERVO_EPR 3
#define EEPROM_ADR_STEERING_REVERSE 4
#define EEPROM_ADR_THROTTLE_REVERSE 5
#define EEPROM_ADR_MAGIC2 6

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


IFNDEF LIGHT_MODE_MASK
#define LIGHT_MODE_MASK b'00001111'
ENDIF

; If the master has a servo output we must for sure enable its configuration
; even if the user forgot to set the flag in the makefile
IFDEF ENABLE_SERVO_OUTPUT
IFNDEF ENABLE_STEERING_WHEEL_SERVO_SETUP
#define ENABLE_STEERING_WHEEL_SERVO_SETUP
ENDIF
ENDIF

;******************************************************************************
;* VARIABLE DEFINITIONS
;******************************************************************************
.data_master UDATA

auto_brake_counter  res 1
auto_reverse_counter res 1
drive_mode_brake_disarm_counter res 1
indicator_state_counter res 1
blink_counter       res 1

flags               res 1

ch3_click_counter   res 1
ch3_clicks          res 1

indicator_state     res 1

blink_mode          res 1
light_mode          res 1
drive_mode          res 1
setup_mode          res 1
startup_mode        res 1

servo               res 1
servo_epl           res 1
servo_centre        res 1
servo_epr           res 1
servo_setup_epl     res 1
servo_setup_centre  res 1
servo_ep_sign_flag  res 1


;******************************************************************************
;* MACROS
;******************************************************************************
swap_x_y    macro   x, y
    ; Currently X contains A; Y contains B
    movf    x, w    ; W = A
    xorwf   y, w    ; W = A ^ B
    xorwf   x, f    ; X = ((A^B)^A) = B
    xorwf   y, f    ; Y = ((A^B)^B) = A
    ; Now X contains B. Y contains A.
            endm



;******************************************************************************
; Reset vector 
;******************************************************************************
.code_reset CODE    0x000           
    goto    Init


;******************************************************************************
; Relocatable code section
;******************************************************************************
.code_master CODE

;******************************************************************************
; Initialization
;******************************************************************************
Init
    ;-----------------------------
    ; Initialise the chip (macro included from hw_*.tmp)
    IO_INIT_MASTER

    ; Initialize local variables
    BANKSEL flags
    clrf    flags
    clrf    blink_mode
    clrf    light_mode
    clrf    drive_mode
    clrf    setup_mode
    clrf    startup_mode
    clrf    auto_brake_counter
    clrf    drive_mode_brake_disarm_counter
    clrf    indicator_state_counter
    clrf    ch3_click_counter
    clrf    ch3_clicks
    clrf    indicator_state
    clrf    servo

    movlw   BLINK_COUNTER_VALUE
    movwf   blink_counter

    call    Init_lights

    IFDEF   ENABLE_SERVO_OUTPUT
    call    Init_servo_output
    ENDIF

    call    EEPROM_load_persistent_data
    call    Init_reader

;   goto    Main_loop    


;**********************************************************************
; Main program
;**********************************************************************
Main_loop
    call    Read_all_channels

    call    Process_ch3_double_click
    call    Process_drive_mode
    call    Process_indicators
    call    Process_channel_reversing
    call    Process_steering_wheel_servo
    call    Service_soft_timer

    call    Output_lights
    
    IFDEF   ENABLE_SERVO_OUTPUT
    BANKSEL servo
    movfw   servo
    call    Make_servo_pulse
    ENDIF

    goto    Main_loop

    
;******************************************************************************
; Service_soft_timer
;
; Soft-timer with a resolution of 65.536 ms
;******************************************************************************
Service_soft_timer
IFDEF TIMER2_SOFT_TIMER
    BANKSEL PIR1
    btfss   PIR1, TMR2IF
    return

    bcf     PIR1, TMR2IF

    ; In case our oscillator runs at more than 16 MHz we can not set Timer2
    ; to overflow every 65.536ms. Instead we set it to 32.768ms and do a 
    ; 1/2 post scaler using a flag.
    IF (FOSC > 16)
    BANKSEL flags
    movlw   1<<SOFT_TIMER_POSTSCALER
    xorwf   flags, f
    btfss   flags, SOFT_TIMER_POSTSCALER
    return
    ENDIF ; FOSC > 16
ELSE   
    BANKSEL INTCON
    btfss   INTCON, T0IF
    return

    bcf     INTCON, T0IF
ENDIF


    BANKSEL ch3_click_counter
    movf    ch3_click_counter, f
    skpz     
    decf    ch3_click_counter, f    


    movf    indicator_state_counter, f
    skpz     
    decf    indicator_state_counter, f    


    decfsz  drive_mode_brake_disarm_counter, f
    goto    service_soft_timer_auto_brake

    btfss   drive_mode, DRIVE_MODE_BRAKE_DISARM
    goto    service_soft_timer_auto_brake

    bcf     drive_mode, DRIVE_MODE_BRAKE_DISARM
    bcf     drive_mode, DRIVE_MODE_BRAKE_ARMED


service_soft_timer_auto_brake
    decfsz  auto_brake_counter, f
    goto    service_soft_timer_auto_reverse

    btfss   drive_mode, DRIVE_MODE_AUTO_BRAKE
    goto    service_soft_timer_auto_reverse

    bcf     drive_mode, DRIVE_MODE_AUTO_BRAKE
    bcf     drive_mode, DRIVE_MODE_BRAKE


service_soft_timer_auto_reverse
    decfsz  auto_reverse_counter, f
    goto    service_soft_timer_blink

    btfss   drive_mode, DRIVE_MODE_AUTO_REVERSE
    goto    service_soft_timer_blink

    bcf     drive_mode, DRIVE_MODE_AUTO_REVERSE
    bcf     drive_mode, DRIVE_MODE_REVERSE


service_soft_timer_blink
    decfsz  blink_counter, f
    return

    movlw   BLINK_COUNTER_VALUE
    movwf   blink_counter
    movlw   1 << BLINK_MODE_BLINKFLAG
    xorwf   blink_mode, f

    return


;******************************************************************************
; Synchronize_blinking
;
; This function ensures that blinking always starts with a full "on" period.
; It resets the blink counter and sets the blink flag, but only if none
; of hazard and indicator are already on (= blinking)
;******************************************************************************
Synchronize_blinking
    BANKSEL blink_mode
    btfsc   blink_mode, BLINK_MODE_HAZARD
    return
    btfsc   blink_mode, BLINK_MODE_INDICATOR_LEFT
    return
    btfsc   blink_mode, BLINK_MODE_INDICATOR_RIGHT
    return

    movlw   BLINK_COUNTER_VALUE
    movwf   blink_counter
    bsf     blink_mode, BLINK_MODE_BLINKFLAG
    return


;******************************************************************************
; Process_ch3_double_click
;******************************************************************************
Process_ch3_double_click
    BANKSEL startup_mode
    movf    startup_mode, f
    bz      process_ch3_no_startup
    return

process_ch3_no_startup
    btfsc   flags, CH3_FLAG_INITIALIZED
    goto    process_ch3_initialized

    ; Ignore the potential "toggle" after power on
    bsf     flags, CH3_FLAG_INITIALIZED
    bcf     flags, CH3_FLAG_LAST_STATE
    BANKSEL ch3
    btfss   ch3, CH3_FLAG_LAST_STATE
    return
    BANKSEL flags               
    bsf     flags, CH3_FLAG_LAST_STATE
    return

process_ch3_initialized
    BANKSEL ch3
    movfw   ch3
    movwf   temp+1  

    ; ch3 is only using bit 0, the same bit as CH3_FLAG_LAST_STATE.
    ; We can therefore use XOR to determine whether ch3 has changed.
   
    BANKSEL flags               
    xorwf   flags, w        
    movwf   temp

IFDEF CH3_MOMENTARY
    ; -------------------------------------------------------    
    ; Code for CH3 having a momentory signal when pressed (Futaba 4PL)

    ; We only care about the switch transition from CH3_FLAG_LAST_STATE 
    ; (set upon initialization) to the opposite position, which is when 
    ; we add a click.
    btfsc   temp, CH3_FLAG_LAST_STATE
    goto    process_ch3_potential_click

    ; ch3 is the same as CH3_FLAG_LAST_STATE (idle position), therefore reset 
    ; our "transitioned" flag to detect the next transition.
    bcf     flags, CH3_FLAG_TANSITIONED        
    goto    process_ch3_click_timeout

process_ch3_potential_click
    ; Did we register this transition already?    
    ;   Yes: check for click timeout.
    ;   No: Register transition and add click
    btfsc   flags, CH3_FLAG_TANSITIONED
    goto    process_ch3_click_timeout

    bsf     flags, CH3_FLAG_TANSITIONED
    ;goto   process_ch3_add_click    

ELSE
    ; -------------------------------------------------------    
    ; Code for CH3 being a two position switch (HK-310, GT3B)

    ; Check whether ch3 has changed with respect to LAST_STATE
    btfss   temp, CH3_FLAG_LAST_STATE
    goto    process_ch3_click_timeout

    bcf     flags, CH3_FLAG_LAST_STATE      ; Store the new ch3 state
    btfsc   temp+1, CH3_FLAG_LAST_STATE     ; temp+1 contains ch3
    bsf     flags, CH3_FLAG_LAST_STATE
    ;goto   process_ch3_add_click    

    ; -------------------------------------------------------    
ENDIF

process_ch3_add_click
    BANKSEL ch3_clicks               
    incf    ch3_clicks, f
    movlw   CH3_BUTTON_TIMEOUT
    movwf   ch3_click_counter
    return
    
process_ch3_click_timeout
    movf    ch3_clicks, f           ; Any buttons pending?
    skpnz   
    return                          ; No: done

    movf    ch3_click_counter, f    ; Double-click timer expired?
    skpz   
    return                          ; No: wait for more buttons


    movf    setup_mode, f
    bz      process_ch3_click_no_setup

    ;====================================
    ; Steering servo setup in progress:
    ; 1 click: next setup step
    ; more than 1 click: cancel setup
    decfsz  ch3_clicks, f                
    goto    process_ch3_setup_cancel
    bsf     setup_mode, SETUP_MODE_NEXT
    return    
    
process_ch3_setup_cancel
    bsf     setup_mode, SETUP_MODE_CANCEL
    return    

    ;====================================
    ; Normal operation; setup is not active
process_ch3_click_no_setup
    decfsz  ch3_clicks, f                
    goto    process_ch3_double_click

    ; --------------------------
    ; Single click: switch light mode up (Parking, Low Beam, Fog, High Beam) 
    rlf     light_mode, f
    bsf     light_mode, 0
    movlw   LIGHT_MODE_MASK
    andwf   light_mode, f
    return

process_ch3_double_click
    decfsz  ch3_clicks, f              
    goto    process_ch3_triple_click

    ; --------------------------
    ; Double click: switch light mode down (Parking, Low Beam, Fog, High Beam)  
    rrf     light_mode, f
    movlw   LIGHT_MODE_MASK
    andwf   light_mode, f
    return

process_ch3_triple_click
    decfsz  ch3_clicks, f              
    goto    process_ch3_quad_click

    ; --------------------------
    ; Triple click: all lights on/off
    movlw   LIGHT_MODE_MASK
    andwf   light_mode, w
    sublw   LIGHT_MODE_MASK
    movlw   LIGHT_MODE_MASK
    skpnz
    movlw   0x00     
    movwf   light_mode
    return

process_ch3_quad_click
    decfsz  ch3_clicks, f              
    goto    process_ch3_5_click

    ; --------------------------
    ; Quad click: Hazard lights on/off  
    clrf    ch3_clicks
    call    Synchronize_blinking
    BANKSEL blink_mode
    movlw   1 << BLINK_MODE_HAZARD
    xorwf   blink_mode, f
    return

process_ch3_5_click
    decfsz  ch3_clicks, f              
    goto    process_ch3_6_click
    goto    process_ch3_click_end

process_ch3_6_click
    decfsz  ch3_clicks, f              
    goto    process_ch3_7_click
    goto    process_ch3_click_end

process_ch3_7_click
    decfsz  ch3_clicks, f
    goto    process_ch3_8_click

    ; --------------------------
    ; 7 clicks: Enter steering channel reverse setup mode
    clrf    ch3_clicks
    movlw   (1 << SETUP_MODE_STEERING_REVERSE) + (1 << SETUP_MODE_THROTTLE_REVERSE)
    movwf   setup_mode    
    return

process_ch3_8_click
    decfsz  ch3_clicks, f
    goto    process_ch3_click_end

    ; --------------------------
    ; 8 clicks: Enter steering wheel servo setup mode
    clrf    ch3_clicks
    IFDEF ENABLE_STEERING_WHEEL_SERVO_SETUP    
    movlw   1 << SETUP_MODE_INIT
    movwf   setup_mode    
    ENDIF
    return

process_ch3_click_end
    clrf    ch3_clicks
    return


;******************************************************************************
; Process_drive_mode
;
; Simulates the state machine in the ESC and updates the variable drive_mode
; accordingly.
;
; Currently programmed for the HPI SC-15WP
;
; +/-10: forward = 0, reverse = 0
; >+10: forward = 1, brake_armed = 1
; <-10:
;   if brake_armed: brake = 1
;   if not brake_armed: reverse = 1, brake = 0
; 2 seconds in Neutral: brake_armed = 0
; Brake -> Neutral: brake = 0, brake_armed = 0
; Reverse -> Neutral: brake = 1 for 2 seconds
;******************************************************************************
Process_drive_mode
    BANKSEL throttle_abs
    movlw   CENTRE_THRESHOLD
    subwf   throttle_abs, w
    bc      process_drive_mode_not_neutral

process_drive_mode_neutral
    BANKSEL drive_mode
    btfsc   drive_mode, DRIVE_MODE_FORWARD
    goto    process_drive_mode_neutral_after_forward    
    btfsc   drive_mode, DRIVE_MODE_REVERSE
    goto    process_drive_mode_neutral_after_reverse
    btfsc   drive_mode, DRIVE_MODE_BRAKE
    goto    process_drive_mode_neutral_after_brake
    return    

process_drive_mode_neutral_after_forward
    bcf     drive_mode, DRIVE_MODE_FORWARD

    ; If DISABLE_BRAKE_DISARM_TIMEOUT is defined the user has to go for 
    ; brake, then neutral, before reverse engages. Otherwise reverse engages
    ; if the user stays in neutral for a few seconds.
    ;
    ; Tamiya ESC need this DISABLE_BRAKE_DISARM_TIMEOUT defined.
    ; The China ESC and HPI SC-15WP need DISABLE_BRAKE_DISARM_TIMEOUT undefined.     
IFNDEF DISABLE_BRAKE_DISARM_TIMEOUT
    bsf     drive_mode, DRIVE_MODE_BRAKE_DISARM
    movlw   BRAKE_DISARM_COUNTER_VALUE
    movwf   drive_mode_brake_disarm_counter   
ENDIF
    
IFNDEF DISABLE_AUTO_BRAKE_LIGHTS_FORWARD    
    bsf     drive_mode, DRIVE_MODE_BRAKE
    
    ; The time the brake lights stay on after going back to neutral is random
    bsf     drive_mode, DRIVE_MODE_AUTO_BRAKE
    movlw   AUTO_BRAKE_COUNTER_VALUE_FORWARD_MIN
    movwf   yl 
    movlw   AUTO_BRAKE_COUNTER_VALUE_FORWARD_MAX
    movwf   yh 
    call    Random_min_max
    BANKSEL auto_brake_counter
    movwf   auto_brake_counter
ENDIF    
    return

process_drive_mode_neutral_after_reverse
    btfsc   drive_mode, DRIVE_MODE_AUTO_REVERSE
    return

    ; The time the reverse lights stay on after going back to neutral is random
    bsf     drive_mode, DRIVE_MODE_AUTO_REVERSE
    movlw   AUTO_REVERSE_COUNTER_VALUE_MIN
    movwf   yl 
    movlw   AUTO_REVERSE_COUNTER_VALUE_MAX
    movwf   yh 
    call    Random_min_max
    BANKSEL auto_reverse_counter 
    movwf   auto_reverse_counter   

IFNDEF DISABLE_AUTO_BRAKE_LIGHTS_REVERSE    
    bsf     drive_mode, DRIVE_MODE_BRAKE
    
    ; The time the brake lights stay on after going back to neutral is random
    bsf     drive_mode, DRIVE_MODE_AUTO_BRAKE
    movlw   AUTO_BRAKE_COUNTER_VALUE_REVERSE_MIN
    movwf   yl 
    movlw   AUTO_BRAKE_COUNTER_VALUE_REVERSE_MAX
    movwf   yh 
    call    Random_min_max
    BANKSEL auto_brake_counter
    movwf   auto_brake_counter
ENDIF    
    return

process_drive_mode_neutral_after_brake
    btfsc   drive_mode, DRIVE_MODE_AUTO_BRAKE
    return
    bcf     drive_mode, DRIVE_MODE_BRAKE
    bcf     drive_mode, DRIVE_MODE_BRAKE_ARMED
    return

process_drive_mode_not_neutral
    BANKSEL drive_mode
    bcf     drive_mode, DRIVE_MODE_AUTO_BRAKE
    bcf     drive_mode, DRIVE_MODE_BRAKE_DISARM

    BANKSEL throttle
    btfsc   throttle, 7
    goto    process_drive_mode_brake_or_reverse

    BANKSEL drive_mode
    bsf     drive_mode, DRIVE_MODE_FORWARD
IFNDEF ESC_FORWARD_REVERSE    
    bsf     drive_mode, DRIVE_MODE_BRAKE_ARMED
ENDIF    
    bcf     drive_mode, DRIVE_MODE_REVERSE
    bcf     drive_mode, DRIVE_MODE_BRAKE
    return

process_drive_mode_brake_or_reverse
    BANKSEL drive_mode
    btfsc   drive_mode, DRIVE_MODE_BRAKE_ARMED
    goto    process_drive_mode_brake

    bsf     drive_mode, DRIVE_MODE_REVERSE
    bcf     drive_mode, DRIVE_MODE_AUTO_REVERSE
    bcf     drive_mode, DRIVE_MODE_BRAKE
    bcf     drive_mode, DRIVE_MODE_FORWARD
    return
    
process_drive_mode_brake
    bsf     drive_mode, DRIVE_MODE_BRAKE
    bcf     drive_mode, DRIVE_MODE_FORWARD
    bcf     drive_mode, DRIVE_MODE_REVERSE
    return


;******************************************************************************
; Process_indicators
; 
; Implements a sensible indicator algorithm.
;
; To turn on the indicators, throtte and steering must be centered for 2 s,
; then steering must be either left or right >50% for more than 2 s.
;
; Indicators are turned off when: 
;   - opposite steering is >30%
;   - steering neutral or opposite for >2s
;******************************************************************************
#define STATE_INDICATOR_NOT_NEUTRAL 0
#define STATE_INDICATOR_NEUTRAL_WAIT 1
#define STATE_INDICATOR_BLINK_ARMED 2
#define STATE_INDICATOR_BLINK_ARMED_LEFT 3
#define STATE_INDICATOR_BLINK_ARMED_RIGHT 4
#define STATE_INDICATOR_BLINK_LEFT 5
#define STATE_INDICATOR_BLINK_LEFT_WAIT 6
#define STATE_INDICATOR_BLINK_RIGHT 7
#define STATE_INDICATOR_BLINK_RIGHT_WAIT 8

Process_indicators
    BANKSEL indicator_state
    movf    indicator_state, w
    movwf   temp
    skpnz   
    goto    process_indicators_not_neutral
    decf    temp, f
    skpnz   
    goto    process_indicators_neutral_wait
    decf    temp, f
    skpnz   
    goto    process_indicators_blink_armed
    decf    temp, f
    skpnz   
    goto    process_indicators_blink_armed_left
    decf    temp, f
    skpnz   
    goto    process_indicators_blink_armed_right
    decf    temp, f
    skpnz   
    goto    process_indicators_blink_left
    decf    temp, f
    skpnz   
    goto    process_indicators_blink_left_wait
    decf    temp, f
    skpnz   
    goto    process_indicators_blink_right
    goto    process_indicators_blink_right_wait

process_indicators_not_neutral
    movlw   CENTRE_THRESHOLD
    BANKSEL throttle_abs
    subwf   throttle_abs, w
    skpnc
    return

    movlw   CENTRE_THRESHOLD
    subwf   steering_abs, w
    skpnc
    return

    BANKSEL indicator_state_counter
    movlw   INDICATOR_STATE_COUNTER_VALUE
    movwf   indicator_state_counter
    movlw   STATE_INDICATOR_NEUTRAL_WAIT
    movwf   indicator_state
    return

process_indicators_neutral_wait
    BANKSEL throttle_abs
    movlw   CENTRE_THRESHOLD
    subwf   throttle_abs, w
    bc      process_indicators_set_not_neutral

    movlw   CENTRE_THRESHOLD
    subwf   steering_abs, w
    bc      process_indicators_set_not_neutral

    BANKSEL indicator_state_counter
    movf    indicator_state_counter, f
    skpz    
    return

process_indicators_set_blink_armed
    BANKSEL indicator_state
    movlw   STATE_INDICATOR_BLINK_ARMED
    movwf   indicator_state
    return

process_indicators_set_not_neutral
    BANKSEL indicator_state
    movlw   STATE_INDICATOR_NOT_NEUTRAL
    movwf   indicator_state
    bcf     blink_mode, BLINK_MODE_INDICATOR_RIGHT
    bcf     blink_mode, BLINK_MODE_INDICATOR_LEFT
    return

process_indicators_blink_armed
    BANKSEL throttle_abs
    movlw   CENTRE_THRESHOLD
    subwf   throttle_abs, w
    bc      process_indicators_set_not_neutral

    movlw   STEERING_BLINK_THRESHOLD
    subwf   steering_abs, w
    skpc
    return    

    movfw   steering
    movwf   temp
    BANKSEL indicator_state_counter
    movlw   INDICATOR_STATE_COUNTER_VALUE
    movwf   indicator_state_counter    
    movlw   STATE_INDICATOR_BLINK_ARMED_LEFT
    btfss   temp, 7                 ; temp = steering
    movlw   STATE_INDICATOR_BLINK_ARMED_RIGHT
    movwf   indicator_state
    return
  
process_indicators_blink_armed_left  
    BANKSEL throttle_abs
    movlw   CENTRE_THRESHOLD
    subwf   throttle_abs, w
    bc      process_indicators_set_not_neutral

    movlw   STEERING_BLINK_THRESHOLD
    subwf   steering_abs, w
    bnc     process_indicators_set_blink_armed

    btfss   steering, 7 
    goto    process_indicators_set_blink_armed

    BANKSEL indicator_state_counter
    movf    indicator_state_counter, f
    skpz    
    return

process_indicators_set_blink_left  
    BANKSEL indicator_state
    movlw   STATE_INDICATOR_BLINK_LEFT
    movwf   indicator_state
    call    Synchronize_blinking
    bsf     blink_mode, BLINK_MODE_INDICATOR_LEFT
    return

process_indicators_blink_armed_right  
    BANKSEL throttle_abs
    movlw   CENTRE_THRESHOLD
    subwf   throttle_abs, w
    bc      process_indicators_set_not_neutral

    movlw   STEERING_BLINK_THRESHOLD
    subwf   steering_abs, w
    bnc     process_indicators_set_blink_armed

    btfsc   steering, 7 
    goto    process_indicators_set_blink_armed

    BANKSEL indicator_state_counter
    movf    indicator_state_counter, f
    skpz    
    return

process_indicators_set_blink_right
    BANKSEL indicator_state
    movlw   STATE_INDICATOR_BLINK_RIGHT
    movwf   indicator_state
    call    Synchronize_blinking
    BANKSEL blink_mode
    bsf     blink_mode, BLINK_MODE_INDICATOR_RIGHT
    return

process_indicators_blink_left
    BANKSEL steering
    btfsc   steering, 7 
    goto    process_indicators_blink_left_centre

    movlw   STEERING_BLINK_THRESHOLD
    subwf   steering_abs, w
    bc      process_indicators_set_not_neutral

process_indicators_blink_left_centre
    BANKSEL steering_abs
    movlw   CENTRE_THRESHOLD
    subwf   steering_abs, w
    skpnc
    return

    BANKSEL indicator_state_counter
    movlw   INDICATOR_STATE_COUNTER_VALUE_OFF
    movwf   indicator_state_counter             
    movlw   STATE_INDICATOR_BLINK_LEFT_WAIT
    movwf   indicator_state
    return

process_indicators_blink_left_wait
    BANKSEL steering
    btfsc   steering, 7 
    goto    process_indicators_blink_left_wait_centre

    movlw   STEERING_BLINK_THRESHOLD
    subwf   steering_abs, w
    bc      process_indicators_set_not_neutral

process_indicators_blink_left_wait_centre
    BANKSEL steering_abs
    movlw   CENTRE_THRESHOLD
    subwf   steering_abs, w
    bc      process_indicators_set_blink_left

    BANKSEL indicator_state_counter
    movf    indicator_state_counter, f
    skpz    
    return
    goto    process_indicators_set_not_neutral

process_indicators_blink_right
    BANKSEL steering
    btfss   steering, 7 
    goto    process_indicators_blink_right_centre

    movlw   STEERING_BLINK_THRESHOLD
    subwf   steering_abs, w
    bnc     process_indicators_set_not_neutral

process_indicators_blink_right_centre
    BANKSEL steering_abs
    movlw   CENTRE_THRESHOLD
    subwf   steering_abs, w
    skpnc
    return

    BANKSEL indicator_state_counter
    movlw   INDICATOR_STATE_COUNTER_VALUE_OFF
    movwf   indicator_state_counter             
    movlw   STATE_INDICATOR_BLINK_RIGHT_WAIT
    movwf   indicator_state
    return

process_indicators_blink_right_wait
    BANKSEL steering
    btfss   steering, 7 
    goto    process_indicators_blink_right_wait_centre

    movlw   STEERING_BLINK_THRESHOLD
    subwf   steering_abs, w
    bc      process_indicators_set_not_neutral

process_indicators_blink_right_wait_centre
    BANKSEL steering_abs
    movlw   CENTRE_THRESHOLD
    subwf   steering_abs, w
    bc      process_indicators_set_blink_right

    BANKSEL indicator_state_counter
    movf    indicator_state_counter, f
    skpz    
    return
    goto    process_indicators_set_not_neutral


;******************************************************************************
; Process_channel_reversing
;
; When the user performs 7 clicks on CH3, the left indicator and front 
; head lights light up.
; The user should then turn the steering wheel to left so that the light
; controller knows the direction of the steering channel.
; The user should then also push the throttle in forward direction so that the
; light controller knows the direction of the throttle channel.
;******************************************************************************
Process_channel_reversing
    BANKSEL setup_mode
    btfsc   setup_mode, SETUP_MODE_STEERING_REVERSE
    goto    process_channel_reversing_active
    btfss   setup_mode, SETUP_MODE_THROTTLE_REVERSE
    return

process_channel_reversing_active
    ; Channel reversing setup only has one step so either NEXT or CANCEL
    ; terminate the setup
    btfsc   setup_mode, SETUP_MODE_NEXT
    clrf    setup_mode
    btfsc   setup_mode, SETUP_MODE_CANCEL
    clrf    setup_mode

process_channel_reversing_steering
    btfss   setup_mode, SETUP_MODE_STEERING_REVERSE
    goto    process_channel_reversing_throttle

    ; Save the direction only when the steering is excerted to 50% or more
    BANKSEL steering_abs
    movlw   50
    subwf   steering_abs, w
    skpc    
    goto    process_channel_reversing_throttle

    ; 50% or more steering input: terminate the steering reversing setup and
    ; toggle the reversing flag if the current sign flag on the steering
    ; channel is positive (left = -100..0, right = 0..+100)
    btfss   steering, 7
    comf    steering_reverse, f
    call    EEPROM_save_persistent_data    
    BANKSEL setup_mode
    movlw   ~(1 << SETUP_MODE_STEERING_REVERSE)
    andwf   setup_mode, f

process_channel_reversing_throttle
    btfss   setup_mode, SETUP_MODE_THROTTLE_REVERSE
    return

    ; Save the direction only when the throttle is excerted to 50% or more
    BANKSEL throttle_abs
    movlw   50
    subwf   throttle_abs, w
    skpc    
    return

    ; 50% or more steering input: terminate the throttle reversing setup and
    ; toggle the reversing flag if the current sign flag on the throttle
    ; channel is negative (backward = -100..0, forward = 0..+100)
    btfsc   throttle, 7
    comf    throttle_reverse, f
    call    EEPROM_save_persistent_data    
    BANKSEL setup_mode
    movlw   ~(1 << SETUP_MODE_THROTTLE_REVERSE)
    andwf   setup_mode, f
    return
    

;******************************************************************************
; Process_steering_wheel_servo
;
; This function calculates:
;
;       (right - centre) * abs(steering)
;       -------------------------------- + centre
;                 100
;
; To ease calculation we first do right - centre, then calculate its absolute
; value but store the sign. After multiplication and division using the
; absolute value we re-apply the sign, then add centre.
;******************************************************************************
Process_steering_wheel_servo
    BANKSEL setup_mode
    movf    setup_mode, f
    bz      process_steering_servo_no_setup

    btfsc   setup_mode, SETUP_MODE_CANCEL
    goto    process_steering_servo_setup_cancel
    btfsc   setup_mode, SETUP_MODE_RIGHT
    goto    process_steering_servo_setup_right
    btfsc   setup_mode, SETUP_MODE_LEFT
    goto    process_steering_servo_setup_left
    btfsc   setup_mode, SETUP_MODE_CENTRE
    goto    process_steering_servo_setup_centre
    btfss   setup_mode, SETUP_MODE_INIT
    return

process_steering_servo_setup_init
    movlw   -120
    movwf   servo_epl
    clrf    servo_centre
    movlw   120
    movwf   servo_epr
    movlw   1 << SETUP_MODE_CENTRE
    movwf   setup_mode
    goto    process_steering_servo_no_setup

process_steering_servo_setup_centre
    btfss   setup_mode, SETUP_MODE_NEXT
    goto    process_steering_servo_no_setup

    call    process_steering_servo_no_setup
    BANKSEL servo
    movf    servo, w
    BANKSEL servo_setup_centre
    movwf   servo_setup_centre         
    movlw   1 << SETUP_MODE_LEFT
    movwf   setup_mode
    return

process_steering_servo_setup_left
    BANKSEL setup_mode
    btfss   setup_mode, SETUP_MODE_NEXT
    goto    process_steering_servo_no_setup

    call    process_steering_servo_no_setup
    BANKSEL servo
    movf    servo, w
    BANKSEL servo_setup_epl         
    movwf   servo_setup_epl         
    movlw   1 << SETUP_MODE_RIGHT
    movwf   setup_mode
    return

process_steering_servo_setup_right
    BANKSEL setup_mode
    btfss   setup_mode, SETUP_MODE_NEXT
    goto    process_steering_servo_no_setup

    call    process_steering_servo_no_setup
    BANKSEL servo
    movf    servo, w
    BANKSEL servo_epr
    movwf   servo_epr         
    movf    servo_setup_epl, w         
    movwf   servo_epl         
    movf    servo_setup_centre, w         
    movwf   servo_centre
    clrf    setup_mode
    call    EEPROM_save_persistent_data
    return

process_steering_servo_setup_cancel
    BANKSEL setup_mode
    clrf    setup_mode
    call    EEPROM_load_persistent_data
    return

process_steering_servo_no_setup
    BANKSEL steering_abs
    movf    steering_abs, f
    bnz     process_steering_servo_not_centre
    BANKSEL servo_centre
    movf    servo_centre, w
    BANKSEL servo
    movwf   servo    
    return

process_steering_servo_not_centre
    BANKSEL steering
    movfw   steering
    movwf   temp
    
    BANKSEL servo_epr
    movf    servo_epr, w
    btfsc   temp, 7             ; temp.7: steering sign flag
    movf    servo_epl, w
    movwf   temp

    clrf    servo_ep_sign_flag
    btfsc   temp, 7
    bsf     servo_ep_sign_flag, 7 ; Save the sign flag of the endpoint

    movf    servo_centre, w
    subwf   temp, f
       
    btfsc   temp, 7
    decf    temp, f
    btfsc   temp, 7
    comf    temp, f

    ; temp contains now     abs(left/right - centre)
    movf    temp, w
    movwf   xl
    BANKSEL steering_abs
    movf    steering_abs, w
    call    Mul_xl_by_w     
    movlw   100
    movwf   yl
    clrf    yh
    call    Div_x_by_y

    BANKSEL servo_ep_sign_flag
    btfss   servo_ep_sign_flag, 7
    goto    process_servo_not_negative

    ; Re-apply the sign bit
    movf    xl, w
    clrf    xl
    subwf   xl, f   

process_servo_not_negative
    BANKSEL servo_centre
    movf    servo_centre, w
    addwf   xl, w
    movwf   servo
    return


;******************************************************************************
; EEPROM_load_persistent_data
; 
;******************************************************************************
EEPROM_load_persistent_data
    ; First check if the magic variables are intact. If not, assume the 
    ; EEPROM has not been initialized yet or is corrupted, so write default
    ; values back.
    movlw   EEPROM_ADR_MAGIC1
    call    EEPROM_read_byte
    sublw   EEPROM_MAGIC1
    bnz     EEPROM_load_defaults

    movlw   EEPROM_ADR_MAGIC2
    call    EEPROM_read_byte
    sublw   EEPROM_MAGIC2
    bnz     EEPROM_load_defaults

    movlw   EEPROM_ADR_SERVO_EPL
    call    EEPROM_read_byte
    BANKSEL servo_epl
    movwf   servo_epl

    movlw   EEPROM_ADR_SERVO_CENTRE
    call    EEPROM_read_byte
    BANKSEL servo_centre
    movwf   servo_centre

    movlw   EEPROM_ADR_SERVO_EPR
    call    EEPROM_read_byte
    BANKSEL servo_epr
    movwf   servo_epr

    movlw   EEPROM_ADR_STEERING_REVERSE
    call    EEPROM_read_byte
    BANKSEL steering_reverse
    movwf   steering_reverse

    movlw   EEPROM_ADR_THROTTLE_REVERSE
    call    EEPROM_read_byte
    BANKSEL throttle_reverse
    movwf   throttle_reverse
    return


;******************************************************************************
; EEPROM_save_persistent_data
; 
;******************************************************************************
EEPROM_save_persistent_data
    BANKSEL servo_epl
    movf    servo_epl, w
    movwf   temp
    movlw   EEPROM_ADR_SERVO_EPL
    call    EEPROM_write_byte

    BANKSEL servo_centre
    movf    servo_centre, w
    movwf   temp
    movlw   EEPROM_ADR_SERVO_CENTRE
    call    EEPROM_write_byte

    BANKSEL servo_epr
    movf    servo_epr, w
    movwf   temp
    movlw   EEPROM_ADR_SERVO_EPR
    call    EEPROM_write_byte

    BANKSEL steering_reverse
    movf    steering_reverse, w
    movwf   temp
    movlw   EEPROM_ADR_STEERING_REVERSE
    call    EEPROM_write_byte

    BANKSEL throttle_reverse
    movf    throttle_reverse, w
    movwf   temp
    movlw   EEPROM_ADR_THROTTLE_REVERSE
    call    EEPROM_write_byte
    return


;******************************************************************************
; EEPROM_load_defaults
;
; Load default values of -100..0..100 for the steering servo, write them 
; back to the EEPROM and write the 2 magic variables. 
;******************************************************************************
EEPROM_load_defaults
    BANKSEL servo_epl
    movlw   -100
    movwf   servo_epl
    clrf    servo_centre
    movlw   100
    movwf   servo_epr
    BANKSEL steering_reverse
    clrf    steering_reverse
    BANKSEL throttle_reverse
    clrf    throttle_reverse

    call    EEPROM_save_persistent_data

    movlw   EEPROM_MAGIC1
    movwf   temp
    movlw   EEPROM_ADR_MAGIC1
    call    EEPROM_write_byte

    movlw   EEPROM_MAGIC2
    movwf   temp
    movlw   EEPROM_ADR_MAGIC2
    call    EEPROM_write_byte
    return


;******************************************************************************
; EEPROM_write_byte
;
; Writes the value stored in 'temp' into the address given in W
;******************************************************************************
EEPROM_write_byte
	BANKSEL EEADR
	movwf   EEADR
	movf    temp, w
	movwf   EEDATA		    ; Setup byte to write
	bsf	    EECON1, WREN    ; Enable writes

    bcf     INTCON, GIE	
	movlw   0x55            ; Required sequence!
	movwf   EECON2
    movlw   0xaa
	movwf   EECON2
	bsf     EECON1, WR      ; Begin write procedure
    bsf     INTCON, GIE	

	bcf     EECON1, WREN	; Disable writes 
                            ;  Note: does not affect current write cycle
	
    btfsc   EECON1, WR      ; Wait for the write to complete before we return
	goto    $-1		 
    return


;******************************************************************************
; EEPROM_read_byte
;
; Reads the value stored at address W. The read value is returned in W.
;******************************************************************************
EEPROM_read_byte
	BANKSEL EEADR
	movwf   EEADR
	bsf     EECON1, RD      
	movf    EEDATA, w
	return


    END     ; Directive 'end of program'
