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
;   RA1         IN  (12) Servo input ST; Slave in (UART RX); ICSPCLK
;   RA0:        IN  (13) Servo input TH; Slave steering wheel servo; ICSPDAT
;   RA3:        IN  ( 4) Servo input CH3; Vpp
;   RC4:        OUT ( 6) Slave out (UART TX); Master steering wheel servo
;
;   RC0:        OUT (10) SCK TLC5940
;   RC2         OUT ( 8) SDI TLC5940 (SDO on 16F1825)
;   RC1:        IN  ( 9) Reserved for SPI EEPROM (SDI on 16F1825)
;   RC3:        OUT ( 7) XLAT TLC5940
;   RC5:        OUT ( 5) VPROG TLC5940
;   RA4:        OUT ( 3) BLANK TLC5940
;   RA5:        OUT ( 2) GSCLK TLC5940
;
;   RA2:        OUT (11) Reserved for audio out (PWM using CCP3) 


#define PORT_STEERING   PORTA, 1        
#define PORT_THROTTLE   PORTA, 0        
#define PORT_CH3        PORTA, 3        

#define PORT_SERVO_SLAVE PORTA, 0        

; TLC5940 LED driver serial communication ports, for both master and slave
#define PORT_SCK        PORTC, 0
#define PORT_SDI        PORTC, 2
#define PORT_SDO        PORTC, 1
#define PORT_XLAT       PORTC, 3
#define PORT_VPROG      PORTC, 5
#define PORT_BLANK      PORTA, 4
#define PORT_GSCLK      PORTA, 5


;******************************************************************************
;* MACROS
;******************************************************************************
_IO_INIT_COMMON macro   master
    BANKSEL PORTA
    clrf    PORTA       ; Set all (output) ports to GND
    clrf    PORTC


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
    
