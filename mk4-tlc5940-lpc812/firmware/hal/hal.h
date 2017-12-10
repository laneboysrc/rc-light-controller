#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <LPC8xx.h>
#include <hal_api.h>

#define HAL_SYSTEM_CLOCK 12000000
#define HAL_NUMBER_OF_PERSISTENT_ELEMENTS 16


// ****************************************************************************
// IO pins: (LPC812 in TSSOP16 package)
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
#define GPIO_BIT_SWITCHED_LIGHT_OUTPUT 9
#define GPIO_BIT_SCK 2
#define GPIO_BIT_SIN 7
#define GPIO_BIT_XLAT 3
#define GPIO_BIT_GSCLK 1
#define GPIO_BIT_BLANK 6

#define GPIO_IOCON_ST LPC_IOCON->PIO0_0
#define GPIO_IOCON_TH LPC_IOCON->PIO0_4
#define GPIO_IOCON_CH3 LPC_IOCON->PIO0_13

#define DECLARE_GPIO(name, bit)                                             \
                                                                            \
    static inline void HAL_gpio_##name##_in(void)                           \
    {                                                                       \
        LPC_GPIO_PORT->DIR0 &= ~(1 << bit);                                 \
    }                                                                       \
                                                                            \
    static inline void HAL_gpio_##name##_out(void)                          \
    {                                                                       \
        LPC_GPIO_PORT->DIR0 |= (1 << bit);                                  \
    }                                                                       \
                                                                            \
    static inline void HAL_gpio_##name##_write(bool value)                  \
    {                                                                       \
        LPC_GPIO_PORT->W0[bit] = value;                                     \
    }                                                                       \
                                                                            \
    static inline bool HAL_gpio_##name##_read(void)                         \
    {                                                                       \
        return LPC_GPIO_PORT->W0[bit];                                      \
    }                                                                       \
                                                                            \
    static inline void HAL_gpio_##name##_set(void)                          \
    {                                                                       \
        LPC_GPIO_PORT->W0[bit] = 1;                                         \
    }                                                                       \
                                                                            \
    static inline void HAL_gpio_##name##_clear(void)                        \
    {                                                                       \
        LPC_GPIO_PORT->W0[bit] = 0;                                         \
    }                                                                       \
                                                                            \
    static inline void HAL_gpio_##name##_toggle(void)                       \
    {                                                                       \
        LPC_GPIO_PORT->NOT0 = 1 << bit;                                     \
    }                                                                       \


DECLARE_GPIO(gsclk, GPIO_BIT_GSCLK)
DECLARE_GPIO(blank, GPIO_BIT_BLANK)
DECLARE_GPIO(ch3, GPIO_BIT_CH3)
DECLARE_GPIO(switched_light_output, GPIO_BIT_SWITCHED_LIGHT_OUTPUT)
