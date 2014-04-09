/******************************************************************************

;   A pre-processor connects to the receiver and normalizes the servo channels
;   but does not control lights directly. It outputs to an intelligent slave
;   containing the light tables etc.
;   The pre-processor is intended to be built into the RC receiver.

   UART output (38400/n/8/1)

   Use SDCC (http://sdcc.sourceforge.net/) to compile it.
       
   Port usage:
   ===========                                             
;   Port usage:
;   ===========                                             
;   RA2:  IN  ( 5) Servo input ST
;   RA4:  IN  ( 3) Servo input TH
;   RA5:  IN  ( 2) Servo input CH3
;   RA0:  OUT ( 7) Slave out (UART TX), ICSPDAT (in circuit programming)
;
;   RA1:  NA  ( 6) Used internally for UART RX, ICSPCLK (in circuit programming)
;   RA3:  NA  ( 4) Vpp (in circuit programming)

*/
#include "processor.h"
#include <stdint.h>


// Chip configuration
static __code uint16_t __at (_CONFIG1) configword1 = 
    _FOSC_INTOSC & 
    _WDTE_OFF & 
    _PWRTE_ON & 
    _MCLRE_OFF & 
    _CP_OFF & 
    _CPD_OFF & 
    _BOREN_OFF & 
    _CLKOUTEN_OFF & 
    _IESO_OFF & 
    _FCMEN_OFF;

static __code uint16_t __at (_CONFIG2) configword2 = 
    _WRT_OFF & 
    _PLLEN_OFF & 
    _STVREN_OFF & 
    _LVP_OFF; 


extern void Init_servo_reader(void);
extern void Read_servos(void);

extern void Init_output(void);
extern void Output_result(void);


/*****************************************************************************
 Init_hardware()
 
 Initializes all used peripherals of the PIC chip.
 ****************************************************************************/
static void Init_hardware(void) {
    //-----------------------------
    // Clock initialization
    OSCCON = 0b01111000;    // 16MHz: 4x PLL disabled, 8 MHz HF, Clock determined by FOSC<2:0>

    //-----------------------------
    // IO Port initialization
    LATA = 0;
    ANSELA = 0;
    TRISA = 0b00110100;     // Make servo ports RA2, RA4 and RA5 inputs
    APFCON0 = 0b00000000;   // Use RA0/RA1 for UART TX/RX

    INTCON = 0;
}


/*****************************************************************************
 main()
 
 No introduction needed ... 
 ****************************************************************************/
void main(void) {
    Init_hardware();
    Init_output();
    Init_servo_reader();
    
    while (1) {
        Read_servos();
        Output_result();
    }
}




