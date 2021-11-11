/******************************************************************************

    Reader for the i-Bus protocol via UART.

    Note: UART must be configured for 11500 BAUD


Since we could not find a formal i-Bus specification, our i-Bus decoder
is designed with the following assumptions:

* Servo packets can be of variable length up to a maximum of 32 bytes.
* The minimum servo packet length must be 6 bytes
  (payload count, packet ID, checksum, 2 bytes for 1 channel)
* The servo packet length must be even
* Checksum is the sum of all bytes (!) XOR 0xffff

We only support channels 1 to 5; we assume the user can assign the desired
functions to those channels in the transmitter.

 *****************************************************************************/
#include <stdint.h>
#include <string.h>

#include <hal.h>
#include <globals.h>
#include <printf.h>

#define MIN_SERVO_PACKET_LENGTH (6)
#define MAX_SERVO_PACKET_LENGTH (32)

#define SBUS_SERVO_PACKET_ID (0x40)

static uint8_t buffer[MAX_SERVO_PACKET_LENGTH];
static uint8_t buffer_index = 0;


// ****************************************************************************
static void discard_from_buffer(void)
{
    memmove(&buffer[0], &buffer[1], sizeof(buffer) - 1);
    --buffer_index;
}


// ****************************************************************************
// Check the buffer whether it contains a valid i-Bus servo packet.
// Data that does not belong in a valid packet is immediately discarded.
static bool process_buffer(uint32_t *out)
{
    uint16_t checksum;
    uint16_t excepted_checksum;

    while (buffer_index) {
        // Servo packets must be even length. If it is odd length it cannot be
        // a valid packet so discard the length byte.
        if (buffer[0] & 1) {
            discard_from_buffer();
            // After we discard the byte, we check the buffer again
            continue;
        }

        // Servo packets cannot be larger than MAX_SERVO_PACKET_LENGTH bytes
        // Servo packets cannot be smaller than MIN_SERVO_PACKET_LENGTH bytes
        if (buffer[0] > MAX_SERVO_PACKET_LENGTH || buffer[0] < MIN_SERVO_PACKET_LENGTH) {
            discard_from_buffer();
            continue;
        }

        // If we only have one byte in the buffer we bail out and wait
        if (buffer_index < 2) {
            return false;
        }

        // More than one byte in the buffer: check that the packet ID is
        // a servo packet as we are only interested in those.
        if (buffer[1] != SBUS_SERVO_PACKET_ID) {
            // Discard the first byte (length) and re-check the remaining
            // data.
            discard_from_buffer();
            continue;
        }

        // Wait until we received the whole packet
        if (buffer_index != buffer[0]) {
            return false;;
        }

        excepted_checksum = *(uint16_t *)(&buffer[buffer_index-2]);
        checksum = 0;
        for (uint8_t i = 0; i < buffer_index - 2; i++) {
            checksum += buffer[i];
        }
        checksum ^= 0xffff;
        if (checksum == excepted_checksum) {
            // Checksum matches!

            // Populate the output servo values, ensure we discard the
            // upper 4 bits that are used for the 18 channel i-Bus extension
            out[0] = *(uint16_t *)(&buffer[2]) & 0xfff;
            out[1] = *(uint16_t *)(&buffer[4]) & 0xfff;
            out[2] = *(uint16_t *)(&buffer[6]) & 0xfff;
            out[3] = *(uint16_t *)(&buffer[8]) & 0xfff;
            out[4] = *(uint16_t *)(&buffer[10]) & 0xfff;

            // Discard the buffer, we don't need it anymore
            buffer_index = 0;
            return true;
        }

        // Checksum wrong: Discard the first byte and re-check
        printf("i-Bus checksum wrong: expected: 0x%04x actual: 0x%04x\n", excepted_checksum, checksum);
        discard_from_buffer();
    }

    return false;
}


// ****************************************************************************
bool ibus_reader_get_new_channels(uint32_t *out)
{
    uint8_t uart_byte;
    bool result = false;

    while (HAL_getchar_pending()) {
        uart_byte = HAL_getchar();

        // We can always add the byte into the buffer because the process_buffer
        // function ensures that the buffer never overflows.
        buffer[buffer_index++] = uart_byte;
        if (process_buffer(out)) {
            result = true;
        }
    }

    return result;
}

