#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <samd21.h>
#include <hal_api.h>


// One flash page is 64 bytes, so 16 * 4 words
#define HAL_NUMBER_OF_PERSISTENT_ELEMENTS 16


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
// PA01     (2)
// PA02     (3)
// PA04     (5 SERCOM0/PAD0)                LED OK
// PA05     (6 SERCOM0/PAD1)                LED Busy
// PA06     (7)                             LED Error
// PA07     (8)                             Light Controller supply enable
// PA09     (12)
// PA11     (14)
// PA18     (19 SERCOM3/PAD2, EXTINT[1])
// PA19     (20 EXTINT[3])
// PA22     (21 SERCOM3/PAD0, TC4/W[0])     TX
// PA23     (22 EXTINT[7], SERCOM3/PAD1)    RX
// PA24     (23 USB-DM)                     USB-DM
// PA25     (24 USB-DP)                     USB-DP
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
//
// Arduino Zero (and Protoneer Nano-ARM) pin mapping:
//
// PA01             [32.768 kHz crystal]
// PA02     A0
// PA04     A3      LED OK
// PA05     A4      LED Busy
// PA06     D8      LED Error
// PA07     D9      Light Controller supply enable
// PA09     D3
// PA11     D0
// PA17     D13     LED
// PA18     D10
// PA19     D12
// PA22     SDA     TX
// PA23     SCL     RX
// PA24     USB-DM
// PA25     USB-DP
//
// ****************************************************************************

// ST and RX share the same pin, but have configurable functionality
static const HAL_GPIO_T HAL_GPIO_ST = { .group = 0, .pin = 23, .mux = PORT_PMUX_PMUXE_A_Val };
static const HAL_GPIO_T HAL_GPIO_RX = { .group = 0, .pin = 23, .mux = PORT_PMUX_PMUXE_C_Val, .rxpo = 1 };

// TH and TX share the same pin in some configurations, but have configurable functionality
static const HAL_GPIO_T HAL_GPIO_TH = { .group = 0, .pin = 18, .mux = PORT_PMUX_PMUXE_A_Val };
static const HAL_GPIO_T HAL_GPIO_TX_ON_TH = { .group = 0, .pin = 18, .mux = PORT_PMUX_PMUXE_D_Val, .txpo = 1 };

static const HAL_GPIO_T HAL_GPIO_CH3 = { .group = 0, .pin = 19, .mux = PORT_PMUX_PMUXE_A_Val };

// OUT and TX share the same pin, but have configurable functionality
static const HAL_GPIO_T HAL_GPIO_OUT = { .group = 0, .pin = 22, .mux = PORT_PMUX_PMUXE_E_Val };
static const HAL_GPIO_T HAL_GPIO_TX_ON_OUT = { .group = 0, .pin = 22, .mux = PORT_PMUX_PMUXE_C_Val, .txpo = 0 };

static const HAL_GPIO_T HAL_GPIO_SCK = { .group = 0, .pin = 5, .mux = PORT_PMUX_PMUXE_D_Val };
static const HAL_GPIO_T HAL_GPIO_SIN = { .group = 0, .pin = 4, .mux = PORT_PMUX_PMUXE_D_Val };
static const HAL_GPIO_T HAL_GPIO_XLAT = { .group = 0, .pin = 6 };
static const HAL_GPIO_T HAL_GPIO_GSCLK = { .group = 0, .pin = 9 };
static const HAL_GPIO_T HAL_GPIO_BLANK = { .group = 0, .pin = 7 };
static const HAL_GPIO_T HAL_GPIO_PUSH_BUTTON = { .group = 0, .pin = 11 };
static const HAL_GPIO_T HAL_GPIO_SWITCHED_LIGHT_OUTPUT = { .group = 0, .pin = 2 };
static const HAL_GPIO_T HAL_GPIO_LED = { .group = 0, .pin = 1 };
static const HAL_GPIO_T HAL_GPIO_LED2 = { .group = 0, .pin = 17 };
static const HAL_GPIO_T HAL_GPIO_USB_DM = { .group = 0, .pin = 24, .mux = PORT_PMUX_PMUXE_G_Val };
static const HAL_GPIO_T HAL_GPIO_USB_DP = { .group = 0, .pin = 25, .mux = PORT_PMUX_PMUXE_G_Val };

#define UART_SERCOM SERCOM3
#define UART_SERCOM_GCLK_ID SERCOM3_GCLK_ID_CORE
#define UART_SERCOM_APBCMASK PM_APBCMASK_SERCOM3
#define UART_SERCOM_IRQN SERCOM3_IRQn
#define UART_SERCOM_HANDLER SERCOM3_Handler

#define SPI_SERCOM SERCOM0
#define SPI_SERCOM_GCLK_ID SERCOM0_GCLK_ID_CORE
#define SPI_SERCOM_APBCMASK PM_APBCMASK_SERCOM0



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

