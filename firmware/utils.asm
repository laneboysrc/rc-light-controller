;******************************************************************************
;
;   uils.asm
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************
    TITLE       Utility functions
    LIST        r=dec
    RADIX       dec

    #include    hw.tmp


    GLOBAL xl
    GLOBAL xh
    GLOBAL yl
    GLOBAL yh
    GLOBAL zl
    GLOBAL zh
    GLOBAL temp

    GLOBAL light_data

    
;******************************************************************************
; Relocatable variables section
;******************************************************************************
.data_all_banks UDATA_SHR

xl                  res 1
xh                  res 1
yl                  res 1
yh                  res 1
zl                  res 1
zh                  res 1
temp                res 2


.data_utils UDATA

IFDEF TLC5940
light_data          res 16      ; TLC5940
ELSE                            
IFDEF DUAL_TLC5916
light_data          res 2       ; DUAL_TLC5917
ELSE  
light_data          res 1       ; Single TLC5917
ENDIF 
ENDIF 
 

;******************************************************************************
; Relocatable code section
;******************************************************************************
.utils CODE


;******************************************************************************
; Max
;  
; Given two 8-bit values in temp and w, returns the larger one in w
;******************************************************************************
.utils_Max CODE
    GLOBAL Max
Max
    subwf   temp, w
    skpc
    subwf   temp, f
    movf    temp, w
    return    


;******************************************************************************
; Min
;  
; Given two 8-bit values in temp and w, returns the smaller one in w
;******************************************************************************
.utils_Min CODE
    GLOBAL Min
Min
    subwf   temp, w
    skpnc
    subwf   temp, f
    movf    temp, w
    return    


;******************************************************************************
; Min_x_z
;  
; Given two 16-bit values in xl/xh and zl/zh, returns the smaller one in zl/zh.
;******************************************************************************
.utils_Min_x_z CODE
    GLOBAL Min_x_z
Min_x_z
    movf    xl, w
    subwf	zl, w	
    movf	xh, w
    skpc
    addlw   1
    subwf	zh, w
    andlw	b'10000000'	
    skpz
    return

	movf	xl, w
	movwf	zl
	movf	xh, w
	movwf	zh
    return


;******************************************************************************
; Max_x_z
;  
; Given two 16-bit values in xl/xh and zl/zh, returns the larger one in zl/zh.
;******************************************************************************
.utils_Max_x_z CODE
    GLOBAL Max_x_z
Max_x_z
    movf    xl, w
    subwf   zl, w		
    movf    xh, w
    skpc
    addlw   1
    subwf   zh, w		
    andlw   b'10000000'  
    skpnz
    return

	movf	xl, w
	movwf	zl
	movf	xh, w
	movwf	zh
    return


;******************************************************************************
; Div_x_by_y
;
; xh/xl = xh/xl / yh/yl; Remainder in zh/zl
;
; Based on "32 by 16 Divison" by Nikolai Golovchenko
; http://www.piclist.com/techref/microchip/math/div/div16or32by16to16.htm
;******************************************************************************
.utils_Div_x_by_y CODE
    GLOBAL Div_x_by_y
Div_x_by_y
    clrf    zl      ; Clear remainder
    clrf    zh
    clrf    temp    ; Clear remainder extension
    movlw   16
    movwf   temp+1
    setc            ; First iteration will be subtraction

div16by16loop
    ; Shift in next result bit and shift out next dividend bit to remainder
    rlf     xl, f   ; Shift LSB
    rlf     xh, f   ; Shift MSB
    rlf     zl, f
    rlf     zh, f
    rlf     temp, f

    movf    yl, w
    btfss   xl, 0
    goto    div16by16add

    ; Subtract divisor from remainder
    subwf   zl, f
    movf    yh, w
    skpc
    incfsz  yh, w
    subwf   zh, f
    movlw   1
    skpc
    subwf   temp, f
    goto    div16by16next

div16by16add
    ; Add divisor to remainder
    addwf   zl, f
    movf    yh, w
    skpnc
    incfsz  yh, w
    addwf   zh, f
    movlw   1
    skpnc
    addwf   temp, f

div16by16next
    ; Carry is next result bit
    decfsz  temp+1, f
    goto    div16by16loop

; Shift in last bit
    rlf     xl, f
    rlf     xh, f
    return


;******************************************************************************
; Mul_xl_by_w
;
; Calculates xh/xl = xl * w
;******************************************************************************
.utils_Mul_xl_by_w CODE
    GLOBAL Mul_xl_by_w
Mul_xl_by_w
    clrf    xh
	clrf    temp
    bsf     temp, 3
    rrf     xl, f

