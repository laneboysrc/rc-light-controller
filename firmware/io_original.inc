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
IO_INIT_MASTER macro
    BANKSEL TRISA
    ;-----------------------------
    ; Port direction
    movlw   b'10110000' ; Make all ports A exceot RA7, RA5 and RA4 output
    movwf   TRISA

    movlw   b'11101110' ; Make RB7, RB6, RB5, RB3, RB2 and RB1 inputs
    movwf   TRISB
    endm
    
IO_INIT_SLAVE macro
    BANKSEL TRISA
    ;-----------------------------
    ; Port direction
    movlw   b'10110000' ; Make all ports A exceot RA7, RA5 and RA4 output
    movwf   TRISA

    movlw   b'01101110' ; Make RB6, RB5, RB3, RB2 and RB1 inputs (for SLAVE!)
    movwf   TRISB
    endm
    
