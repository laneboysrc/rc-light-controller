;******************************************************************************
;
;   hw_tlc5940_16f1825.inc
;
;   This file contains IO port definitions for version 2 of the
;   RC light controller which is based on the PIC 16F1825 and the TLC5940
;   LED driver.
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************

    PROCESSOR   PIC16F1825

    #include    <p16f1825.inc>

IFDEF INCLUDE_CONFIG
    __CONFIG     _CONFIG1, _FOSC_INTOSC & _WDTE_OFF & _PWRTE_ON & _MCLRE_OFF & _CP_OFF & _CPD_OFF & _BOREN_OFF & _CLKOUTEN_OFF & _IESO_OFF & _FCMEN_OFF
    __CONFIG     _CONFIG2, _WRT_OFF & _PLLEN_OFF & _STVREN_OFF & _LVP_OFF 
ENDIF


;******************************************************************************
FOSC = 32           ; Osc frequency in MHz
#define TLC5940     ; Let the code know we are using the TLC5940 LED driver
#define TIMER2_SOFT_TIMER ; Let the code know we are using Timer2 as soft timer

;******************************************************************************
;   Port usage:
;   ===========                                             
;   RA1:  IN  (12) Servo input ST; Slave in (UART RX); ICSPCLK
;   RA0:  IN  (13) Servo input TH; Slave steering wheel servo; ICSPDAT
;   RA3:  IN  ( 4) Servo input CH3; Vpp
;   RC4:  OUT ( 6) Slave out (UART TX); Master steering wheel servo
;
;   RC0:  OUT (10) SCLK TLC5940
;   RC2   OUT ( 8) SIN TLC5940 (SDO on 16F1825)
;   RC1:  IN  ( 9) Reserved for SPI EEPROM (SDI on 16F1825)
;   RA4:  OUT ( 3) XLAT TLC5940
;   RA5:  OUT ( 2) BLANK TLC5940
;   RC5:  OUT ( 5) GSCLK TLC5940
;
;   RA2:  OUT (11) Reserved for audio out (PWM using CCP3) 
;   RC3:  OUT ( 7) Reserved for SPI EEPROM (CS)


#define PORT_STEERING   PORTA, 1        
#define PORT_THROTTLE   PORTA, 0        
#define PORT_CH3        PORTA, 3        

#define PORT_SERVO_SLAVE LATA, 0        

; TLC5940 LED driver serial communication ports, for both master and slave
#define PORT_SCK        PORTC, 0
#define PORT_SDI        PORTC, 2
#define PORT_SDO        PORTC, 1
#define PORT_XLAT       LATA, 4
#define PORT_BLANK      LATA, 5
#define PORT_GSCLK      LATC, 5


;******************************************************************************
;* MACROS
;******************************************************************************
_IO_INIT_COMMON macro   master

    ;-----------------------------
    ; Oscillator Control 
    BANKSEL OSCCON
    movlw   b'11110000' ; 32MHz: 4x PLL enabled, 8 MHz HF, Clock determined by FOSC<2:0>
    movwf   OSCCON

    
    ;-----------------------------
    ; IO Port initialization
    BANKSEL LATA
    clrf    LATA
    clrf    LATC
    bsf     PORT_BLANK  ; Blank the LED output as quickly as possible
    BANKSEL ANSELA
    clrf    ANSELA
    BANKSEL APFCON0
    movlw   b'00000000' ; Use RC4/RC5 for UART TX/RX
    movwf   APFCON0
    clrf    APFCON1
    BANKSEL TRISA
    if (master == 1)    
        movlw   b'00001011' ; Make all ports A except RA0, RA1 and RA3 output
        movwf   TRISA

        movlw   b'00000010' ; Make all ports C except RC1 output
        movwf   TRISC
    else
        movlw   b'00001010' ; Make all ports A except RA1 and RA3 output (Slave!)
        movwf   TRISA

        movlw   b'00000010' ; Make all ports C except RC1 output
        movwf   TRISC
    endif
        
        
    ;-----------------------------
    ; Initialize Timer1 for 1 MHz operation
    BANKSEL T1CON
    movlw   b'00110000' 
    movwf   T1CON

    
    ;-----------------------------
    ; Use Timer2 as soft timer with an interval of 32.768ms. This is half
    ; of what we would like to have, so software has to adjust for it.
    BANKSEL PR2
    movlw   0xff
    movwf   PR2
    BANKSEL T2CON
    movlw   b'01111111' ; 1:16 post scaler; timer on; Prescaler is 64
    movwf   T2CON


    ;-----------------------------
    ; OPTION_REG configuration
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
    clrf    INTCON      ; Clear all interrupts and disable them


    ;-----------------------------
    ; UART specific initialization
BAUDRATE = 38400        ; Desired baudrate
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
    movwf	SPBRGL

    movlw   b'10000000'
            ; |||||||+ RX9D (not used)
            ; ||||||+- OERR (overrun error, read only)
            ; |||||+-- FERR (framing error)
            ; ||||+--- ADDEN(not used in 8 bit mode)
            ; |||+---- CREN (disable reception for MASTER)
            ; ||+----- SREN (not used in async mode)
            ; |+------ RX9  (cleared to use 8 bit mode = no parity)
            ; +------- SPEN (set to enable USART)
    movwf   RCSTA

    movlw   b'00000000'
            ; |||||||+ ABDEN  (Auto-Baud Detect mode is disabled)
            ; ||||||+- WUE    (Receiver is operating normally)
            ; |||||+--        (not implemented)
            ; ||||+--- BRG16  (8-bit Baud Rate Generator is used)
            ; |||+---- SCKP   (Transmit non-inverted data to the TX/CK pin)
            ; ||+-----        (not implemented)
            ; |+------ RCIDL  (read only)
            ; +------- ABDOVF (read only)
    movwf   BAUDCON

    movf	RCREG, w    ; Clear uart receiver including FIFO
    movf	RCREG, w
    movf	RCREG, w

    movlw	0           ; Send dummy character to get a valid transmit flag
    movwf	TXREG


    ;-----------------------------
    ; MSSP1 initialization for the TLC5940
    BANKSEL SSP1CON1
    movlw   b'00100000'
            ; |||||||+ SSPM0  (b'0000' = SPI Master mode, clock = FOSC/4)
            ; ||||||+- SSPM1
            ; |||||+-- SSPM2
            ; ||||+--- SSPM3
            ; |||+---- CKP    (Idle state for clock is a low level)
            ; ||+----- SSPEN  (Enables serial port and configures SCKx, SDOx, SDIx and SSx as the source of the serial port pins)
            ; |+------ SSPOV  (Clear overflow)
            ; +------- WCOL   (Clear collision)
    movwf   SSP1CON1

    movlw   b'01000000'
            ; |||||||+ 
            ; ||||||+- 
            ; |||||+-- 
            ; ||||+--- 
            ; |||+---- 
            ; ||+----- 
            ; |+------ CKE   (Transmit occurs on transition from active to idle clock state)
            ; +------- SMP   (Input data sampled at middle of data output time)
    movwf   SSP1STAT


    endm


IO_INIT_MASTER macro
    _IO_INIT_COMMON 1
    endm

    
IO_INIT_SLAVE macro
    _IO_INIT_COMMON 0
    endm


#define EEDATA EEDATL   ; New chips have EEDATL/H, we map it to the old EEDATA

    
