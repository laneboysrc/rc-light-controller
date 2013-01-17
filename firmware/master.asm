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
    EXTERN Init_local_lights
    EXTERN Output_local_lights


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

    EXTERN xl
    EXTERN xh
    EXTERN yl
    EXTERN yh
    EXTERN zl
    EXTERN zh


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
     
    
#define CH3_BUTTON_TIMEOUT 6    ; Time in which we accept double-click of CH3
#define BLINK_COUNTER_VALUE 5   ; 5 * 65.536 ms = ~333 ms = ~1.5 Hz
#define BRAKE_AFTER_REVERSE_COUNTER_VALUE 15 ; 15 * 65.536 ms = ~1 s
#define BRAKE_DISARM_COUNTER_VALUE 15        ; 15 * 65.536 ms = ~1 s
#define INDICATOR_STATE_COUNTER_VALUE 8      ; 8 * 65.536 ms = ~0.5 s
#define INDICATOR_STATE_COUNTER_VALUE_OFF 30 ; ~2 s

; Bitfields in variable ch3_flags
#define CH3_FLAG_LAST_STATE 0
#define CH3_FLAG_TOGGLED 1
#define CH3_FLAG_INITIALIZED 7

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
#define DRIVE_MODE_REVERSE_BRAKE 4
#define DRIVE_MODE_BRAKE_DISARM 5

#define CENTRE_THRESHOLD 10
#define STEERING_BLINK_THRESHOLD 50
#define STEERING_BLINK_OFF_THRESHOLD 30

#define EEPROM_MAGIC1 0x55
#define EEPROM_MAGIC2 0xAA

#define EEPROM_ADR_MAGIC1 0      
#define EEPROM_ADR_MAGIC2 4
#define EEPROM_ADR_SERVO_EPL 1
#define EEPROM_ADR_SERVO_CENTRE 2
#define EEPROM_ADR_SERVO_EPR 3

; Bitfields in variable setup_mode
#define SETUP_MODE_INIT 0
#define SETUP_MODE_CENTRE 1
#define SETUP_MODE_LEFT 2
#define SETUP_MODE_RIGHT 3
#define SETUP_MODE_NEXT 6
#define SETUP_MODE_CANCEL 7

; Bitfields in variable startup_mode
; Note: the higher 4 bits are used so we can simply "or" it with ch3
; and send it to the slave
#define STARTUP_MODE_NEUTRAL 4      ; Waiting before reading ST/TH neutral
#define STARTUP_MODE_REVERSING 5    ; Waiting for Forward/Left to obtain direction

IFNDEF LIGHT_MODE_MASK
#define LIGHT_MODE_MASK b'00001111'
ENDIF


;******************************************************************************
;* VARIABLE DEFINITIONS
;******************************************************************************
.data_master UDATA

drive_mode_counter  res 1
drive_mode_brake_disarm_counter res 1
indicator_state_counter res 1
blink_counter       res 1

ch3_flags           res 1
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
servo_setup_epr     res 1


d0                  res 1 ; Delay and temp registers
d1                  res 1
d2                  res 1
d3                  res 1
temp                res 3 ; Reserve extra bytes labeled temp+1, temp+2 ...

wl                  res 1 ; Temporary parameters for 16 bit math functions
wh                  res 1


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
    CODE

