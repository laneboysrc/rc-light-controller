#ifndef __UART0_H
#define __UART0_H

#include <stdint.h>
#include <stdbool.h>

void init_uart0(void);

bool uart0_send_is_ready(void);
void uart0_send_char(const char c);
void uart0_send_cstring(const char *cstring);
void uart0_send_int32(int32_t number);
void uart0_send_uint32(uint32_t number);
void uart0_send_uint32_hex(uint32_t number);
void uart0_send_uint16_hex(uint16_t number);
void uart0_send_uint8_hex(uint8_t number);
void uart0_send_uint8_binary(uint8_t number);
void uart0_send_linefeed(void);

bool uart0_read_is_byte_pending(void);
void UART0_irq_handler(void);
uint8_t uart0_read_byte(void);

#endif /* __UART0_H */
