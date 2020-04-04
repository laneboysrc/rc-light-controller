/******************************************************************************

    Application entry point.

    Contains the main loop and the hardware initialization.

******************************************************************************/
#include <stdio.h>
#include <stdbool.h>

#include <globals.h>

#include <hal.h>

#define WATCHDOG_TIMEOUT_MS (2000)

static bool watchdog_enabled = false;
static uint32_t watchdog_expiry_ms;


// ****************************************************************************
void watchdog_reset(void)
{
    watchdog_expiry_ms = milliseconds + WATCHDOG_TIMEOUT_MS;
    watchdog_enabled = true;
}


// ****************************************************************************
static void watchdog_service(void)
{
    if (!watchdog_enabled) {
        return;
    }

    if (milliseconds < watchdog_expiry_ms) {
        return;
    }

    watchdog_enabled = false;

    // Watchdog expired: turn the power to the light controller off
    HAL_gpio_set(HAL_GPIO_POWER_ENABLE);
    HAL_gpio_out(HAL_GPIO_POWER_ENABLE);

    // Turn the LEDs off
    HAL_gpio_out(HAL_GPIO_LED_OK);
    HAL_gpio_out(HAL_GPIO_LED_BUSY);
    HAL_gpio_out(HAL_GPIO_LED_ERROR);
    HAL_gpio_clear(HAL_GPIO_LED_OK);
    HAL_gpio_clear(HAL_GPIO_LED_BUSY);
    HAL_gpio_clear(HAL_GPIO_LED_ERROR);

    // Switch the UART TX and RX to GPIO output  and set it to low, so that
    // we don't power the light controller via the ST/RX pin!
    HAL_gpio_clear(HAL_GPIO_TXIO);
    HAL_gpio_out(HAL_GPIO_TXIO);
    HAL_gpio_pmuxen(HAL_GPIO_TXIO);
    HAL_gpio_clear(HAL_GPIO_RXIO);
    HAL_gpio_out(HAL_GPIO_RXIO);
    HAL_gpio_pmuxen(HAL_GPIO_RXIO);

    // Also switch CH3 and OUT/ISP to low
    HAL_gpio_clear(HAL_GPIO_CH3);
    HAL_gpio_out(HAL_GPIO_CH3);
    HAL_gpio_clear(HAL_GPIO_OUT_ISP);
    HAL_gpio_out(HAL_GPIO_OUT_ISP);

    // Short the power lines of the light controller to discharge its capacitors
    HAL_gpio_set(HAL_GPIO_POWER_SHORT);
    HAL_gpio_out(HAL_GPIO_POWER_SHORT);
}


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
        watchdog_service();
        HAL_service();

        // Transfer up to BUF_SIZE bytes in one go from USB to UART
        for (uint8_t count = 0; count < BUF_SIZE; count++) {
            uint8_t c;

            if (!HAL_getchar_pending(STDOUT_USB)) {
                break;
            }

            c = HAL_getchar(STDOUT_USB);
            HAL_putc(STDOUT_UART, c);
        }

        // Transfer up to BUF_SIZE bytes in one go from UART to USB
        for (uint8_t count = 0; count < BUF_SIZE; count++) {
            uint8_t c;
            if (!HAL_getchar_pending(STDOUT_UART)) {
                break;
            }

            c = HAL_getchar(STDOUT_UART);
            HAL_putc(STDOUT_USB, c);
        }
    }
}
