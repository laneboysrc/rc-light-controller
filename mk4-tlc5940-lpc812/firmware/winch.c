/******************************************************************************
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include <globals.h>
#include <hal.h>
// #include <printf.h>

static uint16_t winch_command_repeat_counter;

// Old winch commands (version 8 and lower)
// #define WINCH_COMMAND_DISABLED '0'
// #define WINCH_COMMAND_IDLE '1'
// #define WINCH_COMMAND_IN '2'
// #define WINCH_COMMAND_OUT '3'

// New winch commands that can be used on the same UART as the preprocessor
// or slave output. So the light controller can now drive a slave as well
// as a winch!
#define WINCH_COMMAND_DISABLED 0x80
#define WINCH_COMMAND_IDLE 0x81
#define WINCH_COMMAND_IN 0x82
#define WINCH_COMMAND_OUT 0x83


// ****************************************************************************
void winch_action(uint8_t ch3_clicks)
{
    if (!config.flags.winch_output) {
        return;
    }

    switch (ch3_clicks) {
        case 1:
            // 1 click: winch in
            global_flags.winch_mode = WINCH_IN;
            winch_command_repeat_counter = 0;
            // printf("Winch in\n");
            break;

        case 2:
            // 2 click: winch out
            global_flags.winch_mode = WINCH_OUT;
            winch_command_repeat_counter = 0;
            // printf("Winch out\n");
            break;

        case 5:
            // 5 click: winch enabled/disabled toggle
            if (global_flags.winch_mode == WINCH_DISABLED) {
                global_flags.winch_mode = WINCH_IDLE;
                // printf("Winch enabled\n");
            }
            else {
                global_flags.winch_mode = WINCH_DISABLED;
                // printf("Winch disabled\n");
            }
            winch_command_repeat_counter = 0;
            break;

        default:
            // Ignore all other clicks
            break;
    }
}


// ****************************************************************************
bool abort_winching(void)
{
    if (!config.flags.winch_output) {
        return false;
    }

    if (global_flags.winch_mode == WINCH_IN ||
        global_flags.winch_mode == WINCH_OUT) {
        global_flags.winch_mode = WINCH_IDLE;
        winch_command_repeat_counter = 0;
        // printf("Winch idle\n");
        return true;
    }
    return false;
}


// ****************************************************************************
// ****************************************************************************
void process_winch(void)
{
    if (!config.flags.winch_output) {
        return;
    }

    if (global_flags.systick) {
        if (winch_command_repeat_counter) {
            --winch_command_repeat_counter;
        }
    }

    if (winch_command_repeat_counter == 0) {
        uint8_t winch_command;

        winch_command_repeat_counter = config.winch_command_repeat_time;
        switch (global_flags.winch_mode) {
            case WINCH_DISABLED:
                winch_command = WINCH_COMMAND_DISABLED;
                break;

            case WINCH_IDLE:
                winch_command = WINCH_COMMAND_IDLE;
                break;

            case WINCH_IN:
                winch_command = WINCH_COMMAND_IN;
                break;

            case WINCH_OUT:
                winch_command = WINCH_COMMAND_OUT;
                break;

            default:
                return;

        }

        HAL_putc(STDOUT, winch_command);
    }
}
