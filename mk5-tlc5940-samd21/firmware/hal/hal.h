#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <samd21.h>
#include <hal_api.h>


// Enable this design when testing on the SAM R21 Xplained Pro board
// See below for pin-out differences
// #define SAMR21_XPLAINED_PRO


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
// IO pins: (SAMD21E15 in QFN32 package)
//
// PA01     (2)                             LED
// PA02     (3)                             OUT15S Switched light output
// PA04     (5 SERCOM0/PAD0)                TLC5940 SIN
// PA05     (6 SERCOM0/PAD1)                TLC5940 SCLK
// PA06     (7)                             TLC5940 XLAT
// PA07     (8)                             TLC5940 BLANK
// PA09     (12)                            TLC5940 GSCLK
// PA11     (14)                            Push button
// PA16     (17 EXTINT[0])                  Steering input
// PA18     (19 EXTINT[1])                  Throttle input
// PA19     (20 EXTINT[3])                  CH3 input
// PA22     (21 SERCOM3/PAD0, TC4/W[0])     OUT / Tx
// PA23     (22 SERCOM3/PAD1)               Rx
//
//
// PA24     (23)  USB-DM
// PA25     (24)  USB-DP
// PA30     (31)  SWCLK
// PA31     (32)  SWDIO
// RESET    (26)
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
// PA01             32.768 kHz crystal
// PA02     A0      OUT15S Switched light output
// PA04     A3      TLC5940 SIN
// PA05     A4      TLC5940 SCLK
// PA06     D8      TLC5940 XLAT
// PA07     D9      TLC5940 BLANK
// PA09     D3      TLC5940 GSCLK
// PA11     D0      Push button
// PA16     D11     Steering input
// PA18     D10     Throttle input
// PA19     D12     CH3 input
// PA22     SDA     OUT / Tx
// PA23     SCL     Rx
//
// PA24     (23)    USB-DM
// PA25     (24)    USB-DP
//
// PA17     D13     LED
//
// ****************************************************************************

#ifndef SAMR21_XPLAINED_PRO

static const HAL_GPIO_T HAL_GPIO_RX = { .group = 0, .pin = 23, .mux = PORT_PMUX_PMUXE_D_Val, .pad = 1 };
static const HAL_GPIO_T HAL_GPIO_TX = { .group = 0, .pin = 22, .mux = PORT_PMUX_PMUXE_D_Val, .pad = 0 };
static const HAL_GPIO_T HAL_GPIO_OUT = { .group = 0, .pin = 22, .mux = PORT_PMUX_PMUXE_E_Val };
static const HAL_GPIO_T HAL_GPIO_ST = { .group = 0, .pin = 16, .mux = PORT_PMUX_PMUXE_A_Val };
static const HAL_GPIO_T HAL_GPIO_TH = { .group = 0, .pin = 18, .mux = PORT_PMUX_PMUXE_A_Val };
static const HAL_GPIO_T HAL_GPIO_CH3 = { .group = 0, .pin = 19, .mux = PORT_PMUX_PMUXE_A_Val };
static const HAL_GPIO_T HAL_GPIO_SCK = { .group = 0, .pin = 5, .mux = PORT_PMUX_PMUXE_C_Val };
static const HAL_GPIO_T HAL_GPIO_SIN = { .group = 0, .pin = 4, .mux = PORT_PMUX_PMUXE_C_Val };
static const HAL_GPIO_T HAL_GPIO_XLAT = { .group = 0, .pin = 6 };
static const HAL_GPIO_T HAL_GPIO_GSCLK = { .group = 0, .pin = 9 };
static const HAL_GPIO_T HAL_GPIO_BLANK = { .group = 0, .pin = 7 };
static const HAL_GPIO_T HAL_GPIO_PUSH_BUTTON = { .group = 0, .pin = 11 };
static const HAL_GPIO_T HAL_GPIO_SWITCHED_LIGHT_OUTPUT = { .group = 0, .pin = 2 };
static const HAL_GPIO_T HAL_GPIO_LED = { .group = 0, .pin = 1 };
static const HAL_GPIO_T HAL_GPIO_LED2 = { .group = 0, .pin = 17 };
static const HAL_GPIO_T HAL_GPIO_USB_DM = { .group = 0, .pin = 24, .mux = PORT_PMUX_PMUXE_G_Val };
static const HAL_GPIO_T HAL_GPIO_USB_DP = { .group = 0, .pin = 25, .mux = PORT_PMUX_PMUXE_G_Val };

