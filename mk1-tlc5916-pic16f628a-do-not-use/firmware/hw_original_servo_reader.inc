;******************************************************************************
;
;   io_original.inc
;
;   This file contains IO port definitions for the original schematics
;   of the RC light controller. 
;   Specifically it initializes the hardware to read servo signals, which
;   is used for masters.
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************

    PROCESSOR   PIC16F628

    #include    <p16f628.inc>

IFDEF INCLUDE_CONFIG
    __CONFIG    _INTRC_OSC_NOCLKOUT & _WDT_OFF & _PWRTE_ON & _MCLRE_OFF & _CP_OFF & _CPD_OFF & _BOREN_OFF & _LVP_OFF
ENDIF


;******************************************************************************
FOSC = 4    ; Osc frequency in MHz


;******************************************************************************
;   Port usage:
;   ===========                                             
;   RB6:  IN  (12) Servo input ST (PGC double-usage)
;   RB7:  IN  (13) Servo input TH (PGD double-usage)
;   RA5:  IN  ( 4) Servo input CH3 (Vpp double-usage)
;   RB2:  OUT ( 8) Slave out (UART TX)
;   RB1:  IN  ( 7) Slave in (UART RX), connected to RB6 to share HW
;
;   RA3:  OUT ( 2) CLK TLC5916
;   RA0:  OUT (17) SDI TLC5916
;   RA2:  OUT ( 1) LE TLC5916
;   RB0:  OUT ( 6) OE TLC5916
;
;   RA4:  IN  ( 3) Tied to RA0 for routing convenience. Note that RA4
;                  is open drain so not good to use as SDI!
;   RA7:  IN  (16) Tied to +Vdd for routing convenience!
;   RB3:  IN  ( 9) Tied to +Vdd for routing convenience!
;   RB5:  IN  (11) Tied to RB2 for routing convenience!
;
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
IO_INIT_MASTER macro
    BANKSEL CMCON
    movlw   0x07
    movwf   CMCON       ; Disable the comparators


    BANKSEL PORTA
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


    BANKSEL INTCON
    clrf    INTCON          ; Clear all interrupt flags; disable interrupts


    ;-----------------------------
    ; Port direction
    BANKSEL TRISA
    movlw   b'10110000' ; Make all ports A exceot RA7, RA5 and RA4 output
    movwf   TRISA

    movlw   b'11101110' ; Make RB7, RB6, RB5, RB3, RB2 and RB1 inputs
    movwf   TRISB


    ;-----------------------------
    ; UART specific initialization
BAUDRATE = d'38400'     ; Desired baudrate
BRGH_VALUE = 1          ; Either 0 or 1
SPBRG_VALUE = (((d'10'*FOSC*d'1000000'/((d'64'-(d'48'*BRGH_VALUE))*BAUDRATE))+d'5')/d'10')-1

    BANKSEL TXSTA
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

