;******************************************************************************
;
;   hw_preprocessor.inc
;
;   This file contains IO port definitions for a light controller pre-processor.
;
;   A pre-processor connects to the receiver and normalizes the servo channels
;   but does not control lights directly. It outputs to a "master" light 
;   controller containing the code to switch the lights etc.
;
;   The pre-processor is intended to be built into the RC receiver.
;
;   The IO ports have been assigned in such a away that none of the programming
;   ports are used. This way we should be able to re-program the PIC without
;   having to de-solder a lot of wires or risking damage of the receiver.
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
;   RA7:  IN  (16) Servo input ST
;   RA0:  IN  (17) Servo input TH
;   RA1:  IN  (18) Servo input CH3
;   RB2:  OUT ( 8) Slave out (UART TX)
;
;   RB7:  NA  (13) PGD (in circuit programming)
;   RB6:  NA  (12) PGC (in circuit programming)
;   RA5:  NA  ( 4) Vpp (in circuit programming)


#define PORT_STEERING   PORTA, 7
#define PORT_THROTTLE   PORTA, 0
#define PORT_CH3        PORTA, 1


;******************************************************************************
;* MACROS
;******************************************************************************
IO_INIT_PREPROCESSOR macro
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
    clrf     INTCON          ; Disable all interrupts


    BANKSEL TRISA
    ;-----------------------------
    ; Port direction
    movlw   b'10000011' ; Make all ports A except RA7, RA1 and RA0 output
    movwf   TRISA

    movlw   b'00000000' ; Make all ports B outputs
    movwf   TRISB


    ;-----------------------------
    ; Initialize Timer1
    BANKSEL T1CON
    clrf    T1CON       ; Stop timer 1, runs at 1us per tick, internal osc


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

    BANKSEL 0
    endm
    
