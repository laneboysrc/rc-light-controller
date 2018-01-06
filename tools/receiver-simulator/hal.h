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
    uint8_t pad;
} HAL_GPIO_T;



// ****************************************************************************
// SAM R21 Xplained Pro mapping:
//
// PB02     ST servo in (AIN10)
// PB03     TH servo in (AIN11)
// PA18     ST servo out (TC3/W0)
// PA22     TH servo out (TC4/W0)
// PA04     Rx (SERCOM0)
// PA05     Tx (SERCOM0)
//
// ****************************************************************************
static const HAL_GPIO_T HAL_GPIO_RX = { .group = 0, .pin = 5, .mux = PORT_PMUX_PMUXE_D_Val, .pad = 1 };
static const HAL_GPIO_T HAL_GPIO_TX = { .group = 0, .pin = 4, .mux = PORT_PMUX_PMUXE_D_Val, .pad = 0 };
static const HAL_GPIO_T HAL_GPIO_ST = { .group = 0, .pin = 18, .mux = PORT_PMUX_PMUXE_E_Val }; // Set GPIO to output TC3/W[0]
static const HAL_GPIO_T HAL_GPIO_TH = { .group = 0, .pin = 22, .mux = PORT_PMUX_PMUXE_E_Val }; // Set GPIO to output TC4/W[0]
static const HAL_GPIO_T HAL_GPIO_ST_IN = { .group = 1, .pin = 2, .mux = PORT_PMUX_PMUXE_B_Val };
static const HAL_GPIO_T HAL_GPIO_TH_IN = { .group = 1, .pin = 3, .mux = PORT_PMUX_PMUXE_B_Val };

#define UART_SERCOM SERCOM0
#define UART_SERCOM_GCLK_ID SERCOM0_GCLK_ID_CORE
#define UART_SERCOM_APBCMASK PM_APBCMASK_SERCOM0


static inline void HAL_gpio_in(const HAL_GPIO_T gpio)
{
    PORT->Group[gpio.group].DIRCLR.reg = 1 << gpio.pin;
    PORT->Group[gpio.group].PINCFG[gpio.pin].reg |= PORT_PINCFG_INEN;
    PORT->Group[gpio.group].PINCFG[gpio.pin].reg &= ~PORT_PINCFG_PULLEN;
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

