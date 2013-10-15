;******************************************************************************
;
;   uart-reader.asm
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************
    TITLE       RC Light Controller - UART reader
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


#define SLAVE_MAGIC_BYTE    0x87


IFNDEF ENABLE_UART
    ERROR "########################"
    ERROR "To use UART input from preprocessor you must add '-D ENABLE_UART' to the CFLAGS in the makefile!"
    ERROR "########################"
ENDIF

;******************************************************************************
;* VARIABLE DEFINITIONS
;******************************************************************************
.data_uart_reader UDATA

throttle            res 1
throttle_abs        res 1  
throttle_reverse    res 1  

steering            res 1
steering_abs        res 1
steering_reverse    res 1

ch3                 res 1


;******************************************************************************
; Relocatable code section
;******************************************************************************
.code_uart_reader CODE

;******************************************************************************
; Initialization
;******************************************************************************
Init_reader
    return


;******************************************************************************
; Read_all_channels
;
; This function returns after having successfully received a complete
; protocol frame via the UART.
;******************************************************************************
Read_all_channels
Read_UART
    call    UART_read_byte
    sublw   SLAVE_MAGIC_BYTE        ; First byte the magic byte?
    bnz     Read_UART               ; No: wait for 0x8f to appear

read_UART_byte_2
    call    UART_read_byte
    BANKSEL steering
    movwf   steering                ; Store 2nd byte
    sublw   SLAVE_MAGIC_BYTE        ; Is it the magic byte?
    bz      read_UART_byte_2        ; Yes: we must be out of sync...

read_UART_byte_3
    call    UART_read_byte
    BANKSEL throttle
    movwf   throttle
    sublw   SLAVE_MAGIC_BYTE
    bz      read_UART_byte_2

read_UART_byte_4
    call    UART_read_byte
    BANKSEL ch3
    movwf   ch3                     ; The 3rd byte contains information for
    BANKSEL startup_mode
    movwf   startup_mode            ;  CH3 as well as startup_mode
    sublw   SLAVE_MAGIC_BYTE
    bz      read_UART_byte_2
    
    BANKSEL ch3
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

    ; The system allows user-configurable reversing of the steering channel
    ; through seven CH3 clicks. If the reversing flag is set we negate
    ; the steering signal coming from the preprocessor.
    movf    steering_reverse, f
    skpnz
    goto    read_UART_throttle_reversing
    comf    steering, f
    incf    steering, f

    ; The system allows user-configurable reversing of the throttle channel
    ; through seven CH3 clicks. If the reversing flag is set we negate
    ; the throttle signal coming from the preprocessor.
read_UART_throttle_reversing
    movf    throttle_reverse, f
    skpnz
    return
    comf    throttle, f
    incf    throttle, f

    return


    END     ; Directive 'end of program'



