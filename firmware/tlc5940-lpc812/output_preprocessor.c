#include <stdint.h>

#include <reader.h>
#include <uart0.h>

#define SLAVE_MAGIC_BYTE 0x87
#define CH3_HYSTERESIS 5

static bool ch3_2pos = false;
static uint8_t tx_data[4];
static uint8_t next_tx_index = 0xff;

// ****************************************************************************
void output_preprocessor(void)
{
    if (new_channel_data) {
        if (ch3_2pos) {
            if (channel[2].normalized < -CH3_HYSTERESIS) {
                ch3_2pos = false;
            }
        }
        else {
            if (channel[2].normalized > CH3_HYSTERESIS) {
                ch3_2pos = true;
            }
        }

        tx_data[0] = SLAVE_MAGIC_BYTE;
        tx_data[1] = channel[0].normalized;
        tx_data[2] = channel[1].normalized;
        tx_data[3] = (ch3_2pos ? (1 << 0) : 0)| 
                     (startup_mode == STARTUP_MODE_NEUTRAL ? (1 << 4) : 0);

        next_tx_index = 0;

#if 0
        if (startup_mode == STARTUP_MODE_NEUTRAL) {
            uart0_send_cstring("Startup\n");
        }
        else {
            uart0_send_cstring("ST: ");
            uart0_send_int32(channel[0].normalized);
            uart0_send_cstring(" TH: ");
            uart0_send_int32(channel[1].normalized);
            uart0_send_cstring(" AUX: ");
            uart0_send_int32(channel[2].normalized);
            uart0_send_linefeed();
        }
#endif    

    }

    if (next_tx_index < sizeof(tx_data)  &&  uart0_send_is_ready()) {
        uart0_send_char(tx_data[next_tx_index++]);
    }
}

