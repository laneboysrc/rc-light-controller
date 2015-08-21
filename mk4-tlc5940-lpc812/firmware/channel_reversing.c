/*****************************************************************************
Process_channel_reversing

When the user performs 7 clicks on CH3, channel reversing setup is engaged.

The user should then turn the steering wheel to left so that the light
controller knows the direction of the steering channel.
The user should also push the throttle in forward direction so that the
light controller knows the direction of the throttle channel.
*****************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include <globals.h>


// ****************************************************************************
void reversing_setup_action(uint8_t ch3_clicks)
{
    (void) ch3_clicks;

    if (global_flags.reversing_setup == REVERSING_SETUP_OFF) {
        global_flags.reversing_setup =
            REVERSING_SETUP_STEERING | REVERSING_SETUP_THROTTLE;
    }
    else {
        global_flags.reversing_setup = REVERSING_SETUP_OFF;
    }
}


// ****************************************************************************
void process_channel_reversing_setup(void)
{
    if (!global_flags.new_channel_data) {
        return;
    }

    if (global_flags.reversing_setup == REVERSING_SETUP_OFF) {
        return;
    }

    if (global_flags.reversing_setup & REVERSING_SETUP_STEERING) {
        if (channel[ST].absolute > 50) {
            // 50% or more steering input: terminate the steering reversing setup.
            // We were expecting the user to turn 'left', which should give us a
            // negative reading if the 'reversed' flag is correct. If we are
            // getting a positive reading we therefore have to reverse the
            // steering channel.
            if (channel[ST].normalized > 0) {
                channel[ST].reversed = !channel[ST].reversed;

            }
            global_flags.reversing_setup &= ~REVERSING_SETUP_STEERING;
        }
    }

    if (global_flags.reversing_setup & REVERSING_SETUP_THROTTLE) {
        if (channel[TH].absolute > 20) {
            // 20% or more throttle input: terminate the throttle reversing setup.
            // We were expecting the user to push the throttle 'forward', which
            // should give a positive reading if the 'reversed' flag is correct.
            // If we are reading a negative value we therefore have to reverse
            // the throttle channel.
            if (channel[TH].normalized < 0) {
                channel[TH].reversed = !channel[TH].reversed;
            }
            global_flags.reversing_setup &= ~REVERSING_SETUP_THROTTLE;
        }
    }

    if (global_flags.reversing_setup == REVERSING_SETUP_OFF) {
        write_persistent_storage();
    }
}
