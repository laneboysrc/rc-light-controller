/******************************************************************************

    This module reads the servo pulses for steering, throttle and CH3/AUX
    from a receiver.

    It can also read the pulses from a CPPM output, provided your receiver
    has such an output.

    It populates the global channel[] array with the read data.


    Internal operation for reading servo pulses:
    --------------------------------------------
    The SCTimer in 16-bit mode is utilized.
    We use 3 events, 3 capture registers, and 3 CTIN signals connected to the
    servo input pins. The 16 bit timer L is running at 2 MHz, giving us a
    resolution of 0.5 us.

    At rest, the 3 capture registers wait for a rising edge.
    When an edge is detected the value is retrieved from the capture register
    and stored in a holding place. The edge of the capture block is toggled.

    When a falling edge is detected we calculate the difference (taking
    overflow into account) and store it in a result registers (raw_data, one
    per channel).

    In order to be able to be able to handle missing channels we do the
    following:

    Each channel has a flag that gets set on the rising edge.
    When a channel sees its flag set at a rising edge it clears the
    flags of the *other* channels, but leaves its own flat set. It then copies
    all result registers into transfer registers (one per channel) and sets a
    flag to let the mainloop know that a set of data is available.
    The result registers are cleared.

    This way the first channel that outputs data will dictate the repeat
    frequency of the combined set of channels. If this "dominant channel"
    goes missing, another channel will take over after two pulses.

    Missing channels will have the value 0 in raw_data, active channels the
    measured pulse duration in milliseconds.

    The downside of the algorithm is that there is a one frame delay
    of the output, but it is very robust for use in the pre-processor.


    Internal operation for reading CPPM:
    ------------------------------------
    The SCTimer in 16-bit mode is utilized.
    We use 1 event, 1 capture register, and the CTIN_1 signals connected to the
    ST servo input pin. The 16 bit timer L is running at 2 MHz, giving us a
    resolution of 0.5 us.

    The capture register is setup to trigger of falling edges of the CPPM
    signal. Every time an interrupt occurs the time duration from the
    previous pulse is calculated.

    If that time difference is larger than the largest servo pulse we expect
    (which is 2.5 ms) then we know that a new CPPM "frame" has started and
    we set our state-machine so that the next edge is stored as CH1, then
    the next as CH2, and one more edge as CH3. After we received all
    3 channels we update the rest of the light controller with the new
    data and setup the CPPM reader to wait for a frame sync signal (= >2/5ms
    between interrupts). Smaller pulses (i.e. because the receiver outputs
    more than 3 channles) are ignored.

    In case the receiver outputs less than 3 channels, the frame detection
    function outputs the channels that have been received so far.


******************************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include <LPC8xx.h>

#include <globals.h>


#define SERVO_PULSE_CLAMP_LOW 800
#define SERVO_PULSE_CLAMP_HIGH 2300


static enum {
    WAIT_FOR_FIRST_PULSE,
    WAIT_FOR_TIMEOUT,
    NORMAL_OPERATION
} servo_reader_state = WAIT_FOR_FIRST_PULSE;

typedef enum {
    WAIT_FOR_ANY_PULSE = 0,
    WAIT_FOR_IDLE_PULSE,
    WAIT_FOR_CH1,
    WAIT_FOR_CH2,
    WAIT_FOR_CH3
} CPPM_STATE_T;

static volatile bool new_raw_channel_data = false;
static uint32_t servo_reader_timer;


// ****************************************************************************
void init_servo_reader(void)
{
    if (config.mode != MASTER_WITH_SERVO_READER  &&
        config.mode != MASTER_WITH_CPPM_READER) {
        return;
    }

    global_flags.initializing = 1;

    // SCTimer setup
    // At this point we assume that SCTimer has been setup in the following way:
    //
    //  * Split 16-bit timers
    //  * Events 1, 2 and 3 available for our use
    //  * Registers 1, 2 and 3 available for our use
    //  * CTIN_1, CTIN_2 and CTIN3 available for our use

    LPC_SCT->CTRL_L |= (1 << 3) |   // Clear the counter L
                       (5 << 5);    // PRE_L[12:5] = 6-1 (SCTimer L clock 2 MHz)


    if (config.mode == MASTER_WITH_SERVO_READER) {
        int i;

        // Configure registers 1..3 to capture servo pulses on SCTimer L
        for (i = 1; i <= 3; i++) {
            LPC_SCT->REGMODE_L |= (1 << i);         // Register i is capture register

            LPC_SCT->EVENT[i].STATE = 0xFFFF;       // Event i happens in all states
            LPC_SCT->EVENT[i].CTRL = (0 << 5) |     // OUTSEL: select input elected by IOSEL
                                     (i << 6) |     // IOSEL: CTIN_i
                                     (0x1 << 10) |  // IOCOND: rising edge
                                     (0x2 << 12);   // COMBMODE: Uses the specified I/O condition only
            LPC_SCT->CAPCTRL[i].L = (1 << i);       // Event i loads capture register i
            LPC_SCT->EVEN |= (1 << i);              // Event i generates an interrupt
        }

        // SCT CTIN_3 at PIO0.13, CTIN_2 at PIO0.4, CTIN_1 at PIO0.0
        LPC_SWM->PINASSIGN6 = 0xff0d0400;
    }

    else { // MASTER_WITH_CPPM_READER
        LPC_SCT->REGMODE_L |= (1 << 1);         // Register i is capture register

        LPC_SCT->EVENT[1].STATE = 0xFFFF;       // Event i happens in all states
        LPC_SCT->EVENT[1].CTRL = (0 << 5) |     // OUTSEL: select input elected by IOSEL
                                 (1 << 6) |     // IOSEL: CTIN_1
                                 (0x2 << 10) |  // IOCOND: falling edge
                                 (0x2 << 12);   // COMBMODE: Uses the specified I/O condition only
        LPC_SCT->CAPCTRL[1].L = (1 << 1);       // Event 1 loads capture register 1
        LPC_SCT->EVEN |= (1 << 1);              // Event 1 generates an interrupt

        // SCT CTIN_1 at PIO0.0
        LPC_SWM->PINASSIGN6 = 0xffffff00;
    }


    LPC_SCT->CTRL_L &= ~(1 << 2);           // Start the SCTimer L
    NVIC_EnableIRQ(SCT_IRQn);
}


// ****************************************************************************
static void output_raw_channels(uint32_t result[3])
{
    channel[ST].raw_data = result[0] >> 1;
    channel[TH].raw_data = result[1] >> 1;
    if (!config.flags.ch3_is_local_switch) {
        channel[CH3].raw_data = result[2] >> 1;
    }

    result[0] = result[1] = result[2] = 0;
    new_raw_channel_data = true;
}


// ****************************************************************************
void SCT_irq_handler(void)
{
    static uint32_t start[3] = {0, 0, 0};
    static uint32_t result[3] = {0, 0, 0};
    static uint8_t channel_flags = 0;
    uint32_t capture_value;

    if (config.mode == MASTER_WITH_SERVO_READER) {
        int i;

        for (i = 1; i <= 3; i++) {
            // Event i: Capture CTIN_i
            if (LPC_SCT->EVFLAG & (1 << i)) {
                capture_value = LPC_SCT->CAP[i].L;

                if (LPC_SCT->EVENT[i].CTRL & (0x1 << 10)) {
                    // Rising edge triggered
                    start[i - 1] = capture_value;

                    if (channel_flags & (1 << i)) {
                        output_raw_channels(result);
                        channel_flags = (1 << i);
                    }
                    channel_flags |= (1 << i);
                }
                else {
                    // Falling edge triggered
                    if (start[i - 1] > capture_value) {
                        // Compensate for wrap-around
                        capture_value += LPC_SCT->MATCHREL[0].L + 1;
                    }
                    result[i - 1] = capture_value - start[i - 1];
                }

                LPC_SCT->EVENT[i].CTRL ^= (0x3 << 10);   // IOCOND: toggle edge
                LPC_SCT->EVFLAG = (1 << i);
            }
        }
    }

    else { // MASTER_WITH_CPPM_READER
        static CPPM_STATE_T cppm_mode = WAIT_FOR_ANY_PULSE;

        start[1] = capture_value = LPC_SCT->CAP[1].L;
        if (start[0] > capture_value) {
            // Compensate for wrap-around
            capture_value += LPC_SCT->MATCHREL[0].L + 1;
        }
        capture_value -= start[0];
        start[0] = start[1];


        // FIXME: make max pulse time configuratble for CPPM
        if (cppm_mode != WAIT_FOR_ANY_PULSE  &&
            capture_value > ((uint32_t)config.servo_pulse_max << 1)) {

            // If we are dealing with a radio that has less than 2 CPPM
            // channels then output the channels when we received the
            // idle marker.
            if (channel_flags) {
                output_raw_channels(result);
                channel_flags = 0;
            }

            cppm_mode = WAIT_FOR_CH1;
        }
        else {
            switch (cppm_mode) {
                case WAIT_FOR_CH1:
                    result[0] = capture_value;
                    channel_flags |= (1 << 0);
                    cppm_mode = WAIT_FOR_CH2;
                    break;

                case WAIT_FOR_CH2:
                    result[1] = capture_value;
                    channel_flags |= (1 << 1);
                    cppm_mode = WAIT_FOR_CH3;
                    break;

                case WAIT_FOR_CH3:
                    result[2] = capture_value;
                    output_raw_channels(result);
                    channel_flags = 0;
                    cppm_mode = WAIT_FOR_IDLE_PULSE;
                    break;

                case WAIT_FOR_ANY_PULSE:
                case WAIT_FOR_IDLE_PULSE:
                default:
                    cppm_mode = WAIT_FOR_IDLE_PULSE;
                    break;
            }
        }

        LPC_SCT->EVFLAG = (1 << 1);
    }
}


// ****************************************************************************
static void normalize_channel(CHANNEL_T *c)
{
    if (c->raw_data < config.servo_pulse_min  ||  c->raw_data > config.servo_pulse_max) {
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

    if (c->raw_data == c->endpoint.centre) {
        c->normalized = 0;
    }
    else if (c->raw_data < c->endpoint.centre) {
        if (c->raw_data < c->endpoint.left) {
            c->endpoint.left = c->raw_data;
        }
        // In order to acheive a stable 100% value we actually calculate the
        // percentage up to 101%, and then clamp to 100%.
        c->normalized = (c->endpoint.centre - c->raw_data) * 101 /
            (c->endpoint.centre - c->endpoint.left);
        if (c->normalized > 100) {
            c->normalized = 100;
        }
        if (!c->reversed) {
            c->normalized = -c->normalized;
        }
    }
    else {
        if (c->raw_data > c->endpoint.right) {
            c->endpoint.right = c->raw_data;
        }
        c->normalized = (c->raw_data - c->endpoint.centre) * 101 /
            (c->endpoint.right - c->endpoint.centre);
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
static void initialize_channel(CHANNEL_T *c) {
    c->endpoint.centre = c->raw_data;
    c->endpoint.left = c->raw_data - config.initial_endpoint_delta;
    c->endpoint.right = c->raw_data + config.initial_endpoint_delta;
}


// ****************************************************************************
void read_all_servo_channels(void)
{
    if (config.mode != MASTER_WITH_SERVO_READER  &&
        config.mode != MASTER_WITH_CPPM_READER) {
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
            servo_reader_timer = config.startup_time;
            servo_reader_state = WAIT_FOR_TIMEOUT;
            break;

        case WAIT_FOR_TIMEOUT:
            if (servo_reader_timer == 0) {
                initialize_channel(&channel[ST]);
                initialize_channel(&channel[ST]);
                if (!config.flags.ch3_is_local_switch) {
                    initialize_channel(&channel[CH3]);
                }

                servo_reader_state = NORMAL_OPERATION;
                global_flags.initializing = 0;
            }
            global_flags.new_channel_data = true;
            break;

        case NORMAL_OPERATION:
            normalize_channel(&channel[ST]);
            normalize_channel(&channel[TH]);
            if (!config.flags.ch3_is_local_switch) {
                // FIXME: does this need to be different for CH3?!!
                normalize_channel(&channel[CH3]);
            }
            global_flags.new_channel_data = true;
            break;

        default:
            servo_reader_state = WAIT_FOR_FIRST_PULSE;
            break;
    }
}
