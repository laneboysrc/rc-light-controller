#include <hal.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>


volatile uint32_t milliseconds;
uint32_t entropy;

void hal_hardware_init(bool is_servo_reader, bool has_servo_output)
{
    (void) is_servo_reader;
    (void) has_servo_output;

    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_GPIOA);

    systick_clear();
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    systick_set_reload(8000000 / 1000);
    systick_interrupt_enable();
    systick_counter_enable();
}

// ****************************************************************************
void sys_tick_handler(void)
{
    ++milliseconds;
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
    rcc_periph_clock_enable(RCC_USART1);

    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);
    gpio_set_af(GPIOA, GPIO_AF1, GPIO9);

    usart_set_baudrate(USART1, baudrate);
    usart_set_databits(USART1, 8);
    usart_set_parity(USART1, USART_PARITY_NONE);
    usart_set_stopbits(USART1, USART_STOPBITS_1);
    usart_set_mode(USART1, USART_MODE_TX);
    usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
    usart_enable(USART1);
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
    usart_send_blocking(USART1, c);
}

void hal_uart_send_uint8(const uint8_t c)
{
    hal_uart_send_char(c);
}

void hal_spi_init(void)
{
    rcc_periph_clock_enable(RCC_SPI2);

    // Configure GPIOs: SS=PB12, SCK=PB13, MOSI=PB15
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO13);
    gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO15);
    gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO12);

    // Reset SPI, SPI_CR1 register cleared, SPI is disabled
    spi_reset(SPI2);

    // Set up SPI2 in Master mode
    spi_init_master(SPI2, SPI_CR1_BAUDRATE_FPCLK_DIV_16,
        SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE, SPI_CR1_CPHA_CLK_TRANSITION_1,
        0, SPI_CR1_MSBFIRST);

    // Configure chip select (SS) to be controlled by software
    spi_enable_software_slave_management(SPI2);
    spi_disable_ss_output(SPI2);
    spi_set_nss_high(SPI2);

    spi_enable(SPI2);
}

void hal_spi_transaction(uint8_t *data, uint8_t count)
{
    (void) data;
    (void) count;

    gpio_clear(GPIOB, GPIO12);

    for (uint8_t i = 0; i < count; i++) {
        spi_xfer(SPI2, data[i]);
    }

    gpio_set(GPIOB, GPIO12);

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