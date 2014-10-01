#include <stdio.h>
#include <stdbool.h>
#include <LPC8xx.h>

#include <globals.h>


#define SERVO_PULSE_MIN 600
#define SERVO_PULSE_MAX 2500
#define SERVO_PULSE_CLAMP_LOW 800
#define SERVO_PULSE_CLAMP_HIGH 2300
#define STARTUP_TIME 2000           // Time at startup until neutral is initialized


static enum {
    WAIT_FOR_FIRST_PULSE,
    WAIT_FOR_TIMEOUT,
    NORMAL_OPERATION
} servo_reader_state = WAIT_FOR_FIRST_PULSE;

static volatile bool new_raw_channel_data = false;
static uint32_t servo_reader_timer;




// ****************************************************************************
void init_servo_reader(void)
{
    int i;

    if (config.mode != MASTER_WITH_SERVO_READER) {
        return;
    }

    for (i = 0; i < 3; i++) {
        channel[i].normalized = 0;
        channel[i].absolute = 0;
        channel[i].reversed = false;
    }

    global_flags.startup_mode_neutral = 1;

    // SCTimer setup
    // At this point we assume that SCTimer has been setup in the following way:
    //
    //  * Split 16-bit timers
    //  * Events 1, 2 and 3 available for our use
    //  * Registers 1, 2 and 3 available for our use
    //  * CTIN_1, CTIN_2 and CTIN3 available for our use

    LPC_SCT->CTRL_L |= (1u << 3) |   // Clear the counter L
                       (5 << 5);    // PRE_L[12:5] = 6-1 (SCTimer L clock 2 MHz)


    // Configure registers 1..3 to capture servo pulses on SCTimer L
    for (i = 1; i <= 3; i++) {
        LPC_SCT->REGMODE_L |= (1u << i);         // Register i is capture register

        LPC_SCT->EVENT[i].STATE = 0xFFFF;       // Event i happens in all states
        LPC_SCT->EVENT[i].CTRL = (0 << 5) |     // OUTSEL: select input elected by IOSEL
                                 (1u << 6) |     // IOSEL: CTIN_i
                                 (0x1u << 10) |  // IOCOND: rising edge
                                 (0x2 << 12);   // COMBMODE: Uses the specified I/O condition only
        LPC_SCT->CAPCTRL[i].L = (1u << i);       // Event i loads capture register i
        LPC_SCT->EVEN |= (1u << i);              // Event i generates an interrupt
    }


    // SCT CTIN_3 at PIO0.13, CTIN_2 at PIO0.4, CTIN_1 at PIO0.0
    LPC_SWM->PINASSIGN6 = 0xff0d0400;


    LPC_SCT->CTRL_L &= ~(1u << 2);           // Start the SCTimer L
    NVIC_EnableIRQ(SCT_IRQn);
}


// ****************************************************************************
void SCT_irq_handler(void)
{
    static uint32_t start[3] = {0, 0, 0};
    static uint32_t result[3] = {0, 0, 0};
    static uint32_t channel_flags = 0;
    uint32_t capture_value;
    int i;

/*
    Algorithm for reading 3 servo pulses

    The SCTimer in 16-bit mode is utilized.
    We use 3 events, 3 capture registers, and 3 CTIN signals connected to the
    servo input pins. The 16 bit timer L is running at 1 MHz, giving us a
    resolution of 1 us.
    (Discussion: we could increase the resolution since we don't need to measure
    65 ms. But detecting overruns could get important).

    At rest the 3 captures wait for the rising edge.
    When an edge is detected the value is retrieved from the capture register
    and stored in a persistent holding place. The edge is toggled.

    When a falling edge is detected we calculate the difference (taking
    overflow into account) and store it in presistent result registers
    (one per channel).

    In order to be able to be able to handle missing channels we do the
    following:

    Each channel has a flag that gets set on the rising edge.
    When a channel sees its flag set at a rising edge it clears the
    flags of the *other* channels, but leaves its own flat set. It then copies
    all result registers into transfer registers (one per channel) and sets a
    flag to let the mainloop know that a set of data is available.
    The result registers are cleared.

    This way the first channel that outputs data will dictate the repeat
    frequency of the combined set of channels. If this dominant channel
    goes missing another channel will take over after two pulses.

    Missing channels will have the value 0, active channels the measured
    pulse duration.

    The downside of the algorithm is that there is a 1 frame delay
    of the output, but it is very robust for use in the pre-processor.

    */

    for (i = 1; i <= 3; i++) {
        // Event i: Capture CTIN_i
        if (LPC_SCT->EVFLAG & (1u << i)) {
            capture_value = LPC_SCT->CAP[i].L;

            if (LPC_SCT->EVENT[i].CTRL & (0x1u << 10)) {
                // Rising edge triggered
                start[i - 1] = capture_value;

                if (channel_flags & (1u << i)) {
                    channel_flags = (1u << i);
                    channel[0].raw_data = result[0] >> 1;
                    channel[1].raw_data = result[1] >> 1;
                    channel[2].raw_data = result[2] >> 1;
                    result[0] = result[1] = result[2] = 0;
                    new_raw_channel_data = true;
                }
                channel_flags |= (1u << i);
            }
            else {
                // Falling edge triggered
                if (start[i - 1] > capture_value) {
                    // Compensate for wrap-around
                    capture_value += LPC_SCT->MATCHREL[0].L + 1;
                }
                result[i - 1] = capture_value - start[i - 1];
            }

            LPC_SCT->EVENT[i].CTRL ^= (0x3 << 10);     // IOCOND: toggle edge
            LPC_SCT->EVFLAG = (1u << i);
        }
    }

}