mul_xl_by_w_loop
	skpnc
	addwf   xh, f
    rrf     xh, f
    rrf     xl, f
	decfsz  temp, f
    goto    mul_xl_by_w_loop
    return


;******************************************************************************
; Mul_x_by_100
;
; Calculates xh/xl = xh/xl * 100
; Only valid for xh/xl <= 655 as the output is only 16 bits
;******************************************************************************
.utils_Mul_x_by_100 CODE
    GLOBAL Mul_x_by_100
Mul_x_by_100
    ; Shift accumulator left 2 times: xh/xl = xh/xl * 4
	clrc
	rlf	    xl, f
	rlf	    xh, f
	rlf	    xl, f
	rlf	    xh, f

    ; Copy accumulator to temporary location
	movf	xh, w
	movwf	temp+1
	movf	xl, w
	movwf	temp

    ; Shift temporary value left 3 times: temp+1/temp = xh/xl * 4 * 8   = xh/xl * 32
	clrc
	rlf	    temp, f
	rlf	    temp+1, f
	rlf	    temp, f
	rlf	    temp+1, f
	rlf	    temp, f
	rlf	    temp+1, f

    ; xh/xl = xh/xl * 32  +  xh/xl * 4   = xh/xl * 36
	movf	temp, w
	addwf	xl, f
	movf	temp+1, w
	skpnc
	incfsz	temp+1, w
	addwf	xh, f

    ; Shift temporary value left by 1: temp+1/temp = xh/xl * 32 * 2   = xh/xl * 64
	clrc
	rlf	    temp, f
	rlf	    temp+1, f

    ; xh/xl = xh/xl * 36  +  xh/xl * 64   = xh/xl * 100 
	movf	temp, w
	addwf	xl, f
	movf	temp+1, w
	skpnc
	incfsz	temp+1, w
	addwf	xh, f
    return


;******************************************************************************
; Div_x_by_4
;
; Calculates xh/xl = xh/xl / 4
;******************************************************************************
.utils_Div_x_by_4 CODE
    GLOBAL Div_x_by_4
Div_x_by_4
	clrc
	rrf     xh, f
	rrf	    xl, f
	clrc
	rrf     xh, f
	rrf	    xl, f
    return


;******************************************************************************
; Add_y_to_x
;
; This function calculates xh/xl = xh/xl + yh/yl.
; C flag is valid, Z flag is not!
;
; y stays unchanged.
;******************************************************************************
.utils_Add_y_to_x CODE
    GLOBAL Add_y_to_x
Add_y_to_x
    movf    yl, w
    addwf   xl, f
    movf    yh, w
    skpnc
    incf    yh, W
    addwf   xh, f
    return         


;******************************************************************************
; Sub_y_from_x
;
; This function calculates xh/xl = xh/xl - yh/yl.
; C flag is valid, Z flag is not!
;
; y stays unchanged.
;******************************************************************************
.utils_Sub_y_from_x CODE
    GLOBAL Sub_y_from_x
Sub_y_from_x
    movf    yl, w
    subwf   xl, f
    movf    yh, w
    skpc
    incfsz  yh, W
    subwf   xh, f
    return         


;******************************************************************************
; If_y_lt_x
;
; This function compares the 16 bit unsigned values in yh/yl with xh/xl.
; If y < x then C flag is cleared on exit
; If y >= x then C flag is set on exit
;
; x and y stay unchanged.
;******************************************************************************
.utils_If_y_lt_x CODE
    GLOBAL If_y_lt_x
If_y_lt_x
    movfw   xl
    subwf   yl, w
    movfw   xh
    skpc                
    incfsz  xh, w       
    subwf   yh, w
    return


;******************************************************************************
; If_x_eq_y
;
; This function compares the 16 bit unsigned values in yh/yl with xh/xl.
; If x == y then Z flag is set on exit
; If y != x then Z flag is cleared on exit
;
; x and y stay unchanged.
;******************************************************************************
.utils_If_x_eq_y CODE
    GLOBAL If_x_eq_y
If_x_eq_y
    movfw   xl
    subwf   yl, w
    skpz
    return
    movfw   xh
    subwf   yh, w
    return


;******************************************************************************
; Mul_x_by_6
;
; Calculates xh/xl = xl * 6
;
; Generated by www.piclist.com/cgi-bin/constdivmul.exe (1-May-2002 version)
;******************************************************************************
.utils_Mul_x_by_6 CODE
    GLOBAL Mul_x_by_6
