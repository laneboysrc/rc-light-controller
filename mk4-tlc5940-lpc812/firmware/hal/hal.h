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
// GND      (13)
// 3.3V     (12)
//
//
// Mk4P IO pins: (LPC812 in TSSOP20 package)
//
// PIO0_0   (19, TDO, ISP-Rx)   Steering input / Rx
// PIO0_1   (12,  TDI)          TLC5940 GSCLK
// PIO0_2   (7,  TMS, SWDIO)    TLC5940 SCLK
// PIO0_3   (6,  TCK, SWCLK)    TLC5940 XLAT
// PIO0_4   (5,  TRST, ISP-Tx)  Throttle input / Tx
// PIO0_5   (4,  RESET)         NC
// PIO0_6   (18)                TLC5940 BLANK
// PIO0_7   (17)                TLC5940 SIN
// PIO0_8   (14, XTALIN)        AUX2 input
// PIO0_9   (13, XTALOUT)       Switched light output (for driving a load via a MOSFET)
// PIO0_10  (9,  Open drain)    NC
// PIO0_11  (8,  Open drain)    AUX3 input
// PIO0_12  (3,  ISP-entry)     OUT / ISP
// PIO0_13  (2)                 AUX input
// PIO0_14  (20)                Hardware detection, must be floating (or high)
// PIO0_15  (11)                NC
// PIO0_16  (10)                NC
// PIO0_17  (1)                 NC
// GND      (13)
// 3.3V     (12)
//
//
// Mk4P LPC832 IO pins: (LPC832 in TSSOP20 package)
//
//          NOTE: LPC832 MCU instead of LPC812
//
// PIO0_0   (19, TDO, ISP-Rx)   Steering input / Rx
// PIO0_1   (12, TDI)           TLC5940 GSCLK
// PIO0_2   (8,  TMS, SWDIO)    TLC5940 SCLK
// PIO0_3   (7,  TCK, SWCLK)    TLC5940 XLAT
// PIO0_4   (6,  TRST, ISP-Tx)  Throttle input / Tx
// PIO0_5   (5,  RESET)         NC
// PIO0_6                       (was TLC5940 BLANK)
// PIO0_7                       (was TLC5940 SIN)
// PIO0_8   (14, XTALIN)        was AUX2 input
// PIO0_9   (13, XTALOUT)       Switched light output (for driving a load via a MOSFET)
// PIO0_10  (10, Open drain)    S.Bus input (Rev3 and newer only! Inverted signal of ST/Rx)
// PIO0_11  (9,  Open drain)    AUX3 input
// PIO0_12  (4,  ISP-entry)     OUT / ISP
// PIO0_13  (3)                 AUX input
// PIO0_14  (20)                Hardware detection, must be floating (or high)
// PIO0_15  (11)                TLC5940 BLANK
// PIO0_17  (2)                 NC
// PIO0_23  (1)                 TLC5940 SIN
// GND      (13)
// 3.3V     (12)
// VREFN    (17)
// VREFP    (18)
//
//
// Mk4S IO pins: (LPC812 in TSSOP20 package)
//
// PIO0_0   (19, TDO, ISP-Rx)   Steering input / Rx
// PIO0_1   (12, TDI)           OUT7
// PIO0_2   (7,  TMS, SWDIO)    OUT2
// PIO0_3   (6,  TCK, SWCLK)    OUT1
// PIO0_4   (5,  TRST, ISP-Tx)  Throttle input / Tx
// PIO0_5   (4,  RESET)         NC
// PIO0_6   (18)                NC
// PIO0_7   (17)                OUT4
// PIO0_8   (14, XTALIN)        OUT5
// PIO0_9   (13, XTALOUT)       OUT6
// PIO0_10  (9,  Open drain)    NC
// PIO0_11  (8,  Open drain)    NC
// PIO0_12  (3,  ISP-entry)     OUT / ISP
// PIO0_13  (2)                 AUX input
// PIO0_14  (20)                Hardware detection, must be GND!
// PIO0_15  (11)                OUT8
// PIO0_16  (10)                OUT3
// PIO0_17  (1)                 OUT0
// GND      (13)
// 3.3V     (12)
//
//
// Mk4S LPC832 IO pins: (LPC832 in TSSOP20 package)
//
// PIO0_0   (19, TDO, ISP-Rx)   Steering input / Rx
// PIO0_1   (12, TDI)           OUT7
// PIO0_2   (8,  TMS, SWDIO)    OUT2
// PIO0_3   (7,  TCK, SWCLK)    OUT1
// PIO0_4   (6,  TRST, ISP-Tx)  Throttle input / Tx
// PIO0_5   (5,  RESET)         OUT4 (changed compared to LPC812 Mk4S)
// PIO0_6
// PIO0_7
// PIO0_8   (14, XTALIN)        OUT5
// PIO0_9   (13, XTALOUT)       OUT6
// PIO0_10  (10, Open drain)    NC
// PIO0_11  (9,  Open drain)    NC
// PIO0_12  (4,  ISP-entry)     OUT / ISP
// PIO0_13  (3)                 AUX input
// PIO0_14  (20)                Hardware detection, must be GND!
// PIO0_15  (11)                OUT8
// PIO0_17  (2)                 OUT0
// PIO0_23  (1)                 OUT3 (changed compared to LPC812 Mk4S)
// GND      (13)
// 3.3V     (12)
// VREFN    (17)
// VREFP    (18)
// ****************************************************************************

