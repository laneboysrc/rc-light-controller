;******************************************************************************
;
;   io_original.inc
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

    PROCESSOR   PIC16F628A

    #include    <p16f628a.inc>

IFDEF INCLUDE_CONFIG
    __CONFIG    _INTOSC_OSC_NOCLKOUT & _WDT_OFF & _PWRTE_ON & _MCLRE_OFF & _CP_OFF & _CPD_OFF & _BOREN_OFF & _LVP_OFF
ENDIF


;******************************************************************************
;   Port usage:
;   ===========                                             
;   RB1 ( 7):       IN  Slave in (UART RX)
;   RB2 ( 8)        OUT TX pin, can not be used as we need the UART
;
;   RB3 ( 9):       OUT LED Tail/Brake light, brake function
;   RA4 ( 3):       OUT LED Tail/Brake light, tail light function
;   RA1 (18):       OUT LED 3rd Brake light
;   RA0 (17):       OUT LED roof light bar
;   RA2 ( 1):       OUT LED Main beam front
;   RA7 (16):       OUT LED Parking lights front
;   RA3 ( 2):       OUT LED Indicators right
;   RA6 (15):       OUT LED Indicators left
;   RB0 ( 6):       OUT LED Reversing lights
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

    clrf    PORTA       ; Set all (output) ports to GND
    clrf    PORTB


    BANKSEL TRISA
    ;-----------------------------
    ; Port direction
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

    bcf     INTCON, T0IF    ; Clear Timer 0 Interrupt Flag    


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

    movlw   b'00000000'
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
    movwf	SPBRG

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
    endm
    
