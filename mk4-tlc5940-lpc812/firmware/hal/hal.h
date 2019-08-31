#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <LPC8xx.h>
#include <hal_api.h>

#define HAL_SYSTEM_CLOCK 12000000
#define HAL_NUMBER_OF_PERSISTENT_ELEMENTS 16

typedef struct {
    uint8_t pin;
    volatile uint32_t *iocon;
} HAL_GPIO_T;

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
// PIO0_8   (11, XTALIN)        AUX2 input
// PIO0_9   (10, XTALOUT)       Switched light output (for driving a load via a MOSFET)
// PIO0_10  (8,  Open drain)    NC
// PIO0_11  (7,  Open drain)    AUX3 input
// PIO0_12  (2,  ISP-entry)     OUT / ISP
// PIO0_13  (1)                 AUX input
//
// GND      (13)
// 3.3V     (12)
// ****************************************************************************


static const HAL_GPIO_T HAL_GPIO_ST = { .pin = 0, .iocon = &LPC_IOCON->PIO0_0 };
static const HAL_GPIO_T HAL_GPIO_TH = { .pin = 4, .iocon = &LPC_IOCON->PIO0_4 };
static const HAL_GPIO_T HAL_GPIO_AUX = { .pin = 13, .iocon = &LPC_IOCON->PIO0_13 };
static const HAL_GPIO_T HAL_GPIO_AUX2 = { .pin = 8, .iocon = &LPC_IOCON->PIO0_8 };
static const HAL_GPIO_T HAL_GPIO_AUX3 = { .pin = 11, .iocon = &LPC_IOCON->PIO0_11 };
static const HAL_GPIO_T HAL_GPIO_OUT = { .pin = 12 };
static const HAL_GPIO_T HAL_GPIO_SWITCHED_LIGHT_OUTPUT = { .pin = 9 };
static const HAL_GPIO_T HAL_GPIO_SCK = { .pin = 2 };
static const HAL_GPIO_T HAL_GPIO_SIN = { .pin = 7 };
static const HAL_GPIO_T HAL_GPIO_XLAT = { .pin = 3 };
static const HAL_GPIO_T HAL_GPIO_GSCLK = { .pin = 1 };
static const HAL_GPIO_T HAL_GPIO_BLANK = { .pin = 6 };

static const HAL_GPIO_T HAL_GPIO_PIN10 = { .pin = 10 };
static const HAL_GPIO_T HAL_GPIO_PIN11 = { .pin = 11 };


static inline void HAL_gpio_in(const HAL_GPIO_T gpio)
{
    LPC_GPIO_PORT->DIR0 &= ~(1 << gpio.pin);
}

static inline void HAL_gpio_out(const HAL_GPIO_T gpio)
{
    LPC_GPIO_PORT->DIR0 |= (1 << gpio.pin);
}

static inline void HAL_gpio_write(const HAL_GPIO_T gpio, bool value)
{
    LPC_GPIO_PORT->W0[gpio.pin] = value;
}

static inline bool HAL_gpio_read(const HAL_GPIO_T gpio)
{
    return LPC_GPIO_PORT->W0[gpio.pin];
}

static inline void HAL_gpio_set(const HAL_GPIO_T gpio)
{
    LPC_GPIO_PORT->W0[gpio.pin] = 1;
}

static inline void HAL_gpio_clear(const HAL_GPIO_T gpio)
{
    LPC_GPIO_PORT->W0[gpio.pin] = 0;
}

static inline void HAL_gpio_toggle(const HAL_GPIO_T gpio)
{
    LPC_GPIO_PORT->NOT0 = 1 << gpio.pin;
}

// NOTE: HAL_gpio_glitch_filter is specific to the LPC812 processor
static inline void HAL_gpio_glitch_filter(const HAL_GPIO_T gpio)
{
    *gpio.iocon |= (1 << 5) |           // Enable Hysteresis
                   (0x1 << 13) |        // Glitch filter 1
                   (0x1 << 11);         // Reject 1 clock cycle of glitch filter
}

