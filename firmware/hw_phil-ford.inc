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
;   RB1 ( 7):       IN  Slave in (UART RX)
;
;   RB2 ( 8):       OUT LED Tail/Brake light, brake function
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
;   RB3 ( 9)
;   RB4 (10)
;   RB5 (11)


#define PORT_LED_BRAKE              TRISB, 2
#define PORT_LED_TAIL               TRISA, 4
#define PORT_LED_BRAKE3             TRISA, 1
#define PORT_LED_ROOF               TRISA, 0
#define PORT_LED_MAIN_BEAM          TRISA, 2
#define PORT_LED_PARKING            TRISA, 7
#define PORT_LED_INDICATOR_LEFT     TRISA, 6
#define PORT_LED_INDICATOR_RIGHT    TRISA, 3
#define PORT_LED_REVERSE            TRISB, 0


;******************************************************************************
;* MACROS
;******************************************************************************
IO_INIT_MASTER macro
    ERROR "Phil's light controller isn't a master, it is an intelligent slave!"
    endm
    
IO_INIT_SLAVE macro
    BANKSEL TRISA
    ;-----------------------------
    ; Port direction
    movlw   b'11111111' ; Make all port A pins input
    movwf   TRISA

    movlw   b'11111111' ; Make all port B pins inputs
    movwf   TRISB
    endm
    
