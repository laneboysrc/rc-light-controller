#include <stdint.h>

#include <LPC8xx.h>

#include <uart0.h>

#ifndef UART0_BAUDRATE
    #define UART0_BAUDRATE 115200
#endif

// INT32_MIN  is -2147483648 (decimal needs 12 characters, incl. terminating 0)
// INT32_MAX  is 2147483647
// UINT32_MIN is 0
// UINT32_MAX is 4294967295

#define NO_LEADING_ZEROS 0

#define UART_STAT_RXRDY (1 << 0)

static uint8_t ram[40];
static UART_HANDLE_T *handle;
static UART_PARAM_T uart_param;

// ****************************************************************************
static void uint32_to_cstring(uint32_t value, char *result, int radix, int number_of_leading_zeros)
{
    // Worst case (base 2) we have to write 32 characters. However, since we
    // only support base 2 for uint8_t we can make due with 12 bytes,
    // which is the maximum needed for decimal.
    char temp[12];
    char *tp = temp;
    int digit;

    // Process the digits in reverse order, i.e. fill temp[] with the least
    // significant digit first. We stop as soon as the higher most remaining
    // digits are 0 (leading zero supression).
    do {
        digit = value % radix;
        *tp++ = (digit < 10) ? (digit + '0') : (digit + 'a' - 10);
        value /= radix;
        --number_of_leading_zeros;
    } while (value || number_of_leading_zeros > 0);

    // We write the digits to "result" in reverse order, i.e. most significant
    // digit first.
    while (tp > temp) {
        *result++ = *--tp;
    }
    *result = '\0';
}


// ****************************************************************************
static void int32_to_cstring(int32_t value, char *result, int radix)
{
    if (radix == 10  &&  value < 0) {
        *result++ = '-';
        value = -value;
    }

    return uint32_to_cstring((uint32_t)value, result, radix, NO_LEADING_ZEROS);
}


// ****************************************************************************
void init_uart0(void)
{
    UART_CONFIG_T uart_config;

    // Turn on peripheral clocks for UART0
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 14);

    // Toggle peripheral reset for USART0
    LPC_SYSCON->PRESETCTRL &= ~(1 << 3);
    LPC_SYSCON->PRESETCTRL |=  (1 << 3);

    LPC_SYSCON->UARTCLKDIV = 1;
    LPC_SYSCON->UARTFRGDIV = 255;

    handle = LPC_UART_API->uart_setup(LPC_USART0_BASE, ram);

    uart_config.sys_clk_in_hz = __SYSTEM_CLOCK;
    uart_config.baudrate_in_hz = UART0_BAUDRATE;
    uart_config.config = (0x01 << 0) | (0x00 << 2) | (0x0 << 4);    // 8n1
    uart_config.sync_mod = (0x0 << 0);      // Async mode
    uart_config.error_en = 0;               // Ignore all errors

    LPC_SYSCON->UARTFRGMULT = LPC_UART_API->uart_init(handle, &uart_config);

    uart_param.size = 0;                // Ignore, should be \0-terminated
    uart_param.transfer_mode = 0x03;    // Stop after \0, no linefeed
    uart_param.driver_mode = 0x00;      // Polling mode
    uart_param.callback_func_pt = 0;
}


// ****************************************************************************
inline void uart0_send_char(const char c)
{
    LPC_UART_API->uart_put_char(handle, c);
}


// ****************************************************************************
void uart0_send_cstring(const char *cstring)
{
    uart_param.buffer = (uint8_t *)cstring;
    uart_param.size = 0;                // Ignore, should be \0-terminated
    LPC_UART_API->uart_put_line(handle, &uart_param);
}


// ****************************************************************************
void uart0_send_int32(int32_t number)
{
    char buf[12];
    int32_to_cstring(number, buf, 10);
    uart0_send_cstring(buf);
}

// ****************************************************************************
void uart0_send_uint32(uint32_t number)
{
    char buf[12];
    uint32_to_cstring(number, buf, 10, NO_LEADING_ZEROS);
    uart0_send_cstring(buf);
}


// ****************************************************************************
void uart0_send_uint32_hex(uint32_t number)
{
    char buf[9];
    uint32_to_cstring(number, buf, 16, 8);
    uart0_send_cstring(buf);
}


// ****************************************************************************
void uart0_send_uint16_hex(uint16_t number)
{
    char buf[5];
    uint32_to_cstring(number, buf, 16, 4);
    uart0_send_cstring(buf);
}


// ****************************************************************************
void uart0_send_uint8_hex(uint8_t number)
{
    char buf[3];
    uint32_to_cstring(number, buf, 16, 4);
    uart0_send_cstring(buf);
}


// ****************************************************************************
void uart0_send_uint8_binary(uint8_t number)
{
    char buf[9];
    uint32_to_cstring(number, buf, 2, 8);
    uart0_send_cstring(buf);
}


// ****************************************************************************
inline void uart0_send_linefeed(void)
{
    uart0_send_char('\n');
}


// ****************************************************************************
int uart0_read_is_byte_pending(void)
{
    return (LPC_USART0->STAT & UART_STAT_RXRDY) ? 1 : 0;
}


// ****************************************************************************
inline uint8_t uart0_read_byte(void)
{
    return LPC_UART_API->uart_get_char(handle);
}