;******************************************************************************
; Initialization
;******************************************************************************
Init
    ;-----------------------------
    ; Initialise the chip (macro included from hw_*.tmp)
    IO_INIT_MASTER

    ; Steering servo related initalization
    movlw   b'00001010'
            ; |||||||+ CCPM0 (Compare mode, generate software interrupt on 
            ; ||||||+- CCPM1  match (CCP1IF bit is set, CCP1 pin is unaffected)
            ; |||||+-- CCPM2 
            ; ||||+--- CCPM3 
            ; |||+---- CCP1Y (not used)
            ; ||+----- CCP1X (not used)
            ; |+------ 
            ; +------- 
    movwf   CCP1CON

    call    Init_local_lights

    call    EEPROM_load_persistent_data

    call    Init_reader

    movlw   BLINK_COUNTER_VALUE
    movwf   blink_counter
    
;   goto    Main_loop    


;**********************************************************************
; Main program
;**********************************************************************
Main_loop
    call    Read_all_channels
    
    call    Process_ch3_double_click
    call    Process_drive_mode
    call    Process_indicators
    call    Process_steering_wheel_servo
    call    Service_timer0

    call    Output_local_lights
    
    IFDEF   ENABLE_SERVO_OUTPUT
    call    Make_servo_pulse
    ENDIF

    goto    Main_loop


;******************************************************************************
    IFDEF ENABLE_SERVO_OUTPUT    
Make_servo_pulse    
    movf    servo, w
    addlw   120
    movwf   xl
    clrf    xh
    call    Mul_x_by_6
    call    Add_x_and_780

    clrf    T1CON           ; Stop timer 1, runs at 1us per tick, internal osc
    clrf    TMR1H           ; Reset the timer to 0
    clrf    TMR1L
    movf    xl, w           ; Load Timer1 compare register with the servo time
    movwf   CCPR1L
    movf    xh, w
    movwf   CCPR1H
    bcf     PIR1, CCP1IF    ; Clear Timer1 compare interrupt flag
  
    bsf     T1CON, 0        ; Start timer 1
    bsf     PORT_SERVO      ; Set servo port to high pulse

    btfss   PIR1, CCP1IF    ; Wait for compare value reached
    goto    $ - 1

    bcf     PORT_SERVO      ; Turn off servo pulse
    clrf    T1CON           ; Stop timer 1
    bcf     PIR1, CCP1IF

    return
    ENDIF    

    
;******************************************************************************
; Service_timer0
;
; Soft-timer with a resolution of 65.536 ms
;******************************************************************************
Service_timer0
    btfss   INTCON, T0IF
    return

    bcf     INTCON, T0IF

    movf    ch3_click_counter, f
    skpz     
    decf    ch3_click_counter, f    

    movf    indicator_state_counter, f
    skpz     
    decf    indicator_state_counter, f    


    decfsz  drive_mode_brake_disarm_counter, f
    goto    service_timer0_drive_mode

    btfss   drive_mode, DRIVE_MODE_BRAKE_DISARM
    goto    service_timer0_drive_mode

    bcf     drive_mode, DRIVE_MODE_BRAKE_DISARM
    bcf     drive_mode, DRIVE_MODE_BRAKE_ARMED


service_timer0_drive_mode
    decfsz  drive_mode_counter, f
    goto    service_timer0_blink

    btfss   drive_mode, DRIVE_MODE_REVERSE_BRAKE
    goto    service_timer0_blink

    bcf     drive_mode, DRIVE_MODE_REVERSE_BRAKE
    bcf     drive_mode, DRIVE_MODE_BRAKE


service_timer0_blink
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
    movf    startup_mode, f
    bz      process_ch3_no_startup
    return

process_ch3_no_startup
    btfsc   ch3_flags, CH3_FLAG_INITIALIZED
    goto    process_ch3_initialized

    ; Ignore the potential "toggle" after power on
    bsf     ch3_flags, CH3_FLAG_INITIALIZED
    bcf     ch3_flags, CH3_FLAG_LAST_STATE
    btfsc   ch3, CH3_FLAG_LAST_STATE
    bsf     ch3_flags, CH3_FLAG_LAST_STATE
    return

process_ch3_initialized
    movfw   ch3
    xorwf   ch3_flags, w
    movwf   temp
    btfss   temp, CH3_FLAG_LAST_STATE
    goto    process_ch3_click_timeout

    bcf     ch3_flags, CH3_FLAG_LAST_STATE
    btfsc   ch3, CH3_FLAG_LAST_STATE
    bsf     ch3_flags, CH3_FLAG_LAST_STATE
    incf    ch3_clicks, f
    movlw   CH3_BUTTON_TIMEOUT
    movwf   ch3_click_counter

    IFDEF   DEBUG
    movlw   0x43                    ; send 'C'
    call    UART_send_w        
    ENDIF
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
    IFDEF   DEBUG
    movlw   0x50                    ; send 'P'
    call    UART_send_w        
    ENDIF

    decfsz  ch3_clicks, f                
    goto    process_ch3_double_click

    ; --------------------------
    ; Single click: switch light mode up (Parking, Low Beam, Fog, High Beam) 
    rlf     light_mode, f
    bsf     light_mode, 0
    movlw   LIGHT_MODE_MASK
    andwf   light_mode, f
    IFDEF   DEBUG
    movlw   0x31                    ; send '1'
    call    UART_send_w        
    ENDIF
    return

process_ch3_double_click
    decfsz  ch3_clicks, f              
    goto    process_ch3_triple_click

    ; --------------------------
    ; Double click: switch light mode down (Parking, Low Beam, Fog, High Beam)  
    rrf     light_mode, f
    movlw   LIGHT_MODE_MASK
    andwf   light_mode, f
    IFDEF   DEBUG
    movlw   0x32                    ; send '2'
    call    UART_send_w        
    ENDIF
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
    IFDEF   DEBUG
    movlw   0x33                    ; send '3'
    call    UART_send_w        
    ENDIF
    return

process_ch3_quad_click
    decfsz  ch3_clicks, f              
    goto    process_ch3_8_click

    ; --------------------------
    ; Quad click: Hazard lights on/off  
    clrf    ch3_clicks
    call    Synchronize_blinking
    movlw   1 << BLINK_MODE_HAZARD
    xorwf   blink_mode, f
    IFDEF   DEBUG
    movlw   0x34                    ; send '4'
    call    UART_send_w        
    ENDIF
    return

process_ch3_8_click
    movlw   4
    subwf   ch3_clicks, w
    bnz     process_ch3_click_end

    movlw   1 << SETUP_MODE_INIT
    movwf   setup_mode    
    IFDEF   DEBUG
    movlw   0x38                    ; send '8'
    call    UART_send_w        
    ENDIF

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

; Bitfields in variable drive_mode
;#define DRIVE_MODE_FORWARD 0 
;#define DRIVE_MODE_BRAKE 1 
;#define DRIVE_MODE_REVERSE 2
;#define DRIVE_MODE_BRAKE_ARMED 3
;******************************************************************************
Process_drive_mode
    movlw   CENTRE_THRESHOLD
    subwf   throttle_abs, w
    bc      process_drive_mode_not_neutral

    btfsc   drive_mode, DRIVE_MODE_REVERSE_BRAKE
    return
    btfsc   drive_mode, DRIVE_MODE_BRAKE_DISARM
    return

    bcf     drive_mode, DRIVE_MODE_FORWARD
    btfss   drive_mode, DRIVE_MODE_REVERSE
    goto    process_drive_mode_not_neutral_after_reverse

    bcf     drive_mode, DRIVE_MODE_REVERSE
    bsf     drive_mode, DRIVE_MODE_REVERSE_BRAKE
    bsf     drive_mode, DRIVE_MODE_BRAKE
    movlw   BRAKE_AFTER_REVERSE_COUNTER_VALUE
    movwf   drive_mode_counter   
    return

process_drive_mode_not_neutral_after_reverse
    bsf     drive_mode, DRIVE_MODE_BRAKE_DISARM
    movlw   BRAKE_DISARM_COUNTER_VALUE
    movwf   drive_mode_brake_disarm_counter   

    btfsc   drive_mode, DRIVE_MODE_BRAKE
    bcf     drive_mode, DRIVE_MODE_BRAKE_ARMED
    bcf     drive_mode, DRIVE_MODE_BRAKE
    return

process_drive_mode_not_neutral
    bcf     drive_mode, DRIVE_MODE_REVERSE_BRAKE
    bcf     drive_mode, DRIVE_MODE_BRAKE_DISARM

    btfsc   throttle, 7
    goto    process_drive_mode_brake_or_reverse

    bsf     drive_mode, DRIVE_MODE_FORWARD
    bsf     drive_mode, DRIVE_MODE_BRAKE_ARMED
    bcf     drive_mode, DRIVE_MODE_REVERSE
    bcf     drive_mode, DRIVE_MODE_BRAKE
    return

process_drive_mode_brake_or_reverse
    btfsc   drive_mode, DRIVE_MODE_BRAKE_ARMED
    goto    process_drive_mode_brake

    bsf     drive_mode, DRIVE_MODE_REVERSE
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
    IF 0
    movf    indicator_state, w
    addwf   PCL, f

Process_indicators_table
    goto    process_indicators_not_neutral
    goto    process_indicators_neutral_wait
    goto    process_indicators_blink_armed
    goto    process_indicators_blink_armed_left
    goto    process_indicators_blink_armed_right
    goto    process_indicators_blink_left
    goto    process_indicators_blink_left_wait
    goto    process_indicators_blink_right
    goto    process_indicators_blink_right_wait
    IF ((HIGH ($)) != (HIGH (Process_indicators_table)))
        ERROR "Process_indicators_table CROSSES PAGE BOUNDARY!"
    ENDIF
    ENDIF

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
    subwf   throttle_abs, w
    skpnc
    return

    movlw   CENTRE_THRESHOLD
    subwf   steering_abs, w
    skpnc
    return

    movlw   INDICATOR_STATE_COUNTER_VALUE
    movwf   indicator_state_counter
    movlw   STATE_INDICATOR_NEUTRAL_WAIT
    movwf   indicator_state
    return

process_indicators_neutral_wait
    movlw   CENTRE_THRESHOLD
    subwf   throttle_abs, w
    bc      process_indicators_set_not_neutral

    movlw   CENTRE_THRESHOLD
    subwf   steering_abs, w
    bc      process_indicators_set_not_neutral

    movf    indicator_state_counter, f
    skpz    
    return

process_indicators_set_blink_armed
    movlw   STATE_INDICATOR_BLINK_ARMED
    movwf   indicator_state
    return

process_indicators_set_not_neutral
    movlw   STATE_INDICATOR_NOT_NEUTRAL
    movwf   indicator_state
    bcf     blink_mode, BLINK_MODE_INDICATOR_RIGHT
    bcf     blink_mode, BLINK_MODE_INDICATOR_LEFT
    return

process_indicators_blink_armed
    movlw   CENTRE_THRESHOLD
    subwf   throttle_abs, w
    bc      process_indicators_set_not_neutral

    movlw   STEERING_BLINK_THRESHOLD
    subwf   steering_abs, w
    skpc
    return    

    movlw   INDICATOR_STATE_COUNTER_VALUE
    movwf   indicator_state_counter    
    movlw   STATE_INDICATOR_BLINK_ARMED_LEFT
    btfss   steering, 7 
    movlw   STATE_INDICATOR_BLINK_ARMED_RIGHT
    movwf   indicator_state
    return
  
process_indicators_blink_armed_left  
    movlw   CENTRE_THRESHOLD
    subwf   throttle_abs, w
    bc      process_indicators_set_not_neutral

    movlw   STEERING_BLINK_THRESHOLD
    subwf   steering_abs, w
    bnc     process_indicators_set_blink_armed

    btfss   steering, 7 
    goto    process_indicators_set_blink_armed

    movf    indicator_state_counter, f
    skpz    
    return

process_indicators_set_blink_left  
    movlw   STATE_INDICATOR_BLINK_LEFT
    movwf   indicator_state
    call    Synchronize_blinking
    bsf     blink_mode, BLINK_MODE_INDICATOR_LEFT
    return

process_indicators_blink_armed_right  
    movlw   CENTRE_THRESHOLD
    subwf   throttle_abs, w
    bc      process_indicators_set_not_neutral

    movlw   STEERING_BLINK_THRESHOLD
    subwf   steering_abs, w
    bnc     process_indicators_set_blink_armed

    btfsc   steering, 7 
    goto    process_indicators_set_blink_armed

    movf    indicator_state_counter, f
    skpz    
    return

process_indicators_set_blink_right
    movlw   STATE_INDICATOR_BLINK_RIGHT
    movwf   indicator_state
    call    Synchronize_blinking
    bsf     blink_mode, BLINK_MODE_INDICATOR_RIGHT
    return

process_indicators_blink_left
    btfsc   steering, 7 
    goto    process_indicators_blink_left_centre

    movlw   STEERING_BLINK_THRESHOLD
    subwf   steering_abs, w
    bc      process_indicators_set_not_neutral

process_indicators_blink_left_centre
    movlw   CENTRE_THRESHOLD
    subwf   steering_abs, w
    skpnc
    return

    movlw   INDICATOR_STATE_COUNTER_VALUE_OFF
    movwf   indicator_state_counter             
    movlw   STATE_INDICATOR_BLINK_LEFT_WAIT
    movwf   indicator_state
    return

process_indicators_blink_left_wait
    btfsc   steering, 7 
    goto    process_indicators_blink_left_wait_centre

    movlw   STEERING_BLINK_THRESHOLD
    subwf   steering_abs, w
    bc      process_indicators_set_not_neutral

process_indicators_blink_left_wait_centre
    movlw   CENTRE_THRESHOLD
    subwf   steering_abs, w
    bc      process_indicators_set_blink_left

    movf    indicator_state_counter, f
    skpz    
    return
    goto    process_indicators_set_not_neutral

process_indicators_blink_right
    btfss   steering, 7 
    goto    process_indicators_blink_right_centre

    movlw   STEERING_BLINK_THRESHOLD
    subwf   steering_abs, w
    bnc     process_indicators_set_not_neutral

process_indicators_blink_right_centre
    movlw   CENTRE_THRESHOLD
    subwf   steering_abs, w
    skpnc
    return

    movlw   INDICATOR_STATE_COUNTER_VALUE_OFF
    movwf   indicator_state_counter             
    movlw   STATE_INDICATOR_BLINK_RIGHT_WAIT
    movwf   indicator_state
    return

process_indicators_blink_right_wait
    btfss   steering, 7 
    goto    process_indicators_blink_right_wait_centre

    movlw   STEERING_BLINK_THRESHOLD
    subwf   steering_abs, w
    bc      process_indicators_set_not_neutral

process_indicators_blink_right_wait_centre
    movlw   CENTRE_THRESHOLD
    subwf   steering_abs, w
    bc      process_indicators_set_blink_right

    movf    indicator_state_counter, f
    skpz    
    return
    goto    process_indicators_set_not_neutral


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
#define SIGN_FLAG wl

Process_steering_wheel_servo
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
    movf    servo, w
    movwf   servo_setup_centre         
    movlw   1 << SETUP_MODE_LEFT
    movwf   setup_mode
    return

process_steering_servo_setup_left
    btfss   setup_mode, SETUP_MODE_NEXT
    goto    process_steering_servo_no_setup

    call    process_steering_servo_no_setup
    movf    servo, w
    movwf   servo_setup_epl         
    movlw   1 << SETUP_MODE_RIGHT
    movwf   setup_mode
    return

process_steering_servo_setup_right
    btfss   setup_mode, SETUP_MODE_NEXT
    goto    process_steering_servo_no_setup

    call    process_steering_servo_no_setup
    movf    servo, w
    movwf   servo_epr         
    movf    servo_setup_epl, w         
    movwf   servo_epl         
    movf    servo_setup_centre, w         
    movwf   servo_centre
    call    EEPROM_save_persistent_data
    clrf    setup_mode
    return

process_steering_servo_setup_cancel
    clrf    setup_mode
    call    EEPROM_load_persistent_data
    return

process_steering_servo_no_setup
    movf    steering_abs, f
    bnz     process_steering_servo_not_centre
    movf    servo_centre, w
    movwf   servo    
    return

process_steering_servo_not_centre
    movf    servo_epr, w
    btfsc   steering, 7
    movf    servo_epl, w
    movwf   temp

    movf    servo_centre, w
    subwf   temp, f

    clrf    SIGN_FLAG
    btfsc   temp, 7
    incf    SIGN_FLAG, f
        
    btfsc   temp, 7
    decf    temp, f
    btfsc   temp, 7
    comf    temp, f

    ; temp contains now     abs(right - centre)
    movf    temp, w
    movwf   xl
    movf    steering_abs, w
    call    Mul_xl_by_w
    movlw   100
    movwf   yl
    clrf    yh
    call    Div_x_by_y

    movf    SIGN_FLAG, f
    bz      process_servo_not_negative

    ; Re-apply the sign bit
    movf    xl, w
    clrf    xl
    subwf   xl, f   

process_servo_not_negative
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
    movwf   servo_epl

    movlw   EEPROM_ADR_SERVO_CENTRE
    call    EEPROM_read_byte
    movwf   servo_centre

    movlw   EEPROM_ADR_SERVO_EPR
    call    EEPROM_read_byte
    movwf   servo_epr
    return


;******************************************************************************
; Servo_store_values
; 
;******************************************************************************
EEPROM_save_persistent_data
    movf    servo_epl, w
    movwf   temp
    movlw   EEPROM_ADR_SERVO_EPL
    call    EEPROM_write_byte

    movf    servo_centre, w
    movwf   temp
    movlw   EEPROM_ADR_SERVO_CENTRE
    call    EEPROM_write_byte

    movf    servo_epr, w
    movwf   temp
    movlw   EEPROM_ADR_SERVO_EPR
    call    EEPROM_write_byte
    return


;******************************************************************************
; Servo_load_defaults
;
; Load default values of -100..0..100 for the steering servo, write them 
; back to the EEPROM and write the 2 magic variables. 
;******************************************************************************
EEPROM_load_defaults
    movlw   -100
    movwf   servo_epl
    clrf    servo_centre
    movlw   100
    movwf   servo_epr

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
	BANKSEL temp
	movf    temp, w
	BANKSEL EEDATA
	movwf   EEDATA		    ; Setup byte to write
	bsf	    EECON1, WREN    ; Enable writes
	
	movlw   H'55'           ; Required sequence!
	movwf   EECON2
    movlw   H'AA'
	movwf   EECON2
	bsf     EECON1, WR      ; Begin write procedure
	bcf     EECON1, WREN	; Disable writes 
                            ;  Note: does not affect current write cycle
	
	; Wait for the write to complete before we return
	BANKSEL PIR1
    btfss   PIR1, EEIF
	goto    $-1		 
	bcf     PIR1, EEIF      ; Clear EEPROM Write Operation IRQ flag
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
	BANKSEL PIR1
	return


    END     ; Directive 'end of program'