Mul_x_by_6
    ; Shift accumulator left 1 times: xh/xl = xl * 2
	clrc
	rlf	    xl, f
	clrf	xh
	rlf	    xh, f

    ; Copy accumulator to temporary location
	movf	xh, w
	movwf	yh
	movf	xl, w
	movwf	yl

    ; Shift temporary value left 1 times: yh/yl = xl * 4
	clrc
	rlf	    yl, f
	rlf	    yh, f

    ; xh/xl  =  xh/xl + yh/yl  =  xl * 6
	movf	yl, w
	addwf	xl, f
	movf	yh, w
	skpnc
	incfsz	yh, w
	addwf	xh, f
    return


;******************************************************************************
; Add_x_and_780
;
; Calculates xh/xl = xh/xl + 780
;******************************************************************************
.utils_Add_x_and_780 CODE
    GLOBAL Add_x_and_780
Add_x_and_780
	movlw	LOW(780)
	addwf	xl, f
	movlw	HIGH(780)
    movwf   yh
	skpnc
	incfsz	yh, w
	addwf	xh, f
    return


;******************************************************************************
; TLC5916_send
;
; Sends the value in the light_data register to the TLC5916 LED driver.
; In case DUAL_TLC5916 is defined then 16 bits light_data, light_data+1 are sent. 
; This is used if two TLC5916 are wired up in series for 16 output channels.
;******************************************************************************
IFDEF PORT_SDI              ; Only enable this function when the ports are defined
IFDEF PORT_CLK
IFDEF PORT_LE
IFDEF PORT_OE
.utils_TLC5916_send
    GLOBAL TLC5916_send
TLC5916_send
    IFDEF DUAL_TLC5916      ; {
    movlw   16
    ELSE                    ; } {
    movlw   8
    ENDIF                   ; } DUAL_TLC5916
    movwf   temp

tlc5916_send_loop
    BANKSEL light_data
    IFDEF DUAL_TLC5916
    rlf     light_data+1, f
    ENDIF
    rlf     light_data, f
    BANKSEL PORTA
    skpc    
    bcf     PORT_SDI
    skpnc    
    bsf     PORT_SDI
    bsf     PORT_CLK
    bcf     PORT_CLK
    decfsz  temp, f
    goto    tlc5916_send_loop

    bsf     PORT_LE
    bcf     PORT_LE
    bcf     PORT_OE
    return
ENDIF ; PORT_OE
ENDIF ; PORT_LE
ENDIF ; PORT_CLK
ENDIF ; PORT_SDI


;******************************************************************************
; TLC5940_send
;
; Sends the value in the 16 light_data registers to the TLC5940 LED driver.
; The data is sent as 6 bit "dot correction" value as we are using the TLC5940s
; capability to programmatically change the constant current source that drives
; each individual LED.
;
; Note: The 16 bytes of light_data are being destroyed!
;******************************************************************************
IFDEF TLC5940
.utils_TLC5940_send
    GLOBAL TLC5940_send
TLC5940_send

    ; =============================
    ; First we pack the data from the 16 bytes into 12 bytes (6 bit x 16) as 
    ; needed by the TLC5940 Dot Correction register.
    ;
    ; We do that by using indirect addressing. We perform a loop 4 times where
    ; each loop run packs 4 bytes into 3.
    ;
    ; A single loop reads from FSR0 and writes to FSR1. The pointers are 
    ; decremented when the respective byte is full.
    ; The packing loop transforms the following input into output:
    ;
    ;      Byte N-3                   Byte N-2                   Byte N-1                   Byte N
    ; IN:  xx xx 15 14 13 12 11 10    xx xx 25 24 23 22 21 20    xx xx 35 34 33 32 31 30    xx xx 45 44 43 42 41 40    
    ; OUT:                            21 20 15 14 13 12 11 10    33 32 31 30 25 24 23 22    45 44 43 42 41 40 35 34
    ;
    ; Note that the algorithm starts at the top (MSB), as the data that goes
    ; on the bus to the TLC5940 is MSB first.
    ; This way on input light_data corresponds to OUT0 and light_data+15 
    ; corresponds to OUT15 on the TLC5940.
    movlw   HIGH light_data+15
    movwf   FSR0H
    movwf   FSR1H
    movlw   LOW light_data+15
    movwf   FSR0L
    movwf   FSR1L

