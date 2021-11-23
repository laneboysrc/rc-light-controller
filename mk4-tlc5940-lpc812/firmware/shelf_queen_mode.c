#include <stdio.h>
#include <stdbool.h>

#include <globals.h>
#include <printf.h>

#define SHELF_QUEEN_MODE_TIMEOUT (5000 / __SYSTICK_IN_MS)

#define COMMAND_MIN 1
#define COMMAND_LIGHT_SWITCH 1
#define COMMAND_INDICATOR 2
#define COMMAND_BRAKE 3
#define COMMAND_REVERSE 4
#define COMMAND_MAX 4

static bool auto_shelf_queen_mode;
static bool last_auto_shelf_queen_mode;
static uint16_t auto_shelf_queen_mode_timeout = SHELF_QUEEN_MODE_TIMEOUT;

static uint16_t delay_time;

static uint8_t command;
static uint8_t last_command;
static uint8_t new_light_switch_position;
static uint8_t last_light_switch_position;
static uint8_t indicator_direction;
static uint8_t last_indicator_direction;

extern uint8_t light_switch_position;


// ****************************************************************************
static void light_switch(void)
{
    do {
        new_light_switch_position = random_min_max(1, config.light_switch_positions) - 1;
    } while (new_light_switch_position == last_light_switch_position);

    last_light_switch_position = new_light_switch_position;
    light_switch_position = new_light_switch_position;

    delay_time = random_min_max(1000/__SYSTICK_IN_MS, 5000/__SYSTICK_IN_MS);
}


// ****************************************************************************
static void indicator(void)
{
    do {
        indicator_direction = random_min_max(1, 3);
    } while (indicator_direction == last_indicator_direction);

    last_indicator_direction = indicator_direction;
    switch (indicator_direction) {
        case 1:
            set_blink_left();
            break;

        case 2:
            set_blink_right();
            break;

        case 3:
        default:
            set_hazard_lights(ON);
            break;
    }

    delay_time = random_min_max(2000/__SYSTICK_IN_MS, 8000/__SYSTICK_IN_MS);
}


// ****************************************************************************
static void brake(void)
{
    global_flags.forward = true;
    global_flags.reversing = false;
    global_flags.braking = false;

    throttle_neutral();
    delay_time = random_min_max(config.auto_brake_counter_value_forward_max, 5000/__SYSTICK_IN_MS);
}


// ****************************************************************************
static void reverse(void)
{
    global_flags.forward = false;
    global_flags.reversing = true;
    global_flags.braking = false;

    throttle_neutral();
    delay_time = random_min_max(config.auto_brake_counter_value_reverse_max, 5000/__SYSTICK_IN_MS);
}



// ****************************************************************************
static void next_action(void)
{

    // Stop the indicators/hazard if active
    if (global_flags.blink_indicator_left || global_flags.blink_indicator_right) {
        set_blink_off();
        delay_time = random_min_max(500/__SYSTICK_IN_MS, 4000/__SYSTICK_IN_MS);
        return;
    }
    else if (global_flags.blink_hazard) {
        set_hazard_lights(OFF);
        delay_time = random_min_max(500/__SYSTICK_IN_MS, 4000/__SYSTICK_IN_MS);
        return;
    }


    do {
        command = random_min_max(COMMAND_MIN, COMMAND_MAX);
    } while (command == last_command);

    last_command = command;

    switch (command) {
        case COMMAND_LIGHT_SWITCH:
            light_switch();
            break;

        case COMMAND_INDICATOR:
            indicator();
            break;

        case COMMAND_BRAKE:
            brake();
            break;

        case COMMAND_REVERSE:
        default:
            reverse();
            break;
    }
}


// ****************************************************************************
void set_shelf_queen_mode(bool state)
{
    global_flags.shelf_queen_mode = state;
}


// ****************************************************************************
void process_shelf_queen_mode(void)
{
    if (!global_flags.systick) {
        return;
    }

    if (config.flags2.shelf_queen_mode) {
        if (!global_flags.no_signal) {
            auto_shelf_queen_mode = false;
            auto_shelf_queen_mode_timeout = SHELF_QUEEN_MODE_TIMEOUT;
        }
        else if (auto_shelf_queen_mode_timeout) {
            --auto_shelf_queen_mode_timeout;
            if (auto_shelf_queen_mode_timeout == 0) {
                auto_shelf_queen_mode = true;
            }
        }

        if (last_auto_shelf_queen_mode != auto_shelf_queen_mode) {
            last_auto_shelf_queen_mode = auto_shelf_queen_mode;
            global_flags.shelf_queen_mode = auto_shelf_queen_mode;
        }
    }

    if (global_flags.shelf_queen_mode) {
        if (delay_time) {
            --delay_time;
        }
        else {
            next_action();
        }
    }
}
