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
    CONFIG      FOSC = INTOSC
    CONFIG      PLLEN = ON

    CONFIG      PWRTE = ON
    CONFIG      WDTE = OFF 
    CONFIG      MCLRE = OFF
    CONFIG      CP = OFF, CPD = OFF
    CONFIG      BOREN = OFF
    CONFIG      CLKOUTEN = OFF
    CONFIG      IESO = OFF
    CONFIG      FCMEN = OFF
    CONFIG      WRT = OFF
    CONFIG      STVREN = OFF
    CONFIG      LVP = OFF
ENDIF


;******************************************************************************
;   Port usage:
;   ===========                                             
;   RA7:        IN   Pin 16  Servo input ST
;   RA0:        IN   Pin 17  Servo input TH
;   RA1:        IN   Pin 18  Servo input CH3
;   RB2:        OUT  Pin 8   Slave out (UART TX)
;
;   RB7:        NA   Pin 13  PGD (in circuit programming)
;   RB6:        NA   Pin 12  PGC (in circuit programming)
;   RA5:        NA   Pin 4   Vpp (in circuit programming)

#define PORT_STEERING   PORTA, 7
#define PORT_THROTTLE   PORTA, 0
#define PORT_CH3        PORTA, 1


; Dummy definitions, mapped to an unused register so they don't disturb anything
#define PORT_SERVO      port_dummy, 0         
#define PORT_CLK        port_dummy, 0
#define PORT_SDI        port_dummy, 0
#define PORT_LE         port_dummy, 0
#define PORT_OE         port_dummy, 0


;******************************************************************************
;* MACROS
;******************************************************************************
IO_INIT_MASTER macro
    BANKSEL TRISA
    ;-----------------------------
    ; Port direction
    movlw   b'11111111' ; Make all ports A input
    movwf   TRISA
    endm
    
IO_INIT_SLAVE macro
  
    endm
    