TLC5940_send_pack_loop
    ; Bits 45..40 --> temp[7..2]
    moviw   FSR0--
    lslf    WREG, f
    lslf    WREG, f
    movwf   temp
    
    ; Bits 35..34 --> temp[1..0]; temp -> *FSR1--
    moviw   0[FSR0]
    swapf   WREG, f 
    andlw   b'00000011'
    iorwf   temp, w
    movwi   FSR1--

    ; Bits 33..30 --> temp[7..4]
    moviw   FSR0--
    swapf   WREG, f 
    andlw   b'11110000'
    movwf   temp

    ; Bits 25..22 --> temp[3..0]; temp -> *FSR1--
    moviw   0[FSR0]
    lsrf    WREG, f
    lsrf    WREG, f
    andlw   b'00001111'
    iorwf   temp, w
    movwi   FSR1--

    ; Bits 21..20 --> temp[7..6]
    moviw   FSR0--
    swapf   WREG, f 
    lslf    WREG, f
    lslf    WREG, f
    andlw   b'11000000'
    movwf   temp

    ; Bits 15..10 --> temp[5..0]; temp -> *FSR1--
    moviw   FSR0--
    andlw   b'00111111'
    iorwf   temp, w
    movwi   FSR1--

    ; Check if we reached light_data+3 in the output; If yes we are done!
    movfw   FSR1L
    sublw   LOW light_data+3
    bnz     TLC5940_send_pack_loop


    ; =============================
    ; Now we send 12 bytes in light_data+15..light_data+4 to the TLC5940
    movlw   12
    movwf   temp              
    movlw   HIGH light_data+15
    movwf   FSR0H
    movlw   LOW light_data+15
    movwf   FSR0L

    BANKSEL SSP1BUF         ; Note: temp is accessible in all banks!
TLC5940_send_loop
    moviw   FSR0--
    movwf   SSP1BUF
    btfss   SSP1STAT, BF    ; Wait for transmit done flag BF being set
    goto    $-1
    movf    SSP1BUF, w      ; Clears BF flag
    decfsz  temp, f
    goto    TLC5940_send_loop

    BANKSEL LATA
    bsf     PORT_XLAT
    nop
    bcf     PORT_XLAT 

       
    return
ENDIF ; TLC5940    


;******************************************************************************
; Send W out via the UART
;******************************************************************************
.utils_UART_send_w CODE
    GLOBAL UART_send_w
UART_send_w
    BANKSEL TXSTA
    btfss   TXSTA, TRMT
    goto    $-1         ; Wait for TSR register being empty

    ; Due to the large error in baudrate on slower PIC chips it is
    ; necessary to add a delay of one bit (~30us) between characters.
    ; To achieve this in the most efficient way we wait until the transmit
    ; register is empty. We then add a 30us delay for the extra bit. Finally
    ; we send the next character.
    ; This way the communication is reliable, and sending a pack of 4 bytes
    ; at 38400 baud takes approximately 1.1ms.

    movwf   temp        ; Save W
    
    ; For 32 MHz we need 80 loop runs; scale down according to FOSC 
    ; given in hw_*.inc
	movlw	80 * FOSC / 32      
	movwf	temp+1
UART_send_w_delay
	decfsz	temp+1, f
	goto	UART_send_w_delay
	
    movfw   temp        ; Restore W
    movwf   TXREG	    ; Send data stored in W
    return    


;******************************************************************************
; UART_read_byte
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
.utils_UART_read_byte CODE
    GLOBAL UART_read_byte
UART_read_byte
    BANKSEL RCSTA
	btfsc   RCSTA, OERR
	goto    overerror       ; if overflow error...
	btfsc   RCSTA, FERR
	goto	frameerror      ; if framing error...
uart_ready
    BANKSEL PIR1
	btfss	PIR1, RCIF
	goto	UART_read_byte  ; if not ready, wait...	

uart_gotit
	bcf     INTCON, GIE     ; Turn GIE off. This is IMPORTANT!
	btfsc	INTCON, GIE     ; MicroChip recommends this check!
	goto 	uart_gotit      ; !!! GOTCHA !!! without this check
                            ;   you are not sure gie is cleared!
    BANKSEL RCREG
	movf	RCREG, w        ; Read UART data
	bsf     INTCON, GIE     ; Re-enable interrupts
	return

    ; The code below is working in the bank where the UART registers are.
    ; The INTCON register is accessible in any bank so we don't need to
    ; switch back and forth!

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
	goto	UART_read_byte  ; Try again...

frameerror			
    ; Framing errors are usually due to wrong baud rate coming in.
	bcf     INTCON, GIE
	btfsc	INTCON, GIE
	goto 	frameerror

	movf	RCREG,w		;reading rcreg clears ferr flag.
	bsf     INTCON, GIE

	goto	UART_read_byte  ; Try again...



    END     ; Directive 'end of program'



