#include <stdint.h>

#include <LPC8xx.h>

#include <globals.h>
#include <uart0.h>


// INT32_MIN  is -2147483648 (decimal needs 12 characters, incl. terminating 0)
// INT32_MAX  is 2147483647
// UINT32_MIN is 0
// UINT32_MAX is 4294967295

#define NO_LEADING_ZEROS 0

#define UART_CFG_ENABLE (1 << 0)
#define UART_CFG_DATALEN(d) (((d) - 7) << 2)
#define UART_STAT_RXRDY (1 << 0)
#define UART_STAT_TXRDY (1 << 2)
#define UART_STAT_TXIDLE (1 << 3)


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
    // Turn on peripheral clocks for UART0
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 14);

    // Toggle peripheral reset for USART0
    LPC_SYSCON->PRESETCTRL &= ~(1 << 3);
    LPC_SYSCON->PRESETCTRL |=  (1 << 3);

// -----------------------------
#if __SYSTEM_CLOCK == 12000000
    // u_pclk = 115200 * (16 * (5(=BRG) + 1)) = 11059200
    // (MULT / 256(=DIV) = ((12000000(=mainclock) / (1(=CLKDIV))) / u_pclk - 1) = 0.0850694444444444
    // MULT = 0.0850694444444444 * 256 = 22
    LPC_SYSCON->UARTCLKDIV = 1;
    LPC_SYSCON->UARTFRGDIV = 255;
    LPC_SYSCON->UARTFRGMULT = 22;

    if (config.baudrate == 115200) {
        LPC_USART0->BRG = 5; 
    }
    else {
        LPC_USART0->BRG = 17;
    }

    LPC_USART0->CFG = UART_CFG_DATALEN(8) | UART_CFG_ENABLE;     // 8n1

// -----------------------------
#elif __SYSTEM_CLOCK == 30000000
    // u_pclk = (60000000(=mainclock) / (1(=CLKDIV))) / (1 + (4(=MULT) / 256(=DIV))
    // baudrate = u_pclk / (16 * (30(=BRG) +1))
    LPC_SYSCON->UARTCLKDIV = 1;
    LPC_SYSCON->UARTFRGDIV = 255;
    LPC_SYSCON->UARTFRGMULT = 4;
#if UART0_BAUDRATE == 115200
    LPC_USART0->BRG = 30;           
#else
    #error Requested baudrate currently not implemented
#endif

    LPC_USART0->CFG = UART_CFG_DATALEN(8) | UART_CFG_ENABLE;     // 8n1
#endif
}


// ****************************************************************************
int uart0_send_is_ready(void)
{
    return (LPC_USART0->STAT & UART_STAT_TXRDY ? 1 : 0);
}


// ****************************************************************************
void uart0_send_char(const char c)
{
    while (!(LPC_USART0->STAT & UART_STAT_TXRDY));
    LPC_USART0->TXDATA = c;
}


// ****************************************************************************
void uart0_send_cstring(const char *cstring)
{
    while (*cstring) {
        uart0_send_char(*cstring);
        ++cstring;
    }
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
uint8_t uart0_read_byte(void)
{
    while (!(LPC_USART0->STAT & UART_STAT_RXRDY));
    return LPC_USART0->RXDATA;
}
