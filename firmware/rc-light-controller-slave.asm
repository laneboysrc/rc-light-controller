;******************************************************************************
;
;   rc-light-controller-slave.asm
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************
    TITLE       RC Light Controller
    LIST        p=pic16f628a, r=dec
    RADIX       dec

    #include    <p16f628a.inc>

    __CONFIG _CP_OFF & _WDT_OFF & _BODEN_ON & _PWRTE_ON & _INTRC_OSC_NOCLKOUT & _MCLRE_OFF & _LVP_OFF


;******************************************************************************
;   Port usage:
;   ===========
;   RB6, RB1:   IN  Servo input ST (PGC and RX for slave double-usage)
;   RB7:        IN  Servo input TH (PGD double-usage)
;   RA5:        IN  Servo input CH3 (Vpp double-usage)
;   RB2:        OUT Slave out (TX Master) / Servo out (Slave) 
;
;   RA3:        OUT CLK TLC5916
;   RA4:        OUT SDI TLC5916
;   RA2:        OUT LE TLC5916
;   RB0:        OUT OE TLC5916
;
;   RA7, RB3:   IN  Tied to +Vdd for routing convenience!
;   RB5         IN  RB5 is tied to RB2 for routing convenience!
;   RA6, RA0, RA1, RB4:     OUT NC pins, switch to output

#define PORT_SERVO      PORTB, 2

; TLC5916 LED driver serial communication ports
#define PORT_CLK        PORTA, 3
#define PORT_SDI        PORTA, 4
#define PORT_LE         PORTA, 2
#define PORT_OE         PORTB, 0


;******************************************************************************
;* VARIABLE DEFINITIONS
;******************************************************************************
    
    CBLOCK  0x70    ; 16 Bytes that are accessible via any bank!

    savew	        ; Interrupt save registers
    savestatus	
    savepclath
    savefsr

	ENDC

    CBLOCK  0x20    ; Bank 0

    servo_sync_flag
    pwm_counter

    uart_light_mode
    uart_light_mode_half
    uart_servo

    light_mode
    light_mode_half

	xl
	xh
	yl
	yh
    temp
    d0

	ENDC



;******************************************************************************
; Reset vector 
;******************************************************************************
    ORG     0x000           
    goto    Init


;******************************************************************************
; Interrupt vector
;******************************************************************************
    ORG     0x004           
Interrupt_handler
	movwf	savew           ; Save W register
	movf	STATUS, w       ; W now has copy of status
	clrf	STATUS          ; Ensure we are in bank 0 now!
	movwf	savestatus	    ; Save status
	movf	PCLATH, w       ; Save pclath
	movwf	savepclath	
	clrf	PCLATH		    ; Explicitly select Page 0

	movf	FSR, w
	movwf	savefsr		    ; Save FSR (just in case)

	btfss	INTCON, T0IF
	goto	int_clean   

    decfsz  pwm_counter, f
    goto    int_reload
      
    movf    light_mode, w
    iorwf   light_mode_half, w
    movwf   temp
    call    TLC5916_send
    goto    int_lights_done

int_reload
    movlw   2
    movwf   pwm_counter

    movf    light_mode, w
    movwf   temp
    call    TLC5916_send

int_lights_done
    clrf    servo_sync_flag

int_t0if_done
	bcf	    INTCON, T0IF    ; Clear interrupt flag that caused interrupt

int_clean
		movf	savefsr, w
		movwf	FSR		    ; Restore FSR
		movf	savepclath, w
		movwf	PCLATH      ; Restore PCLATH (Page=original)
		movf	savestatus, w
		movwf	STATUS      ; Restore status! (bank=original)
		swapf	savew, f    ; Restore W from *original* bank! 
		swapf	savew, w    ; Swapf does not affect any flags!
		retfie              


