;******************************************************************************
;
;   dummy-reader.asm
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************
    TITLE       RC Light Controller - Dummy reader, simulates RC servo signals
    LIST        r=dec
    RADIX       dec

    #include    hw.tmp


    GLOBAL Read_all_channels
    GLOBAL Init_reader

    GLOBAL steering            
    GLOBAL steering_abs       
    GLOBAL steering_reverse
    GLOBAL throttle            
    GLOBAL throttle_abs       
    GLOBAL throttle_threshold       
    GLOBAL throttle_reverse
    GLOBAL ch3  

    ; Functions imported from master.asm
    EXTERN ch3_clicks
    EXTERN blink_mode
    EXTERN drive_mode

    ; Functions imported from utils.asm
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
    EXTERN UART_read_byte
    
    EXTERN xl
    EXTERN xh
    EXTERN yl
    EXTERN yh
    EXTERN zl
    EXTERN zh

    EXTERN startup_mode



; Bitfields in variable blink_mode
#define BLINK_MODE_BLINKFLAG 0          ; Toggles with 1.5 Hz
#define BLINK_MODE_HAZARD 1             ; Hazard lights active
#define BLINK_MODE_INDICATOR_LEFT 2     ; Left indicator active
#define BLINK_MODE_INDICATOR_RIGHT 3    ; Right indicator active
#define BLINK_MODE_SOFTTIMER 7          ; Is 1 for one mainloop when the soft 
                                        ; timer triggers (every 65.536ms)

; Bitfields in variable drive_mode
#define DRIVE_MODE_FORWARD 0 
#define DRIVE_MODE_BRAKE 1 
#define DRIVE_MODE_REVERSE 2
#define DRIVE_MODE_BRAKE_ARMED 3
#define DRIVE_MODE_AUTO_BRAKE 4
#define DRIVE_MODE_BRAKE_DISARM 5
#define DRIVE_MODE_AUTO_REVERSE 6

;******************************************************************************
;* VARIABLE DEFINITIONS
;******************************************************************************
.data_dummy_reader UDATA

throttle            res 1
throttle_abs        res 1  
throttle_threshold  res 1       ; not used here, but defined so it is in the same bank as throttle_abs 
throttle_reverse    res 1  

steering            res 1
steering_abs        res 1
steering_reverse    res 1

ch3                 res 1

debug_counter   res 1
debug_mode      res 1

d1              res 1
d2              res 1


;******************************************************************************
; Relocatable code section
;******************************************************************************
.code_dummy_reader CODE

;******************************************************************************
; Initialization
;******************************************************************************
Init_reader
    BANKSEL debug_counter
    clrf    debug_counter
    incf    debug_counter, f
    clrf    debug_mode    
    return


;******************************************************************************
; Read_all_channels
;******************************************************************************
Read_all_channels
    call    Simulate_servo_signals

    ; Calculate abs(throttle) and abs(steering) for easier math.
    BANKSEL throttle
    movfw   throttle
    movwf   throttle_abs
    btfsc   throttle_abs, 7
    decf    throttle_abs, f
    btfsc   throttle_abs, 7
    comf    throttle_abs, f

    movfw   steering
    movwf   steering_abs
    btfsc   steering_abs, 7
    decf    steering_abs, f
    btfsc   steering_abs, 7
    comf    steering_abs, f
    
;    return

;******************************************************************************
Delay15ms
	movlw	0xBE
	movwf	d1
	movlw	0x5E
	movwf	d2
Delay15ms_0
	decfsz	d1, f
	goto	$+2
	decfsz	d2, f
	goto	Delay15ms_0
	goto	$+1
	return
	

;******************************************************************************
; Simulate_servo_signals
;******************************************************************************
Simulate_servo_signals
    call    Delay15ms
    BANKSEL blink_mode
    bsf     blink_mode, BLINK_MODE_HAZARD
    return





    BANKSEL blink_mode
    btfss   blink_mode, BLINK_MODE_SOFTTIMER
    return

    ;---------------------------
    BANKSEL debug_counter
    movf    debug_counter, f
    skpz     
    decfsz  debug_counter, f    
    return
    
    incf    debug_mode, f
    movfw   debug_mode

    ;-----
    decfsz  WREG, f
    goto    debug_not_0

    movlw   1000 / 65
    movwf   debug_counter
    
    BANKSEL steering
    movlw   0
    movwf   steering
    movlw   0
    movwf   throttle
    movlw   0
    movwf   ch3                     ; Only bit 0 is active
    BANKSEL startup_mode
    movlw   0x10                    ; Only bits 7..4 are active
    movwf   startup_mode
    return

    ;-----
debug_not_0
    decfsz  WREG, f
    goto    debug_not_1

    movlw   500 / 65
    movwf   debug_counter

    BANKSEL startup_mode
    movlw   0x00                    ; Only bits 7..4 are active
    movwf   startup_mode
    return

    ;-----
debug_not_1
    decfsz  WREG, f
    goto    debug_not_2

    movlw   100 / 65
    movwf   debug_counter

    BANKSEL ch3_clicks
    movlw   3
    movwf   ch3_clicks   
    return

    ;-----
debug_not_2
    decfsz  WREG, f
    goto    debug_not_3

    movlw   4000 / 65
    movwf   debug_counter
    
    BANKSEL ch3_clicks
    movlw   6
    movwf   ch3_clicks   
    return

    ;-----
debug_not_3
    return
    
    decfsz  WREG, f
    goto    debug_not_4

    movlw   2000 / 65
    movwf   debug_counter

    BANKSEL ch3_clicks
    movlw   2
    movwf   ch3_clicks   

;    movlw   0x01
;    xorwf   ch3, f

    return

    ;-----
debug_not_4
    return


    decfsz  WREG, f
    goto    debug_not_5

    movlw   2000 / 65
    movwf   debug_counter

    BANKSEL ch3_clicks
    movlw   1
    movwf   ch3_clicks   
    return

    ;-----
debug_not_5
    return

    decfsz  WREG, f
    return

    BANKSEL ch3_clicks
    movlw   4
    movwf   ch3_clicks   
    return

    END     ; Directive 'end of program'



