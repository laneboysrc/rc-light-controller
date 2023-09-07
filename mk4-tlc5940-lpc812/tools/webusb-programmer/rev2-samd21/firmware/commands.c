#include <stdint.h>
#include <stdbool.h>

#include <globals.h>

#include <hal.h>


#define CMD_DUT_POWER_OFF (10)
#define CMD_DUT_POWER_ON (11)
#define CMD_OUT_ISP_LOW (20)
#define CMD_OUT_ISP_HIGH (21)
#define CMD_OUT_ISP_TRISTATE (22)
#define CMD_CH3_LOW (23)
#define CMD_CH3_HIGH (24)
#define CMD_CH3_TRISTATE (25)
#define CMD_BAUDRATE_38400 (30)
#define CMD_BAUDRATE_115200 (31)
#define CMD_LED_OK_OFF (40)
#define CMD_LED_OK_ON (41)
#define CMD_LED_BUSY_OFF (42)
#define CMD_LED_BUSY_ON (43)
#define CMD_LED_ERROR_OFF (44)
#define CMD_LED_ERROR_ON (45)
#define CMD_PING (99)

// ****************************************************************************
bool command_handler(uint16_t wValue)
{
    switch(wValue) {
        case CMD_DUT_POWER_ON:
            HAL_gpio_clear(HAL_GPIO_POWER_SHORT);
            HAL_gpio_clear(HAL_GPIO_POWER_ENABLE);
            // Switch the TX and RX back to UART
            HAL_gpio_pmuxen(HAL_GPIO_TX);
            HAL_gpio_in(HAL_GPIO_RX);
            HAL_gpio_pmuxen(HAL_GPIO_RX);
            break;

        case CMD_DUT_POWER_OFF:
            HAL_gpio_set(HAL_GPIO_POWER_ENABLE);
            // Switch the UART TX and RX to GPIO output  and set it to low, so that
            // we don't power the light controller via the ST/RX pin!
            HAL_gpio_clear(HAL_GPIO_TX);
            HAL_gpio_out(HAL_GPIO_TX);
            HAL_gpio_pmuxdis(HAL_GPIO_TX);
            HAL_gpio_clear(HAL_GPIO_RX);
            HAL_gpio_out(HAL_GPIO_RX);
            HAL_gpio_pmuxdis(HAL_GPIO_RX);

            HAL_gpio_set(HAL_GPIO_POWER_SHORT);
            break;

        case CMD_LED_OK_ON:
            HAL_gpio_set(HAL_GPIO_LED_OK);
            break;

        case CMD_LED_OK_OFF:
            HAL_gpio_clear(HAL_GPIO_LED_OK);
            break;

        case CMD_LED_BUSY_ON:
            HAL_gpio_set(HAL_GPIO_LED_BUSY);
            break;

        case CMD_LED_BUSY_OFF:
            HAL_gpio_clear(HAL_GPIO_LED_BUSY);
            break;

        case CMD_LED_ERROR_ON:
            HAL_gpio_set(HAL_GPIO_LED_ERROR);
            break;

        case CMD_LED_ERROR_OFF:
            HAL_gpio_clear(HAL_GPIO_LED_ERROR);
            break;

        case CMD_OUT_ISP_LOW:
            HAL_gpio_clear(HAL_GPIO_OUT_ISP);
            HAL_gpio_out(HAL_GPIO_OUT_ISP);
            break;

        case CMD_OUT_ISP_HIGH:
            HAL_gpio_set(HAL_GPIO_OUT_ISP);
            HAL_gpio_out(HAL_GPIO_OUT_ISP);
            break;

        case CMD_OUT_ISP_TRISTATE:
            HAL_gpio_in(HAL_GPIO_OUT_ISP);
            break;

        case CMD_CH3_LOW:
            HAL_gpio_clear(HAL_GPIO_CH3);
            HAL_gpio_out(HAL_GPIO_CH3);
            break;

        case CMD_CH3_HIGH:
            HAL_gpio_set(HAL_GPIO_CH3);
            HAL_gpio_out(HAL_GPIO_CH3);
            break;

        case CMD_CH3_TRISTATE:
            HAL_gpio_in(HAL_GPIO_CH3);
            break;

        case CMD_BAUDRATE_38400:
            HAL_uart_set_baudrate(38400);
            break;

        case CMD_BAUDRATE_115200:
            HAL_uart_set_baudrate(115200);
            break;

        case CMD_PING:
            break;

        default:
            return false;
    }

    return true;
}