;******************************************************************************
; Initialization
;******************************************************************************
Init
    BANKSEL CMCON
    movlw   0x07
    movwf   CMCON       ; Disable the comparators

    clrf    PORTA       ; Set all (output) ports to GND
    clrf    PORTB


    BANKSEL OPTION_REG
    movlw   b'10000100'
            ; |||||||+ PS0  (Set pre-scaler to 1:16)
            ; ||||||+- PS1
            ; |||||+-- PS2
            ; ||||+--- PSA  (Use pre-scaler for Timer 0)
            ; |||+---- T0SE (not used when Timer 0 uses internal osc)
            ; ||+----- T0CS (Timer 0 to use internal oscillator)
            ; |+------ INTEDG (not used in this application)
            ; +------- RBPU (Disable Port B pull-ups)
    movwf   OPTION_REG


    ;-----------------------------
    ; Port direction
    movlw   b'10100000' ; Make all ports A exceot RA7 and RA5 output
    movwf   TRISA

    ; FIXME: RB2 needs to be output for slave!
    movlw   b'11101110' ; Make RB7, RB6, RB5, RB3, RB2 and RB1 inputs
    movwf   TRISB


    BANKSEL xl
    ;-----------------------------
    ; Clear all memory locations between 0x20 and 0x7f
    movlw   0x7f
	movwf	FSR
	movwf	0x20		; Store a non-zero value in the last RAM address we
                        ;  like to clear
clear_ram	
    decf	FSR, f		
	clrf	INDF		; Clear Indirect memory location
	movfw	0x20		; If we reached the first RAM location it will be 0 now,
    skpz                ;  so we are done!
	goto	clear_ram   


    BANKSEL TXSTA
    ;-----------------------------
    ; UART specific initialization
OSC = d'4000000'        ; Osc frequency in Hz
BAUDRATE = d'38400'     ; Desired baudrate
BRGH_VALUE = 1          ; Either 0 or 1
SPBRG_VALUE = (((d'10'*OSC/((d'64'-(d'48'*BRGH_VALUE))*BAUDRATE))+d'5')/d'10')-1

    movlw   b'00100000'
            ; |||||||+ TX9D (not used)
            ; ||||||+- TRMT (read only)
            ; |||||+-- BRGH (high baud rate generator)
            ; ||||+---      (not implemented)
            ; |||+---- SYNC (cleared to select async mode)
            ; ||+----- TXEN (set to enable transmit function)
            ; |+------ TX9  (cleared to use 8 bit mode = no parity)
            ; +------- CSRC (not used in async mode)
    movwf   TXSTA

    IF (BRGH_VALUE == 1)
        bsf TXSTA, BRGH
    ELSE
        bcf TXSTA, BRGH
    ENDIF
    movlw	SPBRG_VALUE
    movwf	SPBRG

    BANKSEL RCSTA
    movlw   b'10010000'
            ; |||||||+ RX9D (not used)
            ; ||||||+- OERR (overrun error, read only)
            ; |||||+-- FERR (framing error)
            ; ||||+---      (not implemented)
            ; |||+---- CREN (enable reception for SLAVE)
            ; ||+----- SREN (not used in async mode)
            ; |+------ RX9  (cleared to use 8 bit mode = no parity)
            ; +------- SPEN (set to enable USART)
    movwf   RCSTA

    movf	RCREG, w    ; Clear uart receiver including FIFO
    movf	RCREG, w
    movf	RCREG, w

    movlw	0           ; Send dummy character to get a valid transmit flag
    movwf	TXREG


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


    bcf     INTCON, T0IF    ; Clear Timer0 Interrupt Flag    
    bcf     PIR1, CCP1IF    ; Clear Timer1 Compare Interrupt Flag

	bsf	    INTCON, T0IE    ; Enable Timer0 interrupt
	bsf	    INTCON, GIE     ; Enable interrupts

;   goto    Main_loop    


;******************************************************************************
; Main program
;******************************************************************************
Main_loop
    call    Read_UART
    call    Set_light_mode
    call    Make_servo_pulse    
    goto    Main_loop


