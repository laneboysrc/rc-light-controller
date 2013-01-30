;******************************************************************************
;
;   hw_phil-ford.inc
;
;   This file contains IO port definitions for the RC light controller
;   for Phil's (RCGolf) Ford. 
;   This hardware does not have a TLC5916 but drives the LEDs directly
;   from the PIC16F628A ports (open drain type)
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
;   RB1:  IN  ( 7) Slave in (UART RX)
;   RB2:  OUT ( 8) TX pin, can not be used as we need the UART
;
;   RB3:  OUT ( 9) LED Tail/Brake light, brake function
;   RA4:  OUT ( 3) LED Tail/Brake light, tail light function
;   RA1:  OUT (18) LED 3rd Brake light
;   RA0:  OUT (17) LED roof light bar
;   RA2:  OUT ( 1) LED Main beam front
;   RA7:  OUT (16) LED Parking lights front
;   RA3:  OUT ( 2) LED Indicators right
;   RA6:  OUT (15) LED Indicators left
;   RB0:  OUT ( 6) LED Reversing lights
;
;   ICSP pins (not connected to other functions)
;   RA5 ( 4)        Vpp
;   RB7 (13)        PGD
;   RB6 (12)        PGC
;
;   Unused pins
;   RB4 (10)
;   RB5 (11)


#define PORT_LED_BRAKE              TRISB, 3
#define PORT_LED_TAIL               TRISA, 4
#define PORT_LED_BRAKE3             TRISA, 1
#define PORT_LED_ROOF_BIT           0
#define PORT_LED_ROOF               PORTA, PORT_LED_ROOF_BIT
#define PORT_LED_MAIN_BEAM          TRISA, 2
#define PORT_LED_PARKING            TRISA, 7
#define PORT_LED_INDICATOR_LEFT     TRISA, 6
#define PORT_LED_INDICATOR_RIGHT    TRISA, 3
#define PORT_LED_REVERSE            TRISB, 0


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


    ;-----------------------------
    ; Port direction
    BANKSEL TRISA
    movlw   b'11111110' ; Make all port A pins input except RA0 which drives the roof light
    movwf   TRISA

    movlw   b'11111111' ; Make all port B pins inputs
    movwf   TRISB


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
    clrf    INTCON          ; Disable all interrupts


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
            ; ||+----- TXEN (cleared to disable transmit function)
            ; |+------ TX9  (cleared to use 8 bit mode = no parity)
            ; +------- CSRC (not used in async mode)
    movwf   TXSTA

    IF (BRGH_VALUE == 1)
        bsf TXSTA, BRGH
    ELSE
        bcf TXSTA, BRGH
    ENDIF
    movlw	SPBRG_VALUE
    movwf   SPBRG

    BANKSEL RCSTA
    movlw   b'10010000'
            ; |||||||+ RX9D (not used)
            ; ||||||+- OERR (overrun error, read only)
            ; |||||+-- FERR (framing error)
            ; ||||+---      (not implemented)
            ; |||+---- CREN (enable reception)
            ; ||+----- SREN (not used in async mode)
            ; |+------ RX9  (cleared to use 8 bit mode = no parity)
            ; +------- SPEN (set to enable USART)
    movwf   RCSTA

    movf	RCREG, w    ; Clear uart receiver including FIFO
    movf	RCREG, w
    movf	RCREG, w

    movlw	0           ; Send dummy character to get a valid transmit flag
    movwf	TXREG
    
    BANKSEL 0
    endm
    
