/******************************************************************************

    Application entry point.

    Contains the main loop and the hardware initialization.

******************************************************************************/
#include <stdio.h>
#include <stdbool.h>

#include <globals.h>

#include <hal.h>
#include <printf.h>


// ****************************************************************************
static void service_systick(void)
{
    ++entropy;
}


// ****************************************************************************
int main(void)
{
    HAL_hardware_init(false, false, true);
    HAL_uart_init(115200);
    init_printf(STDOUT, HAL_putc);

    // Wait for 100ms to have the supply settle down before initializing the
    // rest of the system. This is especially important for the TLC5940,
    // which misbehaves (certain LEDs don't work) when being addressed before
    // power is stable.
    //
    // It is also important in terms of servo reader function, because some
    // RC electronics like the Hobbywing Quicrun 1080 are outputing serial
    // data after startup, which interferes with the initialization procedure
    while (milliseconds <  100);

    HAL_hardware_init_final();

    fprintf(STDOUT_DEBUG, "\n\n**********\nWebUSB programmer\n");

    while (1) {
        service_systick();
        HAL_service();
    }
}
