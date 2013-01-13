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
    movlw   b'10000011' ; Make all ports A except RA7, RA1 and RA0 output
    movwf   TRISA

    movlw   b'00000000' ; Make all ports B outputs
    movwf   TRISB
    endm
    
IO_INIT_SLAVE macro
  
    endm
    
