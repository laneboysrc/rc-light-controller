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
    GLOBAL throttle_reverse
    GLOBAL ch3  

    ; Functions imported from master.asm
    EXTERN ch3_clicks
    EXTERN blink_mode

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


;******************************************************************************
;* VARIABLE DEFINITIONS
;******************************************************************************
.data_dummy_reader UDATA

throttle            res 1
throttle_abs        res 1  
throttle_reverse    res 1  

steering            res 1
steering_abs        res 1
steering_reverse    res 1

ch3                 res 1

debug_counter   res 1
debug_mode      res 1


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
    call    Reader_handle_softtimer

    BANKSEL steering
    movlw   0
    movwf   steering
    movlw   0
    movwf   throttle
    movlw   0
    movwf   ch3     
    BANKSEL startup_mode
    movwf   startup_mode

debug_read_done    
    BANKSEL ch3
    movfw   ch3
    movwf   startup_mode
    movlw   0x01                    ; Remove startup_mode bits from CH3
    andwf   ch3, f   
    BANKSEL startup_mode
    movlw   0xf0                    ; Remove CH3 bits from startup_mode
    andwf   startup_mode, f   

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
    return


;******************************************************************************
; Reader_handle_softtimer
; Softtimer related function to simulate activity
;******************************************************************************
Reader_handle_softtimer
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
    return

    ;-----
debug_not_0
    decfsz  WREG, f
    goto    debug_not_1

    movlw   1000 / 65
    movwf   debug_counter

    BANKSEL ch3_clicks
    movlw   4
    movwf   ch3_clicks   
    return

    ;-----
debug_not_1
    decfsz  WREG, f
    goto    debug_not_2

    movlw   5000 / 65
    movwf   debug_counter
    
    BANKSEL ch3_clicks
    movlw   2
    movwf   ch3_clicks   
    return

    ;-----
debug_not_2
    decfsz  WREG, f
    return

    BANKSEL ch3_clicks
    movlw   1
    movwf   ch3_clicks   
    return

    END     ; Directive 'end of program'



