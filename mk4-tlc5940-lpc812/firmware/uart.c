/******************************************************************************
******************************************************************************/
#include <stdint.h>

#include <hal.h>

#include <globals.h>
#include <uart.h>



#define NO_LEADING_ZEROS (0)

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
bool uart0_read_is_byte_pending(void)
{
    return hal_uart_read_is_byte_pending();
}

// ****************************************************************************
uint8_t uart0_read_byte(void)
{
    return hal_uart_read_byte();
}

// ****************************************************************************
bool uart0_send_is_ready(void)
{
    return hal_uart_send_is_ready();
}

// ****************************************************************************
void uart0_send_char(const char c)
{
    hal_uart_send_char(c);
}

// ****************************************************************************
void uart0_send_uint8(const uint8_t c)
{
    hal_uart_send_uint8(c);
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

