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
    GLOBAL throttle            
    GLOBAL throttle_abs       
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
    
    EXTERN xl
    EXTERN xh
    EXTERN yl
    EXTERN yh
    EXTERN zl
    EXTERN zh

    EXTERN startup_mode


#define SLAVE_MAGIC_BYTE    0x87


;******************************************************************************
;* VARIABLE DEFINITIONS
;******************************************************************************
.data_uart_reader UDATA

throttle            res 1
throttle_abs        res 1    

steering            res 1
steering_abs        res 1

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
    call    read_UART_byte
    sublw   SLAVE_MAGIC_BYTE        ; First byte the magic byte?
    bnz     Read_UART               ; No: wait for 0x8f to appear

read_UART_byte_2
    call    read_UART_byte
    movwf   steering                ; Store 2nd byte
    sublw   SLAVE_MAGIC_BYTE        ; Is it the magic byte?
    bz      read_UART_byte_2        ; Yes: we must be out of sync...

read_UART_byte_3
    call    read_UART_byte
    movwf   throttle
    sublw   SLAVE_MAGIC_BYTE
    bz      read_UART_byte_2

read_UART_byte_4
    call    read_UART_byte
    movwf   ch3                     ; The 3rd byte contains information for
    movwf   startup_mode            ;  CH3 as well as startup_mode
    sublw   SLAVE_MAGIC_BYTE
    bz      read_UART_byte_2
    
    movlw   0x01                    ; Remove startup_mode bits from CH3
    andwf   ch3, f   
    movlw   0xf0                    ; Remove CH3 bits from startup_mode
    andwf   startup_mode, f   

    ; Calculate abs(throttle) and abs(steering) for easier math.
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
; read_UART_byte
;
; Recieve one byte from the UART in W.
;
; To enable reception of a byte, CREN must be 1. 
;
; On any error, recover by pulsing CREN low then back to high. 
;
; When a byte has been received the RCIF flag will be set. RCIF is 
; automatically cleared when RCREG is read and empty. RCREG is double buffered, 
; so it is a two byte deep FIFO. If a third byte comes in, then OERR is set. 
; You can still recover the two bytes in the FIFO, but the third (newest) is 
; lost. CREN must be pulsed negative to clear the OERR flag. 
;
; On a framing error FERR is set. FERR is automatically reset when RCREG is 
; read, so errors must be tested for *before* RCREG is read. It is *NOT* 
; recommended that you ignore the error flags. Eventually an error will cause 
; the receiver to hang up if you don't clear the error condition.
;******************************************************************************
read_UART_byte
	btfsc   RCSTA, OERR
	goto    overerror       ; if overflow error...
	btfsc   RCSTA, FERR
	goto	frameerror      ; if framing error...
uart_ready
	btfss	PIR1, RCIF
	goto	read_UART_byte  ; if not ready, wait...	

uart_gotit
	bcf     INTCON, GIE     ; Turn GIE off. This is IMPORTANT!
	btfsc	INTCON, GIE     ; MicroChip recommends this check!
	goto 	uart_gotit      ; !!! GOTCHA !!! without this check
                            ;   you are not sure gie is cleared!
	movf	RCREG, w        ; Read UART data
	bsf     INTCON, GIE     ; Re-enable interrupts
	return

overerror	   		
    ; Over-run errors are usually caused by the incoming data building up in 
    ; the fifo. This is often the case when the program has not read the UART
    ; in a while. Flushing the FIFO will allow normal input to resume.
    ; Note that flushing the FIFO also automatically clears the FERR flag.
    ; Pulsing CREN resets the OERR flag.

	bcf     INTCON, GIE
	btfsc	INTCON, GIE
	goto 	overerror

	bcf     RCSTA, CREN     ; Pulse CREN off...
	movf	RCREG, w        ; Flush the FIFO, all 3 elements
	movf	RCREG, w		
	movf	RCREG, w
	bsf     RCSTA, CREN     ; Turn CREN back on. This pulsing clears OERR
	bsf     INTCON, GIE
	goto	read_UART_byte  ; Try again...

frameerror			
    ; Framing errors are usually due to wrong baud rate coming in.

	bcf     INTCON, GIE
	btfsc	INTCON, GIE
	goto 	frameerror

	movf	RCREG,w		;reading rcreg clears ferr flag.
	bsf     INTCON, GIE
	goto	read_UART_byte  ; Try again...


    END     ; Directive 'end of program'



