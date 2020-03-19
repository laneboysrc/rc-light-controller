/******************************************************************************

    Application entry point.

    Contains the main loop and the hardware initialization.

******************************************************************************/
#include <stdio.h>
#include <stdbool.h>

#include <globals.h>

#include <hal.h>
// #include <printf.h>


// ****************************************************************************
static void service_systick(void)
{
    ++entropy;
}


// ****************************************************************************
int main(void)
{
    HAL_hardware_init();
    HAL_uart_init(115200);
    // init_printf(STDOUT_UART, HAL_putc);

    // Wait for 100ms to have the supply settle
    while (milliseconds <  100);

    HAL_hardware_init_final();

    while (1) {
        service_systick();
        HAL_service();


        if (HAL_getchar_pending(STDOUT_UART)) {
            uint8_t c = HAL_getchar(STDOUT_UART);

            HAL_putc(STDOUT_USB, c);
        }
    }
}
