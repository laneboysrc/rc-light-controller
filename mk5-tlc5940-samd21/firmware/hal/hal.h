#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <samd21.h>
#include <hal_api.h>

#define HAL_NUMBER_OF_PERSISTENT_ELEMENTS 16


// ****************************************************************************
// IO pins: (SAMD21E15 in QFN32 package)
//
// FIXME!!!!!!!!!!!!!!!!
// FIXME!!!!!!!!!!!!!!!!
// FIXME!!!!!!!!!!!!!!!!
//
// PIO0_0   (16, TDO, ISP-Rx)   Steering input / Rx
// PIO0_1   (9,  TDI)           TLC5940 GSCLK
// PIO0_2   (6,  TMS, SWDIO)    TLC5940 SCLK
// PIO0_3   (5,  TCK, SWCLK)    TLC5940 XLAT
// PIO0_4   (4,  TRST, ISP-Tx)  Throttle input / Tx
// PIO0_5   (3,  RESET)         NC (test point)
// PIO0_6   (15)                TLC5940 BLANK
// PIO0_7   (14)                TLC5940 SIN
// PIO0_8   (11, XTALIN)        NC
// PIO0_9   (10, XTALOUT)       Switched light output (for driving a load via a MOSFET)
// PIO0_10  (8,  Open drain)    NC
// PIO0_11  (7,  Open drain)    NC
// PIO0_12  (2,  ISP-entry)     OUT / ISP
// PIO0_13  (1)                 CH3 input
//
// GND      (13)
// 3.3V     (12)
// ****************************************************************************

#define GPIO_BIT_ST 0
#define GPIO_BIT_TH 4
#define GPIO_BIT_CH3 13
#define GPIO_BIT_OUT 12
#define GPIO_BIT_SWITCHED_LIGHT_OUTPUT 19
#define GPIO_BIT_SCK 22
#define GPIO_BIT_SIN 23
#define GPIO_BIT_XLAT 28
#define GPIO_BIT_GSCLK 1
#define GPIO_BIT_BLANK 6

#define GPIO_PORTA 0
#define GPIO_PORTB 1
#define GPIO_PORTC 2

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

