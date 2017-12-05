#include <hal.h>

volatile uint32_t milliseconds;

void hal_hardware_init(bool is_servo_reader, bool has_servo_output)
{
    (void) is_servo_reader;
    (void) has_servo_output;
}

void hal_hardware_init_final(void)
{

}

uint32_t *hal_stack_check(void)
{
    return 0;
}

void hal_uart_init(uint32_t baudrate)
{
    (void) baudrate;
}

bool hal_uart_read_is_byte_pending(void)
{
    return false;
}

uint8_t hal_uart_read_byte(void)
{
    return 0;
}

bool hal_uart_send_is_ready(void)
{
    return true;
}

void hal_uart_send_char(const char c)
{
    (void) c;
}

void hal_uart_send_uint8(const uint8_t c)
{
    (void) c;
}

void hal_spi_init(void)
{

}

void hal_spi_transaction(uint8_t *data, uint8_t count)
{
    (void) data;
    (void) count;
}

volatile const uint32_t *hal_persistent_storage_read(void)
{
    return 0;
}

const char *hal_persistent_storage_write(const uint32_t *new_data)
{
    (void) new_data;
    return 0;
}

void hal_servo_output_init(void)
{

}

void hal_servo_output_set_pulse(uint16_t servo_pulse)
{
    (void) servo_pulse;
}

void hal_servo_output_enable(void)
{

}

void hal_servo_output_disable(void)
{

}

void hal_servo_reader_init(bool CPPM, uint32_t max_pulse)
{
    (void) CPPM;
    (void) max_pulse;
}

bool hal_servo_reader_get_new_channels(uint32_t *raw_data)
{
    (void) raw_data;
    return false;
}