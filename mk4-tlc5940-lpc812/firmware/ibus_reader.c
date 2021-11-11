/******************************************************************************

    Reader for the i-Bus protocol via UART.

    Note: UART must be configured for 11500 BAUD


Since we could not find a formal i-Bus specification, our i-Bus decoder
is designed with the following assumptions:

* Servo packets can be of variable length up to a maximum of 32 bytes.
* The minimum servo packet length must be 6 bytes
  (payload count, packet ID, checksum, 2 bytes for 1 channel)
* The servo packet length must be even


We only support channels 1 to 5; we assume the user can assign the desired
functionsto those channels in the transmitter.

 *****************************************************************************/
#include <stdint.h>
#include <string.h>

#include <hal.h>
#include <globals.h>

#define MIN_SERVO_PACKET_LENGTH (6)
#define MAX_SERVO_PACKET_LENGTH (32)

#define SBUS_SERVO_PACKET_ID (0x40)

static uint8_t buffer[MAX_SERVO_PACKET_LENGTH];
static uint8_t buffer_index = 0;


// ****************************************************************************
static void discard_from_buffer(uint8_t n)
{
    // Safety first ...
    if (n > buffer_index) {
        n = buffer_index;
    }

    memmove(&buffer[0], &buffer[n], sizeof(buffer) - n);
    buffer_index -= n;
}


// ****************************************************************************
// Check the buffer whether it contains a valid i-Bus servo packet.
// Data that does not belong in a valid packet is immediately discarded.
static void process_buffer(void)
{
    while (buffer_index) {
        // Servo packets must be even length. If it is odd length it cannot be
        // a valid packet so discard the length byte.
        if (buffer[0] & 1) {
            discard_from_buffer(1);
            // After we discard the byte, we check the buffer again
            continue;
        }

        // Servo packets cannot be larger than MAX_SERVO_PACKET_LENGTH bytes
        // Servo packets cannot be smaller than MIN_SERVO_PACKET_LENGTH bytes
        if (buffer[0] > MAX_SERVO_PACKET_LENGTH || buffer[0] < MIN_SERVO_PACKET_LENGTH) {
            discard_from_buffer(1);
            continue;
        }

        // If we only have one byte in the buffer we bail out and wait
        if (buffer_index < 2) {
            return;
        }

        // More than one byte in the buffer: check that the packet ID is
        // a servo packet as we are only interested in those.
        if (buffer[1] != SBUS_SERVO_PACKET_ID) {
            // Discard the first byte (length) and re-check the remaining
            // data.
            discard_from_buffer(1);
            continue;
        }

        // Wait until we received the whole packet
        if (buffer_index != buffer[0]) {
            return;
        }

        // Full packet received!
        // Check the checksum
        discard_from_buffer(buffer_index);
    }
}


// ****************************************************************************
void init_ibus_reader(void)
{
    // Nothing to do
}


// ****************************************************************************
void read_ibus(void)
{
    uint8_t uart_byte;

    if (config.mode != IBUS) {
        return;
    }

    while (HAL_getchar_pending()) {
        uart_byte = HAL_getchar();

        // We can always add the byte into the buffer because the process_buffer
        // function ensures that the buffer never overflows.
        buffer[buffer_index++] = uart_byte;
        process_buffer();
    }
}

