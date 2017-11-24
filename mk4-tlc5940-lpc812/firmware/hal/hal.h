#pragma once

#include <stdint.h>
#include <stdbool.h>

extern uint32_t entropy;

void hal_hardware_init(bool is_servo_reader, bool has_servo_output);
void hal_hardware_init_final(void);

uint32_t *hal_stack_check(void);

void hal_uart_init(uint32_t baudrate);
bool hal_uart_read_is_byte_pending(void);
uint8_t hal_uart_read_byte(void);
bool hal_uart_send_is_ready(void);
void hal_uart_send_char(const char c);
void hal_uart_send_uint8(const uint8_t c);

void UART0_irq_handler(void);

void hal_spi_init(void);
void hal_spi_transaction(uint8_t *data, uint8_t count);

#define HAL_NUMBER_OF_PERSISTENT_ELEMENTS 16
volatile const uint32_t *hal_persistent_storage_read(void);
const char *hal_persistent_storage_write(const uint32_t *new_data);

void hal_servo_output_init(void);
void hal_servo_output_set_pulse(uint16_t servo_pulse);
void hal_servo_output_enable(void);
void hal_servo_output_disable(void);


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


#define GPIO_GSCLK LPC_GPIO_PORT->W0[GPIO_BIT_GSCLK]
#define GPIO_BLANK LPC_GPIO_PORT->W0[GPIO_BIT_BLANK]
#define GPIO_XLAT LPC_GPIO_PORT->W0[GPIO_BIT_XLAT]
#define GPIO_SCK LPC_GPIO_PORT->W0[GPIO_BIT_SCK]
#define GPIO_SIN LPC_GPIO_PORT->W0[GPIO_BIT_SIN]
#define GPIO_SWITCHED_LIGHT_OUTPUT LPC_GPIO_PORT->W0[GPIO_BIT_SWITCHED_LIGHT_OUTPUT]
#define GPIO_CH3 LPC_GPIO_PORT->W0[GPIO_BIT_CH3]
