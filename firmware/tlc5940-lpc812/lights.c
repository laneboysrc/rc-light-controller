/******************************************************************************
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include <globals.h>

static uint16_t light_mode;

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
    ;
}