;******************************************************************************
; Read_UART
;
; This function returns after having successfully received a complete
; protocol frame via the UART.
;******************************************************************************
Read_UART
    call    read_UART_byte
    sublw   0x8f                    ; First byte the magic 0x8f?
    bnz     Read_UART               ; No: wait for 0x8f to appear

read_UART_byte_2
    call    read_UART_byte
    movwf   uart_light_mode         ; Store 2nd byte
    sublw   0x8f                    ; Is it the magic byte?
    bz      read_UART_byte_2        ; Yes: we must be out of sync...

read_UART_byte_3
    call    read_UART_byte
    movwf   uart_light_mode_half
    sublw   0x8f
    bz      read_UART_byte_2

read_UART_byte_4
    call    read_UART_byte
    movwf   uart_servo
    sublw   0x8f
    bz      read_UART_byte_2
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


;******************************************************************************
Set_light_mode
    movf    uart_light_mode, w
    movwf   light_mode
    movf    uart_light_mode_half, w
    movwf   light_mode_half
    return    

    
;******************************************************************************
Make_servo_pulse    
    movf    uart_servo, w
    movwf   xl
    call    Mul_x_by_6
    call    Add_x_and_780

    clrf    T1CON           ; Stop timer 1, runs at 1us per tick, internal osc
    clrf    TMR1H           ; Reset the timer to 0
    clrf    TMR1L
    movf    xl, w           ; Load Timer1 compare register
    movwf   CCPR1L
    movf    xh, w
    movwf   CCPR1H
    bcf     PIR1, CCP1IF    ; Clear Timer1 compare interrupt flag

    ; Synchronize with the interrupt to ensure the servo pulse is not
    ; interrupted and stays precise (i.e. no servo chatter)
    bsf     servo_sync_flag, 0
    btfsc   servo_sync_flag, 0
    goto    $ - 1

    bsf     T1CON, 0        ; Start timer 1
    bsf     PORT_SERVO      ; Set servo port to high pulse

    btfsc   PIR1, CCP1IF    ; Wait for compare value reached
    goto    $ - 1

    bcf     PORT_SERVO      ; Turn off servo pulse
    clrf    T1CON           ; Stop timer 1
    bcf     PIR1, CCP1IF

    return


;******************************************************************************
; TLC5916_send
;
; Sends the value in the temp register to the TLC5916 LED driver.
;******************************************************************************
TLC5916_send
    movlw   8
    movwf   d0

tlc5916_send_loop
    rlf     temp, f
    skpc    
    bcf     PORT_SDI
    skpnc    
    bsf     PORT_SDI
    bsf     PORT_CLK
    nop
    bcf     PORT_CLK
    decfsz  d0, f
    goto    tlc5916_send_loop

    bsf     PORT_LE
    nop
    bcf     PORT_LE
    bcf     PORT_OE
    return


;******************************************************************************
; Mul_x_by_6
;
; Calculates xh/xl = xl * 6
;
; Generated by www.piclist.com/cgi-bin/constdivmul.exe (1-May-2002 version)
;******************************************************************************
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
Add_x_and_780
	movlw	LOW(780)
	addwf	xl, f
	movlw	HIGH(780)
    movwf   yh
	skpnc
	incfsz	yh, w
	addwf	xh, f
    return



    END


;******************************************************************************
; Timing architecture:
; ====================
; Timer0 will be used to provide a periodic interrupt of 4096 us.
; This will require a pre-scaler value of 16.
;
; This allows us to send a servo pulse of 2500 us without being disturbed
; by an interrupt by means of synchronization.
;
; The servo pulse is generated using Timer1 in "Compare mode, generate software 
; interrupt on match" mode. The servo pulse is sent after each UART command.
; Since the master sends the UART based on reading the RC receiver, the repeat 
; timing should relatively match the normal 20 ms interval between pulses.
;******************************************************************************

