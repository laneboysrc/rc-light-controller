/******************************************************************************
    Port usage:
    ===========                                             
    RA2:  OUT ( 5) Data output to WS2812

    RA4:  NA  ( 3) 
    RA5:  NA  ( 2) 
    RA0:  NA  ( 7) 
    RA1:  NA  ( 6) 
    RA3:  NA  ( 4) Vpp (in circuit programming)

*/
#include "pic16regs.h"
#include <stdint.h>

#define NUM_LEDS 12


uint8_t led_data[3 * NUM_LEDS];

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



/*****************************************************************************
 Init_hardware()
 
 Initializes all used peripherals of the PIC chip.
 ****************************************************************************/
static void Init_hardware(void) {
    //-----------------------------
    // Clock initialization
    OSCCON = 0b11110000;    // 32MHz: 4x PLL enabled, 32 MHz HF

    //-----------------------------
    // IO Port initialization
    LATA = 0;
    ANSELA = 0;
    TRISA = 0b000000000;     
    APFCON0 = 0b00000000;   

    INTCON = 0;
}


/*****************************************************************************

 A "low" is a pulse duration of ~350ns, a "high" ~700ns.
 At 32 MHz we have an instruction cycle of 125ns, which means a low is
 ~3 instructions, and a high ~6 instructions.
 
 According to source on the internet the low cycle of the pulse is not 
 so timing critical.
 
 ****************************************************************************/
uint8_t count;
uint8_t ws2812_data;
void WS2812_send(uint8_t data) {

    ws2812_data = data;
    count = 8;
    
    __asm
    BANKSEL _ws2812_data
    movfw   _ws2812_data
    BANKSEL _count

WS2812_send_loop:    
    btfsc   WREG, 7
    goto    WS2812_send_loop_high
    bsf     LATA, 2
    nop
    nop
    bcf     LATA, 2
    goto    WS2812_send_loop_end
    
WS2812_send_loop_high:
    bsf     LATA, 2
    nop
    nop
    nop
    nop
    nop
    bcf     LATA, 2

WS2812_send_loop_end:
    RLF     WREG
    decfsz  _count, f
    goto    WS2812_send_loop    
    __endasm;
}


/*****************************************************************************
 main()
 
 No introduction needed ... 
 ****************************************************************************/
void main(void) {
    uint16_t loop_counter;
    uint8_t i;
    uint8_t value;
    
    Init_hardware();
    
    value = 1;

    led_data[0] = 0xa5;
    led_data[1] = 0xff;
    led_data[2] = 0xaa;
    led_data[3] = 0x20;
    led_data[4] = 0x20;
    led_data[5] = 0x20;
    
    while (1) {
        for (i = 0; i < sizeof(led_data) ; i++) {
            WS2812_send(led_data[i]);
        }
        for (loop_counter = 0; loop_counter < 1000; loop_counter++) {
            WREG = loop_counter & 0xff;
        }

        for (i = 0; i < sizeof(led_data) ; i++) {
            led_data[i] = value;
        }
        value <<= 1;
        if (value == 0) {
            value = 1;
        }
        
    }
}




