#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <samd21.h>
#include <hal_api.h>


// Enable this design when testing on the SAM R21 Xplained Pro board
// See below for pin-out differences
#define SAMR21_XPLAINED_PRO


// One flash page is 64 bytes, so 16 * 4 words
#define HAL_NUMBER_OF_PERSISTENT_ELEMENTS 16


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
// PA17     (18 EXTINT[1])                  Throttle input
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
// ****************************************************************************
#define GPIO_PORTA 0
#define GPIO_PORTB 0

#ifndef SAMR21_XPLAINED_PRO
#define GPIO_BIT_RX 23
#define GPIO_BIT_OUT_TX 22
#define GPIO_BIT_ST 16
#define GPIO_BIT_TH 17
#define GPIO_BIT_CH3 19
#define GPIO_BIT_SCK 5
#define GPIO_BIT_SIN 4
#define GPIO_BIT_XLAT 6
#define GPIO_BIT_GSCLK 9
#define GPIO_BIT_BLANK 7
#define GPIO_BIT_PUSH_BUTTON 11
#define GPIO_BIT_SWITCHED_LIGHT_OUTPUT 2
#define GPIO_BIT_LED 1
#define GPIO_BIT_USB_DM 24
#define GPIO_BIT_USB_DP 25
#endif

// ****************************************************************************
// Arduino MKRZero pin mapping:
//
// PA02     A0      OUT15S Switched light output
// PA04     A3      TLC5940 SIN
// PA05     A4      TLC5940 SCLK
// PA06     A5      TLC5940 XLAT
// PA07     A6      TLC5940 BLANK
// PA09     D12     TLC5940 GSCLK
// PA11     D3      Push button
// PA16     D9      Steering input
// PA17     D9      Throttle input
// PA19     D10     CH3 input
// PA22     D0      OUT / Tx
// PA23     D1      Rx
//
// PB08             LED
//
// ****************************************************************************

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
#define GPIO_BIT_RX 5
#define GPIO_BIT_TX 4
#define GPIO_BIT_OUT 22
#define GPIO_BIT_ST 16
#define GPIO_BIT_TH 17
#define GPIO_BIT_CH3 0
#define GPIO_BIT_SCK 8
#define GPIO_BIT_SIN 23
#define GPIO_BIT_XLAT 6
#define GPIO_BIT_GSCLK 14
#define GPIO_BIT_BLANK 8
#define GPIO_BIT_PUSH_BUTTON 15
#define GPIO_BIT_SWITCHED_LIGHT_OUTPUT 19
#define GPIO_BIT_LED 19
#define GPIO_BIT_USB_DM 24
#define GPIO_BIT_USB_DP 25
#endif



#define DECLARE_GPIO(name, port, pin)                                       \
                                                                            \
    static inline void HAL_gpio_##name##_in(void)                           \
    {                                                                       \
        PORT->Group[port].DIRCLR.reg = (1 << pin);                          \
        PORT->Group[port].PINCFG[pin].reg |= PORT_PINCFG_INEN;              \
        PORT->Group[port].PINCFG[pin].reg &= ~PORT_PINCFG_PULLEN;           \
    }                                                                       \
                                                                            \
    static inline void HAL_gpio_##name##_out(void)                          \
    {                                                                       \
        PORT->Group[port].DIRSET.reg = 1 << pin;                            \
        PORT->Group[port].PINCFG[pin].reg |= PORT_PINCFG_INEN;              \
    }                                                                       \
                                                                            \
    static inline void HAL_gpio_##name##_write(bool value)                  \
    {                                                                       \
        if (value) {                                                        \
            PORT->Group[port].OUTSET.reg = 1 << pin;                        \
        }                                                                   \
        else {                                                              \
            PORT->Group[port].OUTCLR.reg = 1 << pin;                        \
        }                                                                   \
    }                                                                       \
                                                                            \
    static inline bool HAL_gpio_##name##_read(void)                         \
    {                                                                       \
        return (PORT->Group[port].IN.reg & (1 << pin)) != 0;                \
    }                                                                       \
                                                                            \
    static inline void HAL_gpio_##name##_set(void)                          \
    {                                                                       \
        PORT->Group[port].OUTSET.reg = 1 << pin;                            \
    }                                                                       \
                                                                            \
    static inline void HAL_gpio_##name##_clear(void)                        \
    {                                                                       \
        PORT->Group[port].OUTCLR.reg = 1 << pin;                            \
    }                                                                       \
                                                                            \
    static inline void HAL_gpio_##name##_toggle(void)                       \
    {                                                                       \
        PORT->Group[port].OUTTGL.reg = 1 << pin;                            \
    }                                                                       \
                                                                            \
    static inline void HAL_gpio_##name##_pmuxen(int pmux)                   \
    {                                                                       \
        PORT->Group[port].PINCFG[pin].reg |= PORT_PINCFG_PMUXEN;            \
        if (pin & 1) {                                                      \
            PORT->Group[port].PMUX[pin >> 1].bit.PMUXO = pmux;              \
        }                                                                   \
        else {                                                              \
            PORT->Group[port].PMUX[pin >> 1].bit.PMUXE = pmux;              \
        }                                                                   \
    }

// NOTE: HAL_gpio_XXX_pmuxen is specific to the ATSAMD21 processor


DECLARE_GPIO(gsclk, GPIO_PORTA, GPIO_BIT_GSCLK)
DECLARE_GPIO(blank, GPIO_PORTA, GPIO_BIT_BLANK)
DECLARE_GPIO(ch3, GPIO_PORTA, GPIO_BIT_CH3)
DECLARE_GPIO(switched_light_output, GPIO_PORTA, GPIO_BIT_SWITCHED_LIGHT_OUTPUT)

