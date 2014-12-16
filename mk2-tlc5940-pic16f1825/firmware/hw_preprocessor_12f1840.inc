;******************************************************************************
;
;   io_preprocessor_12f1840.inc
;
;   This file contains IO port definitions for a light controller "master"
;   that functions as a pre-processor.
;   A pre-processor connects to the receiver and normalizes the servo channels
;   but does not control lights directly. It outputs to an intelligent slave
;   containing the light tables etc.
;   The pre-processor is intended to be built into the RC receiver.
;
;   This particular IO file is for the PIC12F1840 (8 pin).
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
    ; Due to a bug in GPUtils 1.0.0 we have to specify the configuration
    ; registers via the old method. The new method causes a core dump during
    ; linking as well as faulty hex files...
    __CONFIG     _CONFIG1, _FOSC_INTOSC & _WDTE_OFF & _PWRTE_ON & _MCLRE_OFF & _CP_OFF & _CPD_OFF & _BOREN_OFF & _CLKOUTEN_OFF & _IESO_OFF & _FCMEN_OFF
    __CONFIG     _CONFIG2, _WRT_OFF & _PLLEN_OFF & _STVREN_OFF & _LVP_OFF 
ENDIF


;******************************************************************************
FOSC = 16   ; Osc frequency in MHz


;******************************************************************************
;   Port usage:
;   ===========                                             
;   RA2:  IN  ( 5) Servo input ST
;   RA4:  IN  ( 3) Servo input TH
;   RA5:  IN  ( 2) Servo input CH3
;   RA0:  OUT ( 7) Slave out (UART TX), ICSPDAT (in circuit programming)
;
;   RA1:  NA  ( 6) Used internally for UART RX, ICSPCLK (in circuit programming)
;   RA3:  NA  ( 4) Vpp (in circuit programming)


#define PORT_STEERING   PORTA, 2
#define PORT_THROTTLE   PORTA, 4
#define PORT_CH3        PORTA, 5


;******************************************************************************
;* MACROS
;******************************************************************************
IO_INIT_PREPROCESSOR macro
    ;-----------------------------
    ; Oscillator Control 
    BANKSEL OSCCON
    movlw   b'01111010' ; 4x PLL disabled, 16 MHz HF, Internal oscillator
    movwf   OSCCON
    
    ;-----------------------------
    ; IO Port initialization
    BANKSEL PORTA
    clrf    PORTA
    BANKSEL LATA
    clrf    LATA
    BANKSEL ANSELA
    clrf    ANSELA
    BANKSEL TRISA
    movlw   b'11111111' ; Make all ports A input
    movwf   TRISA
    BANKSEL APFCON
    movlw   b'00000000' ; Use RA0/RA1 for UART TX/RX
    movwf   APFCON
    
    ;-----------------------------
    ; Initialize Timer1 for 1 MHz operation
    BANKSEL T1CON
    movlw   b'00100000' ; 
    movwf   T1CON
	
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

    BANKSEL 0
    endm

