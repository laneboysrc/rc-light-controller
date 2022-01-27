#pragma once

extern uint32_t entropy;
extern volatile uint32_t milliseconds;
extern bool is_lpc832;

void HAL_hardware_init(void);
void HAL_hardware_init_final(void);

void HAL_service(void);

void HAL_uart_init(uint32_t baudrate, uint8_t rx_pin, uint8_t tx_pin, bool eight_e_two);
bool HAL_getchar_pending(void);
uint8_t HAL_getchar(void);
void HAL_putc(void *p, char c);

void HAL_spi_init(void);
void HAL_spi_transaction(uint8_t *data, uint8_t count);

volatile const uint32_t *HAL_persistent_storage_read(void);
const char *HAL_persistent_storage_write(const uint32_t *new_data);

bool HAL_switch_triggered(void);

void HAL_servo_output_init(uint8_t pin);
void HAL_servo_output_set_pulse(uint16_t servo_pulse);
void HAL_servo_output_enable(void);
void HAL_servo_output_disable(void);

void HAL_servo_reader_init(void);
bool HAL_servo_reader_get_new_channels(uint32_t *raw_data);
