;******************************************************************************
;
;   steering_wheel_servo.asm
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************
    TITLE       RC Light Controller - Steering wheel servo functions
    LIST        r=dec
    RADIX       dec

    #include    hw.tmp

    GLOBAL Init_steering_wheel_servo
    GLOBAL Make_steering_wheel_servo_pulse 
    
    
    ; Functions and variables imported from utils.asm
    EXTERN Mul_x_by_6
    EXTERN Add_x_and_780    

    EXTERN xl
    EXTERN xh
    EXTERN yl
    EXTERN yh
    EXTERN zl
    EXTERN zh


    ; Functions and variables imported from master.asm or slave.asm
    EXTERN servo


IFNDEF ENABLE_SERVO_OUTPUT
    ERROR "########################"
    ERROR "To use the steering wheel servo you must add '-D ENABLE_SERVO_OUTPUT' to the CFLAGS in the makefile!"
    ERROR "########################"
ENDIF

;******************************************************************************
; Relocatable code section
;******************************************************************************
.code_steering_wheel_servo CODE

;******************************************************************************
Init_steering_wheel_servo
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
    return

    
;******************************************************************************
Make_steering_wheel_servo_pulse    
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

    END     ; Directive 'end of program'
