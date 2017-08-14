/******************************************************************************
******************************************************************************/
#include <stdint.h>

#include <LPC8xx.h>

#include <globals.h>
#include <uart0.h>

/*
UART register value calculation

Problem description:
    - System clock varies, but is fixed at compile time
    - Assumption is that UARTCLKDIV is 1
    - We have a fixed list of baudrates to support
        - Therefore we can use the preprocessor for calculation
    - We want to find settings for MULT and BRG for each baudrate

First we need to calculate the BRG value for the highest possible baudrate

    BAUDRATE = U_PCLK/(16 * (BRGVAL + 1))
    BAUDRATE * 16 * (BRGVAL + 1) = U_PCLK
    U_PCLK = __SYSTEM_CLOCK
    BAUDRATE = 115200
    BAUDRATE * 16 * (BRGVAL_MAXBAUD + 1) = __SYSTEM_CLOCK
    BRGVAL_MAXBAUD = (__SYSTEM_CLOCK / (BAUDRATE * 16)) - 1
    BRGVAL_MAXBAUD = int(__SYSTEM_CLOCK / (115200 * 16)) - 1

    For 12 MHz BRGVAL_MAXBAUD is 5
    For 30 MHz BRGVAL_MAXBAUD is 15

Then we can calculate the exact U_PCLK we need:

    U_PCLK = BAUDRATE * 16 * (BRGVAL_MAXBAUD + 1)

    For 12 MHZ U_PCLK is 11059200
    For 30 MHZ U_PCLK is 29491200

Now we can calculate the MULT needed:

    U_PCLK = (__SYSTEM_CLOCK / UARTCLKDIV) / (1 + (MULT / DIV))
    UARTCLKDIV = 1
    DIV = 256
    U_PCLK = __SYSTEM_CLOCK / (1 + (MULT / DIV))
    1 + (MULT / DIV) = __SYSTEM_CLOCK / U_PCLK
    MULT = round((__SYSTEM_CLOCK / U_PCLK - 1) * DIV)

Since we only can do integer math in #define we multiply by DIV first.
The rounding we implement by adding the divisor / 2 to the nominator.

    MULT = ((DIV *__SYSTEM_CLOCK) + (U_PCLK / 2)) / U_PCLK - DIV

Note that we need 64 bit math for that!

    For 12 MHZ MULT is 22
    For 30 MHZ MULT is 4

With the given MULT we can calculate the BRG values for other baudrates:

    U_PCLK = (__SYSTEM_CLOCK / UARTCLKDIV) / (1 + (MULT / DIV))
    BRGVAL = U_PCLK / (BAUDRATE * 16) - 1

Again we have to round by adding BAUDRATE * 16 / 2 to the nominator:

    BRGVAL = (U_PCLK + (BAUDRATE * 16 / 2)) / (BAUDRATE * 16) - 1

*/
#define MAX_BAUDRATE ((uint64_t)115200)
#define DIV ((uint64_t)256)
#define BRGVAL_MAXBAUD ((__SYSTEM_CLOCK / (MAX_BAUDRATE * 16)) - 1)
#define U_PCLK (MAX_BAUDRATE * 16 * (BRGVAL_MAXBAUD + 1))
#define MULT ((((__SYSTEM_CLOCK * DIV) + (U_PCLK / 2)) / U_PCLK) - DIV)

#define U_PCLK_ACTUAL ((__SYSTEM_CLOCK * DIV) / (DIV + MULT))

#define BRGVAL(x) ((U_PCLK_ACTUAL + (x * 8))/ (x * 16) - 1)



#define NO_LEADING_ZEROS (0)

#define UART_CFG_ENABLE (1 << 0)
#define UART_CFG_DATALEN(d) ((unsigned)((d) - 7) << 2)
#define UART_STAT_RXRDY (1 << 0)
#define UART_STAT_TXRDY (1 << 2)
#define UART_STAT_TXIDLE (1 << 3)

#define RECEIVE_BUFFER_SIZE (16)        // Must be modulo 2 for speed
#define RECEIVE_BUFFER_INDEX_MASK (RECEIVE_BUFFER_SIZE - 1)

