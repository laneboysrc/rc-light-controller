/******************************************************************************

    This function returns after having successfully received a complete
    protocol frame via the UART.

    The frame size is either 4 or 5 bytes. The traditional preprocessor sent
    4 byte frames, the new one with the HK310 expansion protocol sends 5 byte
    frames.

    This software automatically determines the frame size upon startup. It
    checks the number of bytes in the first few frames and only then
    outputs values.


    The preprocessor protocol is as follows:

    The first byte is always 0x87, which indicates that it is a start byte. No
    other byte can have this value.
    Note: values 0x80..0x87 do not appear in the other bytes by design,
    so those can be used as first byte to indicate start of a packet.

    The second byte is a signed char of the steering channel, from -100 to 0
    (Neutral) to +100, corresponding to the percentage of steering left/right.

    The third byte is a signed char of the throttle channel, from -100 to 0
    (Neutral) to +100.

    The fourth byte holds CH3 in the lowest bit (0 or 1), and bit 4 indicates
    whether the preprocessor is initializing. Note that the other bits must
    be zero as this is required by the light controller (waste of bits, poor
    implementation...)

    The (optional) 5th byte is the normalized 6-bit value of CH3 as used in the
    HK310 expansion protocol. This module ignores that value.
    TODO: describe this better, and define the range including both SYNC values

 *****************************************************************************/
#include <stdint.h>

#include <globals.h>
#include <hal.h>


#define SLAVE_MAGIC_BYTE 0x87
#define CONSECUTIVE_BYTE_COUNTS 3


typedef enum {
    STATE_WAIT_FOR_MAGIC_BYTE = 0,
    STATE_STEERING,
    STATE_THROTTLE,
    STATE_CH3
} STATE_T;


// ****************************************************************************
void init_uart_reader(void)
{
    if (config.mode != MASTER_WITH_UART_READER) {
        return;
    }

    global_flags.initializing = 1;
}


// ****************************************************************************
static void normalize_channel(CHANNEL_T *c, uint8_t data)
{
    if (data > 127) {
        c->normalized = -(256 - data);
    }
    else {
        c->normalized = data;
    }

    if (c->reversed) {
        c->normalized = -c->normalized;
    }

    if (c->normalized < 0) {
        c->absolute = -c->normalized;
    }
    else {
        c->absolute = c->normalized;
    }
}


// ****************************************************************************
static void publish_channels(uint8_t channel_data[])
{
    normalize_channel(&channel[ST], channel_data[0]);
    normalize_channel(&channel[TH], channel_data[1]);

    global_flags.initializing =
        (channel_data[2] & 0x10) ? true : false;

    if (!config.flags.ch3_is_local_switch) {
        normalize_channel(&channel[CH3],
            (channel_data[2] & 0x01) ? 100 : -100);
    }

    global_flags.new_channel_data = true;
}


// ****************************************************************************
void read_preprocessor(void)
{
    static STATE_T state = STATE_WAIT_FOR_MAGIC_BYTE;
    static uint8_t channel_data[3];

    uint8_t uart_byte;

    if (config.mode != MASTER_WITH_UART_READER) {
        return;
    }


    global_flags.new_channel_data = false;

    while (HAL_getchar_pending()) {
        uart_byte = HAL_getchar();

        // The preprocessor protocol is designed such that only the first
        // byte can have the MAGIC value. This allows us to be in sync at all
        // times.
        // If we receive the MAGIC value we know it is the first byte, so we
        // can kick off the state machine.
        if (uart_byte == SLAVE_MAGIC_BYTE) {
            state = STATE_STEERING;
            return;
        }

        switch (state) {
            case STATE_WAIT_FOR_MAGIC_BYTE:
                // Nothing to do; SLAVE_MAGIC_BYTE is checked globally
                break;

            case STATE_STEERING:
                channel_data[0] = uart_byte;
                state = STATE_THROTTLE;
                break;

            case STATE_THROTTLE:
                channel_data[1] = uart_byte;
                state = STATE_CH3;
                break;

            case STATE_CH3:
                channel_data[2] = uart_byte;
                publish_channels(channel_data);
                state = STATE_WAIT_FOR_MAGIC_BYTE;
                break;

            default:
                state = STATE_WAIT_FOR_MAGIC_BYTE;
                break;
        }
    }
}

