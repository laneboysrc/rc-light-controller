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


// These tables were created by gamma.c
// https://github.com/adafruit/Adafruit_NeoMatrix/blob/master/extras/gamma.c
static const uint8_t gamma4[] = {
      0,  0,  1,  4,  8, 15, 24, 35, 50, 68, 89,114,143,176,213,255
};

static const uint8_t gamma5[] = {
      0,  0,  0,  1,  1,  2,  4,  5,  8, 10, 13, 17, 22, 27, 32, 39,
     46, 53, 62, 71, 82, 93,105,117,131,146,161,178,196,214,234,255
};

static const uint8_t gamma6[] = {
      0,  0,  0,  0,  0,  0,  1,  1,  1,  2,  2,  3,  3,  4,  5,  6,
      7,  8, 10, 11, 13, 15, 17, 19, 21, 23, 26, 28, 31, 34, 37, 40,
     44, 47, 51, 55, 60, 64, 69, 73, 78, 83, 89, 94,100,106,113,119,
    126,133,140,147,155,163,171,179,188,197,206,215,225,234,245,255
};


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

 According to:  
    http://cpldcpu.wordpress.com/2014/01/14/light_ws2812-library-v2-0-part-i-understanding-the-ws2812/

 - A reset is issued as early as at 9 µs, contrary to the 50 µs mentioned in 
   the data sheet. Longer delays between transmissions should be avoided.

 - The cycle time of a bit should be at least 1.25 µs, the value given in the 
   data sheet, and at most ~9 µs, the shortest time for a reset.

 - A “0″ can be encoded with a pulse as short as 62.5 ns, but should not be 
   longer than ~500 ns (maximum on WS2812).

 - A “1″ can be encoded with pulses almost as long as the total cycle time, 
   but it should not be shorter than ~625 ns (minimum on WS2812B). 
 
 ****************************************************************************/
// These variables are local to WS2812_send, but they had to be moved to 
// global scope because the compiler optimized them away...
static uint8_t ws2812_count;
static uint8_t ws2812_data;
void WS2812_send_byte(uint8_t data) {

    ws2812_data = data;
    ws2812_count = 8;
    
    __asm
    BANKSEL _ws2812_data
    movfw   _ws2812_data
    BANKSEL _ws2812_count

WS2812_send_byte_loop:    
    btfsc   WREG, 7
    goto    WS2812_send_byte_loop_high
    
WS2812_send_byte_loop_low:
    bsf     LATA, 2                 ; LOW: 3 cycles = 375ns
    nop
    nop
    bcf     LATA, 2                 
    ; The loop takes 10 cycles from here to reach the next bit, which is 1000ns. 
    ; This makes the total LOW pulse duration ~1375ns, which is fine according
    ; to the info referenced above.
    goto    WS2812_send_byte_loop_end
    
WS2812_send_byte_loop_high:
    bsf     LATA, 2                 ; HIGH: 6 cycles = 750ns
    nop
    nop
    nop
    nop
    nop
    bcf     LATA, 2
    ; From here the loop takes 7 cycles until the next bit, which is more than 
    ; needed.

WS2812_send_byte_loop_end:
    rlf     WREG, f
    decfsz  _ws2812_count, f
    goto    WS2812_send_byte_loop    
    __endasm;
}


/*****************************************************************************

 By using assembler we can get the delay between two bytes from 5us down 
 to 3us!
 
 This means we can send data for 12 LEDs within 500us.
 ****************************************************************************/
uint8_t ws2812_array_count;
void WS2812_send(void) {

    ws2812_array_count = sizeof(led_data);

    __asm
    movlw	LOW _led_data
    movwf   FSR0L
    movlw	HIGH _led_data
    movwf   FSR0H

    BANKSEL _ws2812_array_count

WS2812_send_loop:
	moviw	FSR0++
	call	_WS2812_send_byte    
    decfsz  _ws2812_array_count, f
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
    
    while (1) {
        for (i = 0; i < sizeof(led_data) ; i++) {
            led_data[i] = gamma4[value & 0x0f];
        }
        value <<= 1;
        if (value == 0) {
            value = 1;
        }

        WS2812_send();

        for (loop_counter = 0; loop_counter < 1000; loop_counter++) {
            WREG = loop_counter & 0xff;
        }
    }
}