static const uint8_t HAL_GPIO_NO_PIN = 0xff;

// GPIOs common to all versions
static const HAL_GPIO_T HAL_GPIO_ST = { .pin = 0, .iocon = &LPC_IOCON->PIO0_0 };
static const HAL_GPIO_T HAL_GPIO_TH = { .pin = 4, .iocon = &LPC_IOCON->PIO0_4 };
static const HAL_GPIO_T HAL_GPIO_AUX = { .pin = 13, .iocon = &LPC_IOCON->PIO0_13 };
static const HAL_GPIO_T HAL_GPIO_OUT = { .pin = 12 };
static const HAL_GPIO_T HAL_GPIO_PIN10 = { .pin = 10, .iocon = &LPC_IOCON->PIO0_10 };
static const HAL_GPIO_T HAL_GPIO_PIN11 = { .pin = 11 };

// GPIOs for 5-channel Pre-processor
static const HAL_GPIO_T HAL_GPIO_AUX2 = { .pin = 8, .iocon = &LPC_IOCON->PIO0_8 };
static const HAL_GPIO_T HAL_GPIO_AUX3 = { .pin = 11, .iocon = &LPC_IOCON->PIO0_11 };

// GPIOs for 5-channel Pre-processor with switching outputs
static const HAL_GPIO_T HAL_GPIO_AUX2_S = { .pin = 6, .iocon = &LPC_IOCON->PIO0_6 };

// GPIOs for original Mk4 and Mk4P
static const HAL_GPIO_T HAL_GPIO_SWITCHED_LIGHT_OUTPUT = { .pin = 9 };
static const HAL_GPIO_T HAL_GPIO_SCK = { .pin = 2 };
static const HAL_GPIO_T HAL_GPIO_SIN = { .pin = 7 };
static const HAL_GPIO_T HAL_GPIO_XLAT = { .pin = 3 };
static const HAL_GPIO_T HAL_GPIO_GSCLK = { .pin = 1 };
static const HAL_GPIO_T HAL_GPIO_BLANK = { .pin = 6 };

// GPIOs for Mk4 and Mk4P with LPC832
static const HAL_GPIO_T HAL_GPIO_BLANK_LPC832 = { .pin = 15 };
static const HAL_GPIO_T HAL_GPIO_SIN_LPC832 = { .pin = 23 };

// GPIOs for Mk4S
static const HAL_GPIO_T HAL_GPIO_HARDWARE_CONFIG = { .pin = 14 };
static const HAL_GPIO_T HAL_GPIO_OUT0 = { .pin = 17 };
static const HAL_GPIO_T HAL_GPIO_OUT1 = { .pin = 3 };
static const HAL_GPIO_T HAL_GPIO_OUT2 = { .pin = 2 };
static const HAL_GPIO_T HAL_GPIO_OUT3 = { .pin = 16 };
static const HAL_GPIO_T HAL_GPIO_OUT4 = { .pin = 7 };
static const HAL_GPIO_T HAL_GPIO_OUT5 = { .pin = 8 };
static const HAL_GPIO_T HAL_GPIO_OUT6 = { .pin = 9 };
static const HAL_GPIO_T HAL_GPIO_OUT7 = { .pin = 1 };
static const HAL_GPIO_T HAL_GPIO_OUT8 = { .pin = 15 };

// GPIOs for Mk4S with LPC832
// Note that GPIO 5 and 23 are not used in the LPC812 Mk4S at all, so we
// can safely always output them (for Mk4S) regardless of which switch
// is used
static const HAL_GPIO_T HAL_GPIO_OUT3_LPC832 = { .pin = 23 };
static const HAL_GPIO_T HAL_GPIO_OUT4_LPC832 = { .pin = 5 };


static inline void HAL_gpio_in(const HAL_GPIO_T gpio)
{
    LPC_GPIO_PORT->DIR0 &= ~(1 << gpio.pin);
}

static inline void HAL_gpio_out(const HAL_GPIO_T gpio)
{
    LPC_GPIO_PORT->DIR0 |= (1 << gpio.pin);
}

static inline void HAL_gpio_out_mask(const uint32_t mask)
{
    LPC_GPIO_PORT->DIR0 |= mask;
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
    LPC_GPIO_PORT->SET0 = (1 << gpio.pin);
}

static inline void HAL_gpio_clear(const HAL_GPIO_T gpio)
{
    LPC_GPIO_PORT->CLR0 = (1 << gpio.pin);
}

static inline void HAL_gpio_clear_mask(const uint32_t mask)
{
    LPC_GPIO_PORT->CLR0 = mask;
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