// ****************************************************************************
static void normalize_channel(struct channel_s *c)
{
    if (c->raw_data < SERVO_PULSE_MIN  ||  c->raw_data > SERVO_PULSE_MAX) {
        c->normalized = 0;
        c->absolute = 0;
        return;
    }

    if (c->raw_data < SERVO_PULSE_CLAMP_LOW) {
        c->raw_data = SERVO_PULSE_CLAMP_LOW;
    }

    if (c->raw_data > SERVO_PULSE_CLAMP_HIGH) {
        c->raw_data = SERVO_PULSE_CLAMP_HIGH;
    }

    if (c->raw_data == c->centre) {
        c->normalized = 0;
    }
    else if (c->raw_data < c->centre) {
        if (c->raw_data < c->ep_l) {
            c->ep_l = c->raw_data;
        }
        // In order to acheive a stable 100% value we actually calculate the
        // percentage up to 101%, and then clamp to 100%.
        c->normalized = (c->centre - c->raw_data) * 101 / (c->centre - c->ep_l);
        if (c->normalized > 100) {
            c->normalized = 100;
        }
        if (!c->reversed) {
            c->normalized = -c->normalized;
        }
    }
    else {
        if (c->raw_data > c->ep_h) {
            c->ep_h = c->raw_data;
        }
        c->normalized = (c->raw_data - c->centre) * 101 / (c->ep_h - c->centre);;
        if (c->normalized > 100) {
            c->normalized = 100;
        }
        if (c->reversed) {
            c->normalized = -c->normalized;
        }
    }

    if (c->normalized < 0) {
        c->absolute = -c->normalized;
    }
    else {
        c->absolute = c->normalized;
    }
}


// ****************************************************************************
void read_all_servo_channels(void)
{
    if (config.mode != MASTER_WITH_SERVO_READER) {
        return;
    }

    if (global_flags.systick) {
        if (servo_reader_timer) {
            --servo_reader_timer;
        }
    }

    global_flags.new_channel_data = false;

    if (!new_raw_channel_data) {
        return;
    }
    new_raw_channel_data = false;

    switch (servo_reader_state) {
        case WAIT_FOR_FIRST_PULSE:
            servo_reader_timer = STARTUP_TIME / __SYSTICK_IN_MS;
            servo_reader_state = WAIT_FOR_TIMEOUT;
            break;

        case WAIT_FOR_TIMEOUT:
            if (servo_reader_timer == 0) {
                int i;

                servo_reader_state = NORMAL_OPERATION;
                global_flags.startup_mode_neutral = 0;

                for (i = 0; i < 3; i++) {
                    channel[i].centre = channel[i].raw_data;
                    channel[i].ep_l = channel[i].raw_data - 250;
                    channel[i].ep_h = channel[i].raw_data + 250;
                }
            }
            global_flags.new_channel_data = true;
            break;

        case NORMAL_OPERATION:
            normalize_channel(&channel[0]);
            normalize_channel(&channel[1]);
            normalize_channel(&channel[2]);
            global_flags.new_channel_data = true;
            break;
    }
}
