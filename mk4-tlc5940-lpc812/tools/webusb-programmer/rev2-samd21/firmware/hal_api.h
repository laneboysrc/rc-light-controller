#pragma once

extern uint32_t entropy;
extern volatile uint32_t milliseconds;

void HAL_hardware_init(void);
void HAL_hardware_init_final(void);

void HAL_service(void);

void HAL_uart_init(uint32_t baudrate);
void HAL_uart_set_baudrate(uint32_t baudrate);

bool HAL_getchar_pending(void *p);
uint8_t HAL_getchar(void *p);
void HAL_putc(void *p, char c);
