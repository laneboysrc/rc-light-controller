;******************************************************************************
;
;   io_original.inc
;
;   This file contains IO port definitions for the original schematics
;   of the RC light controller. 
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************

    PROCESSOR   PIC16F628A

    #include    <p16f628a.inc>

IFDEF INCLUDE_CONFIG
    CONFIG      FOSC = INTOSCIO
    CONFIG      WDTE = OFF
    CONFIG      PWRTE = ON
    CONFIG      MCLRE = OFF
    CONFIG      BOREN = OFF
    CONFIG      LVP = OFF
    CONFIG      CP = OFF, CPD = OFF
ENDIF

;******************************************************************************
;   Port usage:
;   ===========                                             
;   RB6         IN  Servo input ST (PGC double-usage)
;   RB7:        IN  Servo input TH (PGD double-usage)
;   RA5:        IN  Servo input CH3 (Vpp double-usage)
;   RB2:        OUT Slave out (UART TX)
;   RB1:        IN  Slave in (UART RX), connected to RB6 to share HW
;
;   RA3:        OUT CLK TLC5916
;   RA0         OUT SDI TLC5916
;   RA2:        OUT LE TLC5916
;   RB0:        OUT OE TLC5916
;
;   RA4         IN  Tied to RA0 for routing convenience. Note that RA4 is open
;                   drain so not good to use as SDI!
;   RA7, RB3:   IN  Tied to +Vdd for routing convenience!
;   RB5         IN  Tied to RB2 for routing convenience!
;   RA6, RA0, RA1, RB4:     OUT NC pins, switch to output


#define PORT_CH3        PORTA, 5        ; Master usage
#define PORT_STEERING   PORTB, 6        ; Master usage
#define PORT_THROTTLE   PORTB, 7        ; Master usage

#define PORT_SERVO      PORTB, 7        ; Slave usage

; TLC5916 LED driver serial communication ports, for both master and slave
#define PORT_CLK        PORTA, 3
#define PORT_SDI        PORTA, 0
#define PORT_LE         PORTA, 2
#define PORT_OE         PORTB, 0



;******************************************************************************
;* MACROS
;******************************************************************************
_IO_INIT_COMMON macro   master
    BANKSEL CMCON
    movlw   0x07
    movwf   CMCON       ; Disable the comparators

    clrf    PORTA       ; Set all (output) ports to GND
    clrf    PORTB


    BANKSEL OPTION_REG
    movlw   b'10000111'
            ; |||||||+ PS0  (Set pre-scaler to 1:256)
            ; ||||||+- PS1
            ; |||||+-- PS2
            ; ||||+--- PSA  (Use pre-scaler for Timer 0)
            ; |||+---- T0SE (not used when Timer 0 uses internal osc)
            ; ||+----- T0CS (Timer 0 to use internal oscillator)
            ; |+------ INTEDG (not used in this application)
            ; +------- RBPU (Disable Port B pull-ups)
    movwf   OPTION_REG

    bcf     INTCON, T0IF    ; Clear Timer 0 Interrupt Flag    


    BANKSEL TRISA
    ;-----------------------------
    ; Port direction
    if (master == 1)    
        movlw   b'10110000' ; Make all ports A exceot RA7, RA5 and RA4 output
        movwf   TRISA

        movlw   b'11101110' ; Make RB7, RB6, RB5, RB3, RB2 and RB1 inputs
        movwf   TRISB
    else
        movlw   b'10110000' ; Make all ports A exceot RA7, RA5 and RA4 output
        movwf   TRISA

        movlw   b'01101110' ; Make RB6, RB5, RB3, RB2 and RB1 inputs (for SLAVE!)
        movwf   TRISB
    endif


    BANKSEL 0
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
    movlw   b'10000000'
            ; |||||||+ RX9D (not used)
            ; ||||||+- OERR (overrun error, read only)
            ; |||||+-- FERR (framing error)
            ; ||||+---      (not implemented)
            ; |||+---- CREN (disable reception for MASTER)
            ; ||+----- SREN (not used in async mode)
            ; |+------ RX9  (cleared to use 8 bit mode = no parity)
            ; +------- SPEN (set to enable USART)
    movwf   RCSTA

    movf	RCREG, w    ; Clear uart receiver including FIFO
    movf	RCREG, w
    movf	RCREG, w

    movlw	0           ; Send dummy character to get a valid transmit flag
    movwf	TXREG
    endm

IO_INIT_MASTER macro
    _IO_INIT_COMMON 1
    endm
    
IO_INIT_SLAVE macro
    _IO_INIT_COMMON 0
    endm
    
