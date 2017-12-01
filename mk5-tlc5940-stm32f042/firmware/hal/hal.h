#pragma once

#include <stdint.h>
#include <stdbool.h>


#define HAL_NUMBER_OF_PERSISTENT_ELEMENTS 16


#define HAL_NUMBER_OF_PERSISTENT_ELEMENTS 16


extern uint32_t entropy;
extern volatile uint32_t milliseconds;


void hal_hardware_init(bool is_servo_reader, bool has_servo_output);
void hal_hardware_init_final(void);

uint32_t *hal_stack_check(void);

void hal_uart_init(uint32_t baudrate);
bool hal_uart_read_is_byte_pending(void);
uint8_t hal_uart_read_byte(void);
bool hal_uart_send_is_ready(void);
void hal_uart_send_char(const char c);
void hal_uart_send_uint8(const uint8_t c);

void hal_spi_init(void);
void hal_spi_transaction(uint8_t *data, uint8_t count);

volatile const uint32_t *hal_persistent_storage_read(void);
const char *hal_persistent_storage_write(const uint32_t *new_data);

void hal_servo_output_init(void);
void hal_servo_output_set_pulse(uint16_t servo_pulse);
void hal_servo_output_enable(void);
void hal_servo_output_disable(void);

void hal_servo_reader_init(bool CPPM, uint32_t max_pulse);
bool hal_servo_reader_get_new_channels(uint32_t *raw_data);


/* ****************************************************************************
IO pins: (STM32F042F6P56 in SSOP20 package)

Pin     Name    Type    AF                                                                                      Additional functions
------------------------------------------------------------------------------------------------------------------------------------------------
1       PB8     I/O     I2C1_SCL, CEC, TIM16_CH1, TSC_SYNC, CAN_RX                                              BOOT0
2       PF0     I/O     CRS_ SYNC, I2C1_SDA                                                                     OSC_IN
3       PF1     I/O     I2C1_SCL                                                                                OSC_OUT
4       NRST    I/O                                                                                             Reset
5       VDDA    S                                                                                               Analog power supply
6       PA0     I/O     USART2_CTS, TIM2_CH1_ETR, TSC_G1_IO1                                                    RTC_ TAMP2, WKUP1, ADC_IN0
7       PA1     I/O     USART2_RTS, TIM2_CH2, TSC_G1_IO2, EVENTOUT                                              ADC_IN1
8       PA2     I/O     USART2_TX, TIM2_CH3, TSC_G1_IO3                                                         ADC_IN2, WKUP4
9       PA3     I/O     USART2_RX, TIM2_CH4, TSC_G1_IO4                                                         ADC_IN3
10      PA4     I/O     SPI1_NSS, I2S1_WS, TIM14_CH1, TSC_G2_IO1, USART2_CK USB_NOE                             ADC_IN4
11      PA5     I/O     SPI1_SCK, I2S1_CK, CEC, TIM2_CH1_ETR, TSC_G2_IO2                                        ADC_IN5
12      PA6     I/O     SPI1_MISO, I2S1_MCK, TIM3_CH1, TIM1_BKIN, TIM16_CH1, TSC_G2_IO3, EVENTOUT               ADC_IN6
13      PA7     I/O     SPI1_MOSI, I2S1_SD, TIM3_CH2, TIM14_CH1, TIM1_CH1N, TIM17_CH1, TSC_G2_IO4, EVENTOUT     ADC_IN7
14      PB1     I/O     TIM3_CH4, TIM14_CH1, TIM1_CH3N, TSC_G3_IO3                                              ADC_IN9
15      VSS     S                                                                                               Ground
16      VDD     S                                                                                               Digital power supply
17      PA9     I/O     USART1_TX, TIM1_CH2, TSC_G4_IO1, I2C1_SCL
17*     PA11    I/O     CAN_RX, USART1_CTS, TIM1_CH4, TSC_G4_IO3, EVENTOUT, I2C1_SCL                            USB_DM
18      PA10    I/O     USART1_RX, TIM1_CH3, TIM17_BKIN, TSC_G4_IO2, I2C1_SDA
18*     PA12    I/O     CAN_TX,USART1_RTS, TIM1_ETR, TSC_G4_IO4, EVENTOUT, I2C1_SDA                             USB_DP
19      PA13    I/O     IR_OUT, SWDIO USB_NOE
20      PA14    I/O     USART2_TX, SWCLK


Fixed pins:
PB8  ( 1): BOOT0
PA11 (17): USB_DM  (PA9/USART1_TX  on discovery board)
PA12 (18): USB_DP  (PA10/USART1_RX on discovery board)
PA13 (19): SWDIO
PA14 (20): SWCLK
PA5  (11): SPI1_SCK
PA7  (13): SPI1_MOSI
PA2  (08): USART2_TX  -> OUT/Tx (TIM2_CH3 for output servo pulse?)
PA3  (09): USART2_RX  -> ST/Rx (TIM2_CH4 for servo reader?)


Free to move pins:
BLANK
XLAT
GSCLK
Switched light output
CH3/Button input (TIM2_CH1 or TIM2_CH2 for servo reader?)
TH: (TIM2_CH1 or TIM2_CH2 for servo reader?)


****************************************************************************/




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


#define DECLARE_GPIO(name, bit)                                             \
                                                                            \
    static inline void hal_gpio_##name##_in(void)                           \
    {                                                                       \
        ;                                 \
    }                                                                       \
                                                                            \
    static inline void hal_gpio_##name##_out(void)                          \
    {                                                                       \
        ;                                  \
    }                                                                       \
                                                                            \
    static inline void hal_gpio_##name##_write(bool value)                  \
    {                                                                       \
        (void) value;                                     \
    }                                                                       \
                                                                            \
    static inline bool hal_gpio_##name##_read(void)                         \
    {                                                                       \
        return false;                                      \
    }                                                                       \
                                                                            \
    static inline void hal_gpio_##name##_set(void)                          \
    {                                                                       \
        ;                                         \
    }                                                                       \
                                                                            \
    static inline void hal_gpio_##name##_clear(void)                        \
    {                                                                       \
        ;                                         \
    }                                                                       \
                                                                            \
    static inline void hal_gpio_##name##_toggle(void)                       \
    {                                                                       \
        ;                                     \
    }                                                                       \


DECLARE_GPIO(gsclk, GPIO_BIT_GSCLK)
DECLARE_GPIO(blank, GPIO_BIT_BLANK)
DECLARE_GPIO(ch3, GPIO_BIT_CH3)
DECLARE_GPIO(switched_light_output, GPIO_BIT_SWITCHED_LIGHT_OUTPUT)
