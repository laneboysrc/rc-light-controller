#include <stdint.h>
#include <stdio.h>

#include <hal.h>
#include <uart.h>


volatile uint32_t milliseconds;


// These are all defined by the linker via the samd21e15.ld linker script.
extern unsigned int _ram;
extern unsigned int _stacktop;


void SysTick_handler(void);
void HardFault_handler(void);


DECLARE_GPIO(UART_TXD, GPIO_PORTA, 4)
DECLARE_GPIO(UART_RXD, GPIO_PORTA, 5)




// ****************************************************************************
void SysTick_handler(void)
{
    ++milliseconds;
}


// ****************************************************************************
void HardFault_handler(void)
{
    uart0_send_cstring("HARD\n");
    while (1);
}


// ****************************************************************************
void hal_hardware_init(bool is_servo_reader, bool has_servo_output)
{
    (void) is_servo_reader;
    (void) has_servo_output;

    // Switch to 8MHz clock (disable prescaler)
    SYSCTRL->OSC8M.bit.PRESC = 0;

    hal_gpio_led0_out();
    hal_gpio_led0_set();

    SysTick_Config((__SYSTEM_CLOCK / 1000) - 1);

    __enable_irq();
}


// ****************************************************************************
void hal_hardware_init_final(void)
{

}


// ****************************************************************************
uint32_t *hal_stack_check(void)
{
    #define CANARY 0xcafebabe

    /*
    There is an issue if we initialize the last_found static variable with
    _stacktop at compile time. For some reason it does not contain the proper
    value.
    We therefore initialize it with 0 and check for that. If we enounter 0 we
    load the _stacktop address and everything works well.

    Worst case the program hangs when last_found is not aligned to 4 bytes, as
    a hard fault is raised.
    */

    static uint32_t *last_found;
    uint32_t *now;

    if (last_found == NULL) {
        last_found = (uint32_t *)&_stacktop;
    }

    now = last_found;

    // for (int i = 0; i < 60; i++) {
    //     uart0_send_uint32_hex(*now--);
    //     uart0_send_linefeed();
    // }
    // uart0_send_linefeed();

    while (*now != CANARY  &&  now > (uint32_t *)&_ram) {
        --now;
    }

    if (now < last_found) {
        last_found = now;
        return now;
    }
    return NULL;
}

// ****************************************************************************
void hal_uart_init(uint32_t baudrate)
{
    uint64_t brr = (uint64_t)65536 * (__SYSTEM_CLOCK - 16 * baudrate) / __SYSTEM_CLOCK;

    hal_gpio_UART_TXD_out();
    hal_gpio_UART_TXD_pmuxen();
    PORT->Group[GPIO_PORTA].PMUX[2].bit.PMUXE = PORT_PMUX_PMUXE_D_Val; // TX

    hal_gpio_UART_RXD_in();
    hal_gpio_UART_RXD_pmuxen();
    PORT->Group[GPIO_PORTA].PMUX[2].bit.PMUXO = PORT_PMUX_PMUXO_D_Val; // RX

    PM->APBCMASK.reg |= PM_APBCMASK_SERCOM0;

    GCLK->CLKCTRL.reg =
        GCLK_CLKCTRL_ID(SERCOM0_GCLK_ID_CORE) |
        GCLK_CLKCTRL_CLKEN |
        GCLK_CLKCTRL_GEN(0);

    SERCOM0->USART.CTRLA.reg =
        SERCOM_USART_CTRLA_DORD |
        SERCOM_USART_CTRLA_MODE(SERCOM_USART_CTRLA_MODE_USART_INT_CLK_Val) |
        SERCOM_USART_CTRLA_RXPO(1/*PAD1*/) |
        SERCOM_USART_CTRLA_TXPO(0/*PAD0*/);

    SERCOM0->USART.CTRLB.reg =
        SERCOM_USART_CTRLB_RXEN |
        SERCOM_USART_CTRLB_TXEN |
        SERCOM_USART_CTRLB_CHSIZE(0/*8 bits*/);
    while (SERCOM0->USART.SYNCBUSY.reg);

    SERCOM0->USART.BAUD.reg = (uint16_t)brr;
    while (SERCOM0->USART.SYNCBUSY.reg);

    SERCOM0->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
    while (SERCOM0->USART.SYNCBUSY.reg);

    // SERCOM0->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXC;
    // NVIC_EnableIRQ(SERCOM0_IRQn);
}


// ****************************************************************************
bool hal_uart_read_is_byte_pending(void)
{
    return false;
}


// ****************************************************************************
uint8_t hal_uart_read_byte(void)
{
    return 0;
}


// ****************************************************************************
bool hal_uart_send_is_ready(void)
{
    return (SERCOM0->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_DRE);
}


// ****************************************************************************
void hal_uart_send_char(const char c)
{
    while (!(SERCOM0->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_DRE));
    SERCOM0->USART.DATA.reg = c;
}


// ****************************************************************************
void hal_uart_send_uint8(const uint8_t c)
{
    hal_uart_send_char(c);
}


// ****************************************************************************
void hal_spi_init(void)
{

}


// ****************************************************************************
void hal_spi_transaction(uint8_t *data, uint8_t count)
{
    (void) data;
    (void) count;
    hal_gpio_led0_clear();
}


// ****************************************************************************
volatile const uint32_t *hal_persistent_storage_read(void)
{
    return 0;
}


// ****************************************************************************
const char *hal_persistent_storage_write(const uint32_t *new_data)
{
    (void) new_data;
    return 0;
}


// ****************************************************************************
void hal_servo_output_init(void)
{

}


// ****************************************************************************
void hal_servo_output_set_pulse(uint16_t servo_pulse)
{
    (void) servo_pulse;
}


// ****************************************************************************
void hal_servo_output_enable(void)
{

}


// ****************************************************************************
void hal_servo_output_disable(void)
{

}


// ****************************************************************************
void hal_servo_reader_init(bool CPPM, uint32_t max_pulse)
{
    (void) CPPM;
    (void) max_pulse;
}


// ****************************************************************************
bool hal_servo_reader_get_new_channels(uint32_t *raw_data)
{
    (void) raw_data;
    return false;
}