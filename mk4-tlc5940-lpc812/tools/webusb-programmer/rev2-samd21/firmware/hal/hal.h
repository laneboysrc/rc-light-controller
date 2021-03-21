#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <samd21.h>
#include <hal_api.h>



extern bool start_bootloader;

typedef struct {
    uint8_t group;
    uint8_t pin;
    uint8_t mux;
    uint8_t txpo;
    uint8_t rxpo;
} HAL_GPIO_T;


// ****************************************************************************
// IO pins: (SAMD21E15 in QFN32 package)
//
// PA03     (4)                             Light Controller supply HAL_GPIO_POWER_ENABLE
// PA04     (5 SERCOM0/PAD0)                LED OK
// PA05     (6 SERCOM0/PAD1)                LED Busy
// PA06     (7)                             LED Error
// PA07     (8)                             Light Controller supply short to GND
// PA18     (19)                            CH3
// PA22     (21 SERCOM3/PAD0, TC4/W[0])     TX
// PA23     (22 EXTINT[7], SERCOM3/PAD1)    RX
// PA24     (23 USB-DM)                     USB-DM
// PA25     (24 USB-DP)                     USB-DP
// PA27     (25)                            ISP
// PA30     (31 SWCLK)                      SWCLK
// PA31     (32 SWDIO)                      SWDIO
// RESET    (26)                            Reset
//
// GND      (10)
// GND      (28)
// VDDCORE  (29)
// 3.3V     (30)  VDDIN
// 3.3V     (29)  VDDANA
//


static const HAL_GPIO_T HAL_GPIO_LED_OK = { .group = 0, .pin = 4 };
static const HAL_GPIO_T HAL_GPIO_LED_BUSY = { .group = 0, .pin = 5 };
static const HAL_GPIO_T HAL_GPIO_LED_ERROR = { .group = 0, .pin = 6 };

static const HAL_GPIO_T HAL_GPIO_POWER_ENABLE = { .group = 0, .pin = 3 };
static const HAL_GPIO_T HAL_GPIO_POWER_SHORT = { .group = 0, .pin = 7 };


static const HAL_GPIO_T HAL_GPIO_TX = { .group = 0, .pin = 22, .mux = PORT_PMUX_PMUXE_C_Val, .txpo = 0 };
static const HAL_GPIO_T HAL_GPIO_RX = { .group = 0, .pin = 23, .mux = PORT_PMUX_PMUXE_C_Val, .rxpo = 1 };

static const HAL_GPIO_T HAL_GPIO_CH3 = { .group = 0, .pin = 18 };

static const HAL_GPIO_T HAL_GPIO_USB_DM = { .group = 0, .pin = 24, .mux = PORT_PMUX_PMUXE_G_Val };
static const HAL_GPIO_T HAL_GPIO_USB_DP = { .group = 0, .pin = 25, .mux = PORT_PMUX_PMUXE_G_Val };

static const HAL_GPIO_T HAL_GPIO_OUT_ISP = { .group = 0, .pin = 27 };


#define UART_SERCOM SERCOM3
#define UART_SERCOM_GCLK_ID SERCOM3_GCLK_ID_CORE
#define UART_SERCOM_APBCMASK PM_APBCMASK_SERCOM3
#define UART_SERCOM_IRQN SERCOM3_IRQn
#define UART_SERCOM_HANDLER SERCOM3_Handler



static inline void HAL_gpio_in(const HAL_GPIO_T gpio)
{
    PORT->Group[gpio.group].DIRCLR.reg = 1 << gpio.pin;
    PORT->Group[gpio.group].PINCFG[gpio.pin].reg |= PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
    PORT->Group[gpio.group].OUTSET.reg = 1 << gpio.pin;
}

static inline void HAL_gpio_out(const HAL_GPIO_T gpio)
{
    PORT->Group[gpio.group].DIRSET.reg = 1 << gpio.pin;
    PORT->Group[gpio.group].PINCFG[gpio.pin].reg |= PORT_PINCFG_INEN;
}

static inline void HAL_gpio_write(const HAL_GPIO_T gpio, bool value)
{
    if (value) {
        PORT->Group[gpio.group].OUTSET.reg = 1 << gpio.pin;
    }
    else {
        PORT->Group[gpio.group].OUTCLR.reg = 1 << gpio.pin;
    }
}

static inline bool HAL_gpio_read(const HAL_GPIO_T gpio)
{
    return (PORT->Group[gpio.group].IN.reg & (1 << gpio.pin)) != 0;
}

static inline void HAL_gpio_set(const HAL_GPIO_T gpio)
{
    PORT->Group[gpio.group].OUTSET.reg = 1 << gpio.pin;
}

static inline void HAL_gpio_clear(const HAL_GPIO_T gpio)
{
    PORT->Group[gpio.group].OUTCLR.reg = 1 << gpio.pin;
}

static inline void HAL_gpio_toggle(const HAL_GPIO_T gpio)
{
    PORT->Group[gpio.group].OUTTGL.reg = 1 << gpio.pin;
}

// NOTE: HAL_gpio_pmuxen is specific to the ATSAMD21 processor
static inline void HAL_gpio_pmuxen(const HAL_GPIO_T gpio)
{
    if (gpio.pin & 1) {
        PORT->Group[gpio.group].PMUX[gpio.pin >> 1].bit.PMUXO = gpio.mux;
    }
    else {
        PORT->Group[gpio.group].PMUX[gpio.pin >> 1].bit.PMUXE = gpio.mux;
    }
    PORT->Group[gpio.group].PINCFG[gpio.pin].reg |= PORT_PINCFG_PMUXEN;
}

static inline void HAL_gpio_pmuxdis(const HAL_GPIO_T gpio)
{
    PORT->Group[gpio.group].PINCFG[gpio.pin].reg &= ~PORT_PINCFG_PMUXEN;
}
