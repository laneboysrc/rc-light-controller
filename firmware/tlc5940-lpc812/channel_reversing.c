/******************************************************************************
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include <globals.h>

// ****************************************************************************
// Process_channel_reversing
//
// When the user performs 7 clicks on CH3, the left indicator and front
// head lights light up.
// The user should then turn the steering wheel to left so that the light
// controller knows the direction of the steering channel.
// The user should then also push the throttle in forward direction so that the
// light controller knows the direction of the throttle channel.
// ****************************************************************************
void process_channel_reversing_setup(void)
{
    if (!global_flags.new_channel_data) {
        return;
    }

    if (global_flags.reversing_setup == REVERSING_SETUP_OFF) {
        return;
    }

    if (channel[ST].absolute > 50) {
        // 50% or more steering input: terminate the steering reversing setup.
        // We were expecting the user to turn 'left', which should give us a
        // negative reading if the 'reversed' flat is correct. If we are
        // getting a positive reading we therefore have to reverse the
        // steering channel.
        if (channel[ST].normalized > 0) {
            channel[ST].reversed = ~channel[ST].reversed;
            // FIXME: save persistently
        }
    }

    if (channel[TH].absolute > 20) {
        // 20% or more throttle input: terminate the throttle reversing setup.
        // We were expecting the user to push the throttle 'forward', which
        // should give a positive reading if the 'reversed' flag is correct.
        // If we are reading a negative value we therefore have to reverse
        // the throttle channel.
        if (channel[TH].normalized < 0) {
            channel[TH].reversed = ~channel[ST].reversed;
            // FIXME: save persistently
        }
    }

}
