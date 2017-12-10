/******************************************************************************
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include <LPC8xx.h>
#include <hal.h>

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



#define UART_CFG_ENABLE (1 << 0)
#define UART_CFG_DATALEN(d) ((unsigned)((d) - 7) << 2)
#define UART_STAT_RXRDY (1 << 0)
#define UART_STAT_TXRDY (1 << 2)
#define UART_STAT_TXIDLE (1 << 3)

#define RECEIVE_BUFFER_SIZE (16)        // Must be modulo 2 for speed
#define RECEIVE_BUFFER_INDEX_MASK (RECEIVE_BUFFER_SIZE - 1)


void UART0_irq_handler(void);


static uint8_t receive_buffer[RECEIVE_BUFFER_SIZE];
static volatile uint16_t read_index = 0;
static volatile uint16_t write_index = 0;


// ****************************************************************************
void HAL_uart_init(uint32_t baudrate)
{
    // Turn on peripheral clocks for UART0
    LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 14);

    // Toggle peripheral reset for USART0
    LPC_SYSCON->PRESETCTRL &= ~(1 << 3);
    LPC_SYSCON->PRESETCTRL |=  (1 << 3);

    LPC_SYSCON->UARTCLKDIV = 1;
    LPC_SYSCON->UARTFRGDIV = 255;
    LPC_SYSCON->UARTFRGMULT = MULT;

    if (baudrate == 115200) {
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
bool HAL_uart_send_is_ready(void)
{
    return (LPC_USART0->STAT & UART_STAT_TXRDY);
}


// ****************************************************************************
void HAL_uart_send_char(const char c)
{
    while (!(LPC_USART0->STAT & UART_STAT_TXRDY));
    LPC_USART0->TXDATA = c;
}

// ****************************************************************************
void HAL_uart_send_uint8(const uint8_t u)
{
    while (!(LPC_USART0->STAT & UART_STAT_TXRDY));
    LPC_USART0->TXDATA = u;
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
bool HAL_uart_read_is_byte_pending(void)
{
    if (LPC_USART0->STAT & (1 << 8)) {
        // uart0_send_cstring("overrun\n");
        LPC_USART0->STAT |= (1 << 8);
    }
    if (LPC_USART0->STAT & (1 << 13)) {
        // uart0_send_cstring("frameerr\n");
        LPC_USART0->STAT |= (1 << 13);
    }
    if (LPC_USART0->STAT & (1 << 15)) {
        // uart0_send_cstring("noise\n");
        LPC_USART0->STAT |= (1 << 15);
    }

    return (read_index != write_index);
}


// ****************************************************************************
uint8_t HAL_uart_read_byte(void)
{
    uint8_t data;

    while (!HAL_uart_read_is_byte_pending());

    data = receive_buffer[read_index++];

    // Wrap around the read pointer.
    read_index &= RECEIVE_BUFFER_INDEX_MASK;

    return data;
}
