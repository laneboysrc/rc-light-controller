/******************************************************************************
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <LPC8xx.h>

#include <globals.h>
#include <uart0.h>

static uint16_t light_mode;
static uint8_t tlc5940_light_data[16];

/*
SPI configuration:
    Configuration: CPOL = 0, CPHA = 0,
    We can send 6 bit frame lengths, so no need to pack light data!
    TXRDY indicates when we can put the next data into txbuf
    Check Master idle status flag before asserting XLAT
*/

static void send_light_data_to_tlc5940(void)
{
    volatile int i;

    LPC_GPIO_PORT->W0[10] = 0;          // XLAT = 0

    for (i = 15; i >= 0; i--) {
        // Wait for TXRDY
        while (!(LPC_SPI0->STAT & (1 << 1)));

        LPC_SPI0->TXDAT = tlc5940_light_data[i];
    }

    // Force END OF TRANSFER
    LPC_SPI0->STAT = (1 << 7);

    // Wait for MSTIDLE
    while (!(LPC_SPI0->STAT & (1 << 8)));

    LPC_GPIO_PORT->W0[10] = 1;          // XLAT = 1
}


void init_lights(void)
{
    tlc5940_light_data[0] = 5;

    LPC_GPIO_PORT->W0[2] = 1;           // BLANK = 1
    LPC_GPIO_PORT->W0[10] = 0;          // XLAT = 0
    LPC_GPIO_PORT->W0[1] = 0;           // GSCLK = 0

    LPC_SPI0->DIV = 100;
    LPC_SPI0->DLY = 0;

    LPC_SPI0->CFG = (1 << 0) |          // Enable SPI0
                    (1 << 2) |          // Master mode
                    (0 << 3) |          // LSB First mode disabled
                    (0 << 4) |          // CPHA = 0
                    (0 << 5);           // CPOL = 0

    LPC_SPI0->TXCTRL = (1 << 21) |      // set EOF
                       (1 << 22) |      // RXIGNORE
                       ((6 - 1) << 24); // 6 bit frames

    LPC_SWM->PINASSIGN3 = 0x0bffffff;   // PIO0_11 is SCK
    LPC_SWM->PINASSIGN4 = 0x0fffff03;   // PIO0_3 is SIN (MOSI)

    send_light_data_to_tlc5940();

    LPC_GPIO_PORT->W0[2] = 0;           // BLANK = 0
    LPC_GPIO_PORT->W0[1] = 1;           // GSCLK = 1
}


void next_light_sequence(void)
{
	;
}


void more_lights(void)
{
    // Switch light mode up (Parking, Low Beam, Fog, High Beam)
    light_mode <<= 1;
    light_mode |= 1;
    light_mode &= config.light_mode_mask;
}


void less_lights(void)
{
    // Switch light mode down (Parking, Low Beam, Fog, High Beam)
    light_mode >>= 1;
    light_mode &= config.light_mode_mask;
}


void toggle_lights(void)
{
    if (light_mode == config.light_mode_mask) {
        light_mode = 0;
    }
    else {
        light_mode = config.light_mode_mask;
    }
}


void process_lights(void)
{
    if (global_flags.systick) {
        ++tlc5940_light_data[0];
        send_light_data_to_tlc5940();
    }

    static uint16_t old_light_mode = 0xffff;
    if (light_mode != old_light_mode) {
        old_light_mode = light_mode;
        uart0_send_cstring("light_mode ");
        uart0_send_uint32(light_mode);
        uart0_send_linefeed();
    }
}
