;******************************************************************************
;
;   hw_ws2812_12f1840.inc
;
;   This file contains IO port definitions for the version of the
;   RC light controller which is based on the PIC 12F1840 driving a string of
;   "Smart LEDs" based on the WS2812 (or PL9823) chip.
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************

    PROCESSOR   PIC12F1840

    #include    <p12f1840.inc>

IFDEF INCLUDE_CONFIG
    __CONFIG     _CONFIG1, _FOSC_INTOSC & _WDTE_OFF & _PWRTE_ON & _MCLRE_OFF & _CP_OFF & _CPD_OFF & _BOREN_OFF & _CLKOUTEN_OFF & _IESO_OFF & _FCMEN_OFF
    __CONFIG     _CONFIG2, _WRT_OFF & _PLLEN_OFF & _STVREN_OFF & _LVP_OFF 
ENDIF


;******************************************************************************
FOSC = 32           ; Osc frequency in MHz
#define WS2812      ; Let the code know we are using the WS2812 LED driver
#define TIMER2_SOFT_TIMER ; Let the code know we are using Timer2 as soft timer

;******************************************************************************
;   Port usage:
;   ===========                                             
;   RA0:  OUT ( 7) Master steering wheel servo; (UART TX); ICSPDAT
;   RA1:  IN  ( 6) Servo input ST; Slave in (UART RX); ICSPCLK
;   RA2:  IN  ( 5) Servo input TH; 
;   RA3:  IN  ( 4) Servo input CH3; Vpp
;   RA4:  OUT ( 3) 
;   RA5:  OUT ( 2) WS2812 data


#define PORT_STEERING   PORTA, 1        
#define PORT_THROTTLE   PORTA, 2        
#define PORT_CH3        PORTA, 3        

IFDEF SERVO_OUTPUT_ON_THROTTLE
#define PORT_SERVO      LATA, 2
ELSE
#define PORT_SERVO      LATA, 0        
ENDIF

; WS2812 data port
#define PORT_WS2812_OUT LATA, 5


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
    BANKSEL ANSELA
    clrf    ANSELA
    BANKSEL APFCON
    movlw   b'00000000' ; Use RA0/RA1 for UART TX/RX
    movwf   APFCON
    BANKSEL TRISA
    movlw   b'00001110' ; Make ports RA0, RA4 and RA5 output
    movwf   TRISA
        
        
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

IFDEF ENABLE_UART
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

    movlw   b'10010000'
            ; |||||||+ RX9D (not used)
            ; ||||||+- OERR (overrun error, read only)
            ; |||||+-- FERR (framing error)
            ; ||||+--- ADDEN(not used in 8 bit mode)
            ; |||+---- CREN (enable reception for MASTER)
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
ENDIF

    endm


IO_INIT_MASTER macro
    _IO_INIT_COMMON 0
    endm

    
IO_INIT_SLAVE macro
    _IO_INIT_COMMON 0
    endm


#define EEDATA EEDATL   ; New chips have EEDATL/H, we map it to the old EEDATA
    
