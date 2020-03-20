/******************************************************************************

    Application entry point.

    Contains the main loop and the hardware initialization.

******************************************************************************/
#include <stdio.h>
#include <stdbool.h>

#include <globals.h>

#include <hal.h>


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

    // Wait for 100ms to have the supply settle
    while (milliseconds <  100);

    HAL_hardware_init_final();

    while (1) {
        service_systick();
        HAL_service();

        // Transfer up to 16 bytes in one go from USB to UART
        for (uint8_t count = 0; count < 16; count++) {
            uint8_t c;

            if (!HAL_getchar_pending(STDOUT_USB)) {
                break;
            }

            c = HAL_getchar(STDOUT_USB);
            HAL_putc(STDOUT_UART, c);
        }

        // Transfer up to 16 bytes in one go from UART to USB
        for (uint8_t count = 0; count < 16; count++) {
            uint8_t c;
            if (!HAL_getchar_pending(STDOUT_UART)) {
                break;
            }

            c = HAL_getchar(STDOUT_UART);
            HAL_putc(STDOUT_USB, c);
        }
    }
}
