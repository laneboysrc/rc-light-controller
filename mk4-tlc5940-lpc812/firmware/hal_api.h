#pragma once

extern uint32_t entropy;
extern volatile uint32_t milliseconds;

void HAL_hardware_init(bool is_servo_reader, bool has_servo_output);
void HAL_hardware_init_final(void);

uint32_t *HAL_stack_check(void);

void HAL_service(void);

void HAL_uart_init(uint32_t baudrate);
bool HAL_uart_is_byte_pending(void);
uint8_t HAL_getc(void);
void HAL_putc(void *p, char c);

void HAL_spi_init(void);
void HAL_spi_transaction(uint8_t *data, uint8_t count);

volatile const uint32_t *HAL_persistent_storage_read(void);
const char *HAL_persistent_storage_write(const uint32_t *new_data);

void HAL_servo_output_init(void);
void HAL_servo_output_set_pulse(uint16_t servo_pulse);
void HAL_servo_output_enable(void);
void HAL_servo_output_disable(void);

void HAL_servo_reader_init(bool CPPM, uint32_t max_pulse);
bool HAL_servo_reader_get_new_channels(uint32_t *raw_data);

#if 0
#define DECLARE_GPIO(name, bit)                                             \
                                                                            \
    static inline void HAL_gpio_##name##_in(void)                           \
    {                                                                       \
    }                                                                       \
                                                                            \
    static inline void HAL_gpio_##name##_out(void)                          \
    {                                                                       \
    }                                                                       \
                                                                            \
    static inline void HAL_gpio_##name##_write(bool value)                  \
    {                                                                       \
    }                                                                       \
                                                                            \
    static inline bool HAL_gpio_##name##_read(void)                         \
    {                                                                       \
    }                                                                       \
                                                                            \
    static inline void HAL_gpio_##name##_set(void)                          \
    {                                                                       \
    }                                                                       \
                                                                            \
    static inline void HAL_gpio_##name##_clear(void)                        \
    {                                                                       \
    }                                                                       \
                                                                            \
    static inline void HAL_gpio_##name##_toggle(void)                       \
    {                                                                       \
    }
#endif