// FIXME not tested
#define UART_SERCOM SERCOM3
#define UART_SERCOM_GCLK_ID SERCOM0_GCLK_ID_CORE
#define UART_SERCOM_APBCMASK PM_APBCMASK_SERCOM3

#define SPI_SERCOM SERCOM0
#define SPI_SERCOM_GCLK_ID SERCOM3_GCLK_ID_CORE
#define SPI_SERCOM_APBCMASK PM_APBCMASK_SERCOM0

#endif


// ****************************************************************************
// SAM R21 Xplained Pro mapping:
//
//
// R21      D21
// -------------
// PA19     PA02     OUT15S Switched light output
// PA23     PA04     TLC5940 SIN
// ????     PA05     TLC5940 SCLK
//          PA06     TLC5940 XLAT
//          PA07     TLC5940 BLANK
// PA14     PA09     TLC5940 GSCLK
// PA15     PA11     Push button
// PA22              OUT (Separate from hard-wired Tx!)
//          PA16     Steering input
//          PA17     Throttle input
// ????     PA19     CH3 input
// PA04     PA23     Rx
// PA05     PA22     Tx
//
// NOTE: SERCOM0 and SERCOM3 swapped between UART and SPI!
//
// ****************************************************************************
#ifdef SAMR21_XPLAINED_PRO

static const HAL_GPIO_T HAL_GPIO_RX = { .group = 0, .pin = 5, .mux = PORT_PMUX_PMUXE_D_Val, .pad = 1 };
static const HAL_GPIO_T HAL_GPIO_TX = { .group = 0, .pin = 4, .mux = PORT_PMUX_PMUXE_D_Val, .pad = 0 };
static const HAL_GPIO_T HAL_GPIO_OUT = { .group = 0, .pin = 22, .mux = PORT_PMUX_PMUXE_E_Val }; // Set GPIO to output TC4/W[0]
static const HAL_GPIO_T HAL_GPIO_ST = { .group = 0, .pin = 16, .mux = PORT_PMUX_PMUXE_A_Val };  // Enable the EIC function on the ST pin (EXTINT0)
static const HAL_GPIO_T HAL_GPIO_TH = { .group = 0, .pin = 17, .mux = PORT_PMUX_PMUXE_A_Val };  // Enable the EIC function on the TH pin (EXTINT1)
static const HAL_GPIO_T HAL_GPIO_CH3 = { .group = 0, .pin = 0 };
static const HAL_GPIO_T HAL_GPIO_SCK = { .group = 0, .pin = 8, .mux = PORT_PMUX_PMUXE_C_Val };
static const HAL_GPIO_T HAL_GPIO_SIN = { .group = 0, .pin = 23, .mux = PORT_PMUX_PMUXE_C_Val };
static const HAL_GPIO_T HAL_GPIO_XLAT = { .group = 0, .pin = 6 };
static const HAL_GPIO_T HAL_GPIO_GSCLK = { .group = 0, .pin = 14 };
static const HAL_GPIO_T HAL_GPIO_BLANK = { .group = 0, .pin = 8 };
static const HAL_GPIO_T HAL_GPIO_PUSH_BUTTON = { .group = 0, .pin = 15 };
static const HAL_GPIO_T HAL_GPIO_SWITCHED_LIGHT_OUTPUT = { .group = 0, .pin = 19 };
static const HAL_GPIO_T HAL_GPIO_LED = { .group = 0, .pin = 19 };
static const HAL_GPIO_T HAL_GPIO_USB_DM = { .group = 0, .pin = 24, .mux = PORT_PMUX_PMUXE_G_Val };
static const HAL_GPIO_T HAL_GPIO_USB_DP = { .group = 0, .pin = 25, .mux = PORT_PMUX_PMUXE_G_Val };

#define UART_SERCOM SERCOM0
#define UART_SERCOM_GCLK_ID SERCOM0_GCLK_ID_CORE
#define UART_SERCOM_APBCMASK PM_APBCMASK_SERCOM0

#define SPI_SERCOM SERCOM3
#define SPI_SERCOM_GCLK_ID SERCOM3_GCLK_ID_CORE
#define SPI_SERCOM_APBCMASK PM_APBCMASK_SERCOM3

#endif


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

