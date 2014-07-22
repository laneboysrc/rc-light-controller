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


void WS2812_send(void);

uint8_t light_data[3 * NUM_LEDS];

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

static const uint8_t dim_curve[] = {
    0,   1,   1,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,
    3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,   4,   4,   4,   4,
    4,   4,   4,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   6,   6,   6,
    6,   6,   6,   6,   6,   7,   7,   7,   7,   7,   7,   7,   8,   8,   8,   8,
    8,   8,   9,   9,   9,   9,   9,   9,   10,  10,  10,  10,  10,  11,  11,  11,
    11,  11,  12,  12,  12,  12,  12,  13,  13,  13,  13,  14,  14,  14,  14,  15,
    15,  15,  16,  16,  16,  16,  17,  17,  17,  18,  18,  18,  19,  19,  19,  20,
    20,  20,  21,  21,  22,  22,  22,  23,  23,  24,  24,  25,  25,  25,  26,  26,
    27,  27,  28,  28,  29,  29,  30,  30,  31,  32,  32,  33,  33,  34,  35,  35,
    36,  36,  37,  38,  38,  39,  40,  40,  41,  42,  43,  43,  44,  45,  46,  47,
    48,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,
    63,  64,  65,  66,  68,  69,  70,  71,  73,  74,  75,  76,  78,  79,  81,  82,
    83,  85,  86,  88,  90,  91,  93,  94,  96,  98,  99,  101, 103, 105, 107, 109,
    110, 112, 114, 116, 118, 121, 123, 125, 127, 129, 132, 134, 136, 139, 141, 144,
    146, 149, 151, 154, 157, 159, 162, 165, 168, 171, 174, 177, 180, 183, 186, 190,
    193, 196, 200, 203, 207, 211, 214, 218, 222, 226, 230, 234, 238, 242, 248, 255,
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
 Init_WS2812

 Initializes the operation of the WS2812 (or PL9823) LEDs and sets all LED 
 outputs to Off.
 
 The LEDs seem to have a reset timeing themselves, so we send the commands
 repeatidely for a certain time, which was determined using experiments.
 
 Note that the LEDs, especially the PL9823, still flash after power on, nothing 
 we can do about this.
 ****************************************************************************/
uint8_t delay;
void Init_WS2812(void) {
    uint8_t i;

    for (i = 0 ; i < sizeof(light_data); i++) {
        light_data[i] = 0;
    }

    delay = 10;

    for (i = 0; i < 10; i++) {
        // This delay is to ensure the minimum 50us reset time of the LEDs
        __asm
        BANKSEL _delay
        movlw   10
        movwf   _delay
low_loop:
        nop
        nop
        decfsz  _delay, f
        goto    low_loop      
        __endasm;
        
        WS2812_send();
    }
}


/*****************************************************************************
 WS2812_send

 Sends the value in the light_data registers to the WS2812 (or PL9823) LEDs.


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

 By using assembler we can get the delay between two bytes from 5us down 
 to 3us!
 
 This means we can send data for 12 LEDs within 500us.
 
 ****************************************************************************/

// These variables are local to WS2812_send, but they had to be moved to 
// global scope because otherwise the assembler can not access them.
static uint8_t ws2812_count;
static uint8_t ws2812_array_count;

void WS2812_send(void) {

    ws2812_array_count = sizeof(light_data);
    ws2812_count = 8;       // Need to initialize it to prevent optimizing it away...

    __asm
    movlw	LOW _light_data
    movwf   FSR0L
    movlw	HIGH _light_data
    movwf   FSR0H

    BANKSEL _ws2812_array_count

WS2812_send_loop:
    movlw   8
    movwf   _ws2812_count
    moviw   FSR0++

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

    decfsz  _ws2812_array_count, f
    goto    WS2812_send_loop
    __endasm;
}

uint8_t r;
uint8_t g;
uint8_t b;

void getRGB(uint16_t hue, uint8_t sat, uint8_t val) {
  /* convert hue, saturation and brightness ( HSB/HSV ) to RGB
     The dim_curve is used only on brightness/value and on saturation (inverted).
     This looks the most natural.      
  */
    uint16_t base;

    val = dim_curve[val];
    sat = 255 - dim_curve[255 - sat];
 
    if (sat == 0) { // Acromatic color (gray). Hue doesn't mind.
        r = val;
        g = val;
        b = val; 
        return;
    }
 
    base = ((255 - sat) * val) >> 8;

    switch(hue / 60) {
        case 0:
            r = val;
            g = (((val - base) * hue) / 60) + base;
            b = base;
            break;

        case 1:
            r = (((val - base) * (60 - (hue % 60))) / 60) +  base;
            g = val;
            b = base;
            break;

        case 2:
            r = base;
            g = val;
            b = (((val - base) * (hue % 60)) / 60) + base;
            break;

        case 3:
            r = base;
            g = (((val - base) * (60 - (hue % 60))) / 60) + base;
            b = val;
            break;

        case 4:
            r = (((val - base) * (hue % 60)) / 60) + base;
            g = base;
            b = val;
            break;

        case 5:
            r = val;
            g = base;
            b = (((val - base) * (60 - (hue % 60))) / 60) + base;
            break;
    }
}

/*****************************************************************************
 main()
 
 No introduction needed ... 
 ****************************************************************************/
void main(void) {
    uint16_t loop_counter;
    uint16_t h;
    uint8_t s;
    uint8_t v;
    uint8_t up;

    Init_hardware();
    Init_WS2812();

    for (h = 0 ; h < 1; h++) {
        for (loop_counter = 0; loop_counter < 60000; loop_counter++) {
            WREG = loop_counter & 0xff;
        }
    }

    up = 1;    
    v = 1;
    
    while (1) {

        if (up) {
            if (v >= 45) {
                up = 0;
            }
            else {
                ++v;
            }
        }
        else {
            if (v == 0) {
                up = 1;
            }
            else {
                --v;
            }
        }

        //getRGB(h, s, v);

        s = gamma6[v];
        light_data[0] = s;
        light_data[1] = 0;
        light_data[2] = 0;

        light_data[3] = 0;
        light_data[4] = 0;
        light_data[5] = s;

        WS2812_send();

        for (loop_counter = 0; loop_counter < 30000; loop_counter++) {
            WREG = loop_counter & 0xff;
        }
    }
}




