/******************************************************************************

    Reader for the SBUS protocol via UART.

    Note: UART must be configured for 100000 BAUD, 8 bits, even parity, 2 stop bits



 *****************************************************************************/
#include <stdint.h>
#include <string.h>

#include <hal.h>
#include <globals.h>
#include <printf.h>

#define SBUS_PACKET_LENGTH (25)

#define SBUS_HEADER (0x0f)
#define SBUS_FOOTER (0x00)
#define SBUS_FOOTER2a (0x4)
#define SBUS_FOOTER2b (0x8)
#define SBUS_FOOTER2_MASK (0x0f)

#define FAILSAFE_MASK (0x08)

static uint8_t buffer[SBUS_PACKET_LENGTH];
static uint8_t buffer_index = 0;


// ****************************************************************************
static uint16_t sbus_to_microseconds(uint16_t sbus_value)
{
    // Note: we AND the incoming value with 0x7ff as we optimized that out
    // of unpacking the SBUS packet, see decode_packet()
    return (uint16_t)(((5 * (uint32_t)(sbus_value & 0x07ff)) / 8) + 880);
}


// ****************************************************************************
static bool decode_packet(uint32_t *out)
{
    static uint16_t temp[16];

    // Step 1: unpack the servo channels from the SBUS packet
    //
    // Note: the unpacking does not AND with 0x07ff (optimization, we do that
    // later when converting the value to microseconds)

    temp[0] = (buffer[1] >> 0) | (buffer[2] << 8); // & 0x07ff
    temp[1] = (buffer[2] >> 3) | (buffer[3] << 5);
    temp[2] = (buffer[3] >> 6) | (buffer[4] << 2) | (buffer[5] << 10);
    temp[3] = (buffer[5] >> 1) | (buffer[6] << 7);
    temp[4] = (buffer[6] >> 4) | (buffer[7] << 4);
    temp[5] = (buffer[7] >> 7) | (buffer[8] << 1) | (buffer[9] << 9);
    temp[6] = (buffer[9] >> 2) | (buffer[10] << 6);
    temp[7] = (buffer[10] >> 5) | (buffer[11] << 3);
    temp[8] = (buffer[12] >> 0) | (buffer[13] << 8);
    temp[9] = (buffer[13] >> 3) | (buffer[14] << 5);
    temp[10] = (buffer[14] >> 6) | ((uint16_t)buffer[15] << 2) | ((uint16_t)buffer[16] << 10);
    temp[11] = (buffer[16] >> 1) | ((uint16_t)buffer[17] << 7);
    temp[12] = (buffer[17] >> 4) | ((uint16_t)buffer[18] << 4);
    temp[13] = (buffer[18] >> 7) | ((uint16_t)buffer[19] << 1) | ((uint16_t)buffer[20] << 9);
    temp[14] = (buffer[20] >> 2) | ((uint16_t)buffer[21] << 6);
    temp[15] = (buffer[21] >> 5) | ((uint16_t)buffer[22] << 3);

    // Step 2: place the decoded servo channels into the light controller channel memory

    out[0] = sbus_to_microseconds(temp[0]);
    out[1] = sbus_to_microseconds(temp[1]);
    out[2] = sbus_to_microseconds(temp[2 + config.aux_channel_offset]);
    out[3] = sbus_to_microseconds(temp[3 + config.aux_channel_offset]);
    out[4] = sbus_to_microseconds(temp[4 + config.aux_channel_offset]);
    out[5] = sbus_to_microseconds(temp[5 + config.aux_channel_offset]);
    out[6] = sbus_to_microseconds(temp[6 + config.aux_channel_offset]);
    out[7] = sbus_to_microseconds(temp[7 + config.aux_channel_offset]);

    // This function returns 'true' (new data available) if failsafe is
    // false, otherwise it returns 'false' (no new data)
    return ((buffer[23] & FAILSAFE_MASK) == 0);
}


// ****************************************************************************
static bool is_header(uint8_t c)
{
    return (c == SBUS_HEADER);
}


// ****************************************************************************
static bool is_footer(uint8_t c)
{
    if ((c == SBUS_FOOTER)  ||
        ((c & SBUS_FOOTER2_MASK) == SBUS_FOOTER2a)  ||
        ((c & SBUS_FOOTER2_MASK) == SBUS_FOOTER2b)) {
        return true;
    }

    return false;
}


// ****************************************************************************
static bool process_buffer(uint8_t incoming_byte, uint32_t *out)
{
    static uint32_t last_byte_ms = 0;

    // S.Bus packet start is determined by a pause of more than 2 ms
    // between the last byte of data.
    if ((milliseconds - last_byte_ms) > 2) {
        buffer_index = 0;
    }
    last_byte_ms = milliseconds;

    if (buffer_index < SBUS_PACKET_LENGTH) {
        buffer[buffer_index] = incoming_byte;
        ++buffer_index;
    }

    if (buffer_index == SBUS_PACKET_LENGTH) {
        if (is_header(buffer[0]) && is_footer(buffer[SBUS_PACKET_LENGTH - 1])) {
            // We have a valid SBUS packet!
            return decode_packet(out);
        }
    }

    return false;
}


// ****************************************************************************
bool sbus_reader_get_new_channels(uint32_t *out)
{
    bool result = false;

    while (HAL_getchar_pending()) {
        uint8_t uart_byte;

        uart_byte = HAL_getchar();

        if (process_buffer(uart_byte, out)) {
            result = true;
        }
    }

    return result;
}

