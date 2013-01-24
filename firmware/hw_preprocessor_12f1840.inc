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
;   Port usage:
;   ===========                                             
;   RA3:        IN   Pin 4   Servo input ST
;   RA4:        IN   Pin 3   Servo input TH
;   RA5:        IN   Pin 2   Servo input CH3
;   RA0:        OUT  Pin 7   Slave out (UART TX), ICSPDAT (in circuit programming)
;
;   RA1:        NA   Pin 6   used internally for UART RX, ICSPCLK (in circuit programming)
;   RA2:        NA   Pin 5   not used

#define PORT_STEERING   PORTA, 3
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

    ;-----------------------------
    ; Initialize Timer1 for 1 MHz operation
    BANKSEL T1CON
    movlw   b'00100000' ; 
    movwf   T1CON

    ;-----------------------------
    ; Clear all memory locations between 0x20 and 0x7f
    BANKSEL 0
	clrf    FSR0H
    movlw   0x7f
	movwf	FSR0L
	movwf	0x20		; Store a non-zero value in the last RAM address we
                        ;  like to clear
clear_ram	
    decf	FSR0L, f		
	clrf	INDF0		; Clear Indirect memory location
	movfw	0x20		; If we reached the first RAM location it will be 0 now,
                        ;  so we are done!
	bnz	clear_ram       
	
    BANKSEL TXSTA
    ;-----------------------------
    ; UART specific initialization
OSC = d'16000000'       ; Osc frequency in Hz
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

    BANKSEL 0
    endm

