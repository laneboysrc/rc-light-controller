/******************************************************************************
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

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

void init_lights(void)
{
    ;
}


void process_lights(void)
{
    static uint16_t old_light_mode = 0xffff;
    if (light_mode != old_light_mode) {
        old_light_mode = light_mode;
        uart0_send_cstring("light_mode ");
        uart0_send_uint32(light_mode);
        uart0_send_linefeed();
    }   
    //if (global_flags.new_channel_data) {
    //    uart0_send_cstring("CH3 ");
    //    uart0_send_int32((int32_t)channel[CH3].normalized);
    //    uart0_send_linefeed();
    //}
}