/*
INT32_MIN  is -2147483648 (decimal needs 12 characters, incl. terminating '\0')
INT32_MAX  is 2147483647
UINT32_MIN is 0
UINT32_MAX is 4294967295

Worst case (base 2) we would have to write 32 characters + '\0'. However,
since we only support base 2 for uint8_t we can make due with 12 bytes,
which is the maximum needed for decimal.
*/
#define TRANSMIT_BUFFER_SIZE (12)


static uint8_t receive_buffer[RECEIVE_BUFFER_SIZE];
static volatile uint16_t read_index = 0;
static volatile uint16_t write_index = 0;




// ****************************************************************************
static void uint32_to_cstring(uint32_t value, char *result,
    unsigned int radix, int number_of_leading_zeros)
{
    char temp[TRANSMIT_BUFFER_SIZE];
    char *tp = temp;
    unsigned int digit;

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
static void int32_to_cstring(int32_t value, char *result, unsigned int radix)
{
    if (radix == 10  &&  value < 0) {
        *result++ = '-';
        value = -value;
    }

    uint32_to_cstring((uint32_t)value, result, radix, NO_LEADING_ZEROS);
}


// ****************************************************************************
void init_uart0(void)
{
    // Turn on peripheral clocks for UART0
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 14);

    // Toggle peripheral reset for USART0
    LPC_SYSCON->PRESETCTRL &= ~(1 << 3);
    LPC_SYSCON->PRESETCTRL |=  (1 << 3);

    LPC_SYSCON->UARTCLKDIV = 1;
    LPC_SYSCON->UARTFRGDIV = 255;
    LPC_SYSCON->UARTFRGMULT = MULT;

    if (config.baudrate == 115200) {
        LPC_USART0->BRG = BRGVAL(115200);
    }
    else {
        LPC_USART0->BRG = BRGVAL(38400);
    }

    LPC_USART0->CFG = UART_CFG_DATALEN(8) | UART_CFG_ENABLE;     // 8n1

    LPC_USART0->INTENSET = (1 << 0);    // Enable RXRDY interrupt
    NVIC_EnableIRQ(UART0_IRQn);
}


// ****************************************************************************
bool uart0_send_is_ready(void)
{
    return (LPC_USART0->STAT & UART_STAT_TXRDY);
}


// ****************************************************************************
void uart0_send_char(const char c)
{
    while (!(LPC_USART0->STAT & UART_STAT_TXRDY));
    LPC_USART0->TXDATA = c;
}

// ****************************************************************************
void uart0_send_uint8(const uint8_t u)
{
    while (!(LPC_USART0->STAT & UART_STAT_TXRDY));
    LPC_USART0->TXDATA = u;
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
// ****************************************************************************
// ****************************************************************************
void UART0_irq_handler(void)
{
    receive_buffer[write_index++] = (uint8_t)LPC_USART0->RXDATA;

    // Wrap around the write pointer. This works because the buffer size is
    // a modulo of 2.
    write_index &= RECEIVE_BUFFER_INDEX_MASK;

    // If we are bumping into the read pointer we are dealing with a buffer
    // overflow. Back off and rather destroy the last value.
    if (write_index == read_index) {
        write_index = (write_index - 1) & RECEIVE_BUFFER_INDEX_MASK;
    }
}


// ****************************************************************************
bool uart0_read_is_byte_pending(void)
{
    if (LPC_USART0->STAT & (1 << 8)) {
        uart0_send_cstring("overrun\n");
        LPC_USART0->STAT |= (1 << 8);
    }
    if (LPC_USART0->STAT & (1 << 13)) {
        uart0_send_cstring("frameerr\n");
        LPC_USART0->STAT |= (1 << 13);
    }
    if (LPC_USART0->STAT & (1 << 15)) {
        uart0_send_cstring("noise\n");
        LPC_USART0->STAT |= (1 << 15);
    }

    return (read_index != write_index);
}


// ****************************************************************************
uint8_t uart0_read_byte(void)
{
    uint8_t data;

    while (!uart0_read_is_byte_pending());

    data = receive_buffer[read_index++];

    // Wrap around the read pointer.
    read_index &= RECEIVE_BUFFER_INDEX_MASK;

    return data;
}
