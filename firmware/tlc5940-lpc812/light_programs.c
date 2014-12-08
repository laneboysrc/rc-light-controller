/******************************************************************************

    Light programs:
        * Bogdan's idea regarding flame simulation, depending on throttle
        - Programs reside at the end of flash space
        - The maximum number of programs is predetermined as we need to
          assign memory like program counter to each program, and so far
          there is no heap (malloc)
        - Mini programming language
            - GOTO to implement loops
            - SET start_led stop_led value (0..100%)
            - SET start_led stop_led VARIABLE (0..100%)
            - FADE start_led stop_led value (0..100%)
            - FADE start_led stop_led VARIABLE  (0..100%)
            - SLEEP time (ms)
            - SLEEP VARIABLE (ms)
            - VARIABLE = {integer, VARIABLE, LED[x], random-value, TH, ST}
            - VARIABLE = abs {integer, VARIABLE, LED[x], random-value, TH, ST}
            - VARIABLE += {integer, VARIABLE, LED[x], random-value, TH, ST}
            - VARIABLE -= {integer, VARIABLE, LED[x], random-value, TH, ST}
            - VARIABLE *= {integer, VARIABLE, LED[x], random-value, TH, ST}
            - VARIABLE /= {integer, VARIABLE, LED[x], random-value, TH, ST}
            - SKIP IF EQUAL {VARIABLE, LED[x]} {integer, VARIABLE, LED[x], random-value, TH, ST}
            - SKIP IF NOT EQUAL {VARIABLE, LED[x]} {integer, VARIABLE, LED[x], random-value, TH, ST}
            - SKIP IF GREATER OR EQUAL {VARIABLE, LED[x]} {integer, VARIABLE, LED[x], random-value, TH, ST}
            - SKIP IF GREATER {VARIABLE, LED[x]} {integer, VARIABLE, LED[x], random-value, TH, ST}
            - SKIP IF SMALLER OR EQUAL  {VARIABLE, LED[x]} {integer, VARIABLE, LED[x], random-value, TH, ST}
            - SKIP IF SMALLER  {VARIABLE, LED[x]} {integer, VARIABLE, LED[x], random-value, TH, ST}
            - SKIP IF ANY {run-state-mask} (compiler shortcut: SKIP IF IS {single-run-state})
            - SKIP IF ALL {run-state-mask}
            - SKIP IF NONE {run-state-mask} (compiler shortcut: SKIP IF NOT {single-run-state})


        - INSTRUCTIONS and OPCODES
            - Every instruction is 4 bytes
            - This means that 1 byte opcode + 3 bytes parameters is feasible
            - End-of-program marker to find different programs in the flash
            - 0x00 and 0xff should not be used (empty flash, 0 initialized)
              for opcodes
            - In order to achieve the instructions requiring run-state-mask,
              which is a UINT32 (almost), we need to separate the highest
              3 bits, and ensure we don't use those in run_state. Actually
              the always_on state can stay at bit 31 as it does not make sense
              for the program.
              Upper 3 bits can not be 111 (assuming 000 is used for other
              opcodes and those have at least one of the lower bits set)
              to ensure we don't have an issue with 0xff and 0x00.
            - For SKIP IF EQ... we need to deal with 16 bit immediates. The
              easiest way is to make separate opcodes for immediates and LEDs,
              and immediates and variables, and led/var led/var.

        * VARIABLES
            - Global pool of variables, assigned at "compile time"
            - Variables are accessible by multiple programs
                * The "compiler" recognize local and global variables
            - Variables are int16_t
                - Because we can have immediates only be 24 bits max anyway
            - Special variable that increments on 6 clicks * "Next sequence"

        * It should be possible to "name" the variables and lights for
          human friendly programming, and to be able to share programs between
          projects without having to adjust each and every opcode

        - The lights used in a program are automatically removed from normal
          car light processing, and from following programs

        - Issue: how to return to the normal program if a light program has
          an IF .. GOTO loop that waits for a certain condition?
            - Only execute a certain number of instructions per systick
        - Programs are active because of an event, or because of a match state
        - Program triggering events
            - Gearbox change event
            - There can only be one event active
            - New events stop currently running events
            - Event programs have priority over other programs regarding light use
        - Run states
            - Any of the car states
            - Any of the priority states like initializing, no-signal
            - Multiple programs can be running in parallel
                - If multiple programs run then the first program using a
                  particular light wins, the other can not use that light


    The first  word of a light program defines the priority states when the
    program is run.

    The second word of a light program defines the car states when the program
    is run. This is mutually exclusive with the priority states.

    The third word  is a bit-field, each bit indicating
    whether the corresponding LED is used by the light program.
    If a light is used that is not specified here, weird things may happen.



******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include <globals.h>
#include <uart0.h>
#include <utils.h>

#define MAX_INSTRUCTIONS_PER_SYSTICK 30

// Pre-defined global variables in var[]
#define GLOBAL_VAR_CLICKS 0
#define GLOBAL_VAR_LIGHT_SWITCH_POSITION 1

typedef struct {
    const uint32_t *PC;
    uint16_t timer;
    unsigned event : 1;
} LIGHT_PROGRAM_CPU_T;

static LIGHT_PROGRAM_CPU_T cpu[MAX_LIGHT_PROGRAMS];
static uint32_t car_state;
static uint32_t run_state;
static uint32_t priority_run_state;

static int16_t var[MAX_LIGHT_PROGRAM_VARIABLES];

extern LED_T light_setpoint[];
extern LED_T light_actual[];
extern uint8_t max_change_per_systick[];
extern uint8_t light_switch_position;


void init_light_programs(void);
void process_light_program_events(void);
uint32_t process_light_programs(void);


// ****************************************************************************
static void reset_program(int n)
{
    cpu[n].PC = light_programs.start[n] + FIRST_OPCODE_OFFSET;
    cpu[n].timer = 0;
    cpu[n].event = 0;
}


// ****************************************************************************
void init_light_programs(void)
{
    int i;

    for (i = 0; i < light_programs.number_of_programs; i++) {
        reset_program(i);
    }
}


// ****************************************************************************
void next_light_sequence(void)
{
	++var[0];
}


// ****************************************************************************
static void load_light_program_environment(void)
{
    priority_run_state = 0;
    if (global_flags.no_signal) {
        priority_run_state |= RUN_WHEN_NO_SIGNAL;
    }
    if (global_flags.initializing) {
        priority_run_state |= RUN_WHEN_INITIALIZING;
    }
    if (global_flags.servo_output_setup == SERVO_OUTPUT_SETUP_CENTRE) {
        priority_run_state |= RUN_WHEN_SERVO_OUTPUT_SETUP_CENTRE;
    }
    if (global_flags.servo_output_setup == SERVO_OUTPUT_SETUP_LEFT) {
        priority_run_state |= RUN_WHEN_SERVO_OUTPUT_SETUP_LEFT;
    }
    if (global_flags.servo_output_setup == SERVO_OUTPUT_SETUP_RIGHT) {
        priority_run_state |= RUN_WHEN_SERVO_OUTPUT_SETUP_RIGHT;
    }
    if (global_flags.reversing_setup & REVERSING_SETUP_STEERING) {
        priority_run_state |= RUN_WHEN_REVERSING_SETUP_STEERING;
    }
    if (global_flags.reversing_setup & REVERSING_SETUP_THROTTLE) {
        priority_run_state |= RUN_WHEN_REVERSING_SETUP_THROTTLE;
    }


    run_state = RUN_ALWAYS;
    run_state |= (RUN_WHEN_LIGHT_SWITCH_POSITION << light_switch_position);
    if (global_flags.forward) {
        run_state |= RUN_WHEN_FORWARD;
    }
    else if (global_flags.reversing) {
        run_state |= RUN_WHEN_REVERSING;
    }
    else {
        run_state |= RUN_WHEN_NEUTRAL;
    }
    if (global_flags.braking) {
        run_state |= RUN_WHEN_BRAKING;
    }
    if (global_flags.blink_flag) {
        run_state |= RUN_WHEN_BLINK_FLAG;
    }
    if (global_flags.blink_indicator_left) {
        run_state |= RUN_WHEN_INDICATOR_LEFT;
        if (global_flags.blink_flag) {
            run_state |= RUN_WHEN_BLINK_LEFT;
        }
    }
    if (global_flags.blink_indicator_right) {
        run_state |= RUN_WHEN_INDICATOR_RIGHT;
        if (global_flags.blink_flag) {
            run_state |= RUN_WHEN_BLINK_RIGHT;
        }
    }
    if (global_flags.blink_hazard) {
        run_state |= RUN_WHEN_HAZARD;
        if (global_flags.blink_flag) {
            run_state |= RUN_WHEN_BLINK_LEFT;
            run_state |= RUN_WHEN_BLINK_RIGHT;
        }
    }


    // car_state is run_state (minus run-always) plus some of the priority
    // run conditions mixed in
    car_state = run_state & ~RUN_ALWAYS;
    if (global_flags.servo_output_setup == SERVO_OUTPUT_SETUP_CENTRE) {
        car_state |= CAR_STATE_SERVO_OUTPUT_SETUP_CENTRE;
    }
    if (global_flags.servo_output_setup == SERVO_OUTPUT_SETUP_LEFT) {
        car_state |= CAR_STATE_SERVO_OUTPUT_SETUP_LEFT;
    }
    if (global_flags.servo_output_setup == SERVO_OUTPUT_SETUP_RIGHT) {
        car_state |= CAR_STATE_SERVO_OUTPUT_SETUP_RIGHT;
    }
    if (global_flags.reversing_setup & REVERSING_SETUP_STEERING) {
        car_state |= CAR_STATE_REVERSING_SETUP_STEERING;
    }
    if (global_flags.reversing_setup & REVERSING_SETUP_THROTTLE) {
        car_state |= CAR_STATE_REVERSING_SETUP_THROTTLE;
    }
}


// ****************************************************************************
static void limit_light_switch_position_variable(void)
{
    if (var[GLOBAL_VAR_LIGHT_SWITCH_POSITION] < 0) {
        var[GLOBAL_VAR_LIGHT_SWITCH_POSITION] = 0;
    }
    if (var[GLOBAL_VAR_LIGHT_SWITCH_POSITION] > config.light_switch_positions) {
        var[GLOBAL_VAR_LIGHT_SWITCH_POSITION] = config.light_switch_positions;
    }
}


// ****************************************************************************
static int16_t get_cmp1(uint32_t instruction)
{
    uint8_t id;

    id = (instruction >> 16) & 0xff;

    // Bit 2 in the opcode field is cleared for VARIABLE, set for LED
    if (instruction & 0x02000000) {
        return light_actual[id];
    }
    else {
        return var[id];
    }
}


// ****************************************************************************
static int16_t get_parameter_value(uint32_t instruction)
{
    uint8_t type;
    // Odd numbered opcodes have an immediate as parameter
    if (instruction & 0x01000000) {
        return (int16_t)(instruction & 0xffff);
    }

    // Even numbered opcodes have either variable, led or random as parameter,
    // determined by param2 of the opcode
    type = instruction >> 8;
    switch (type) {
        case PARAMETER_TYPE_VARIABLE:
            return var[instruction & 0xff];

        case PARAMETER_TYPE_LED:
            return light_actual[instruction & 0xff] * 100 / 255;

        case PARAMETER_TYPE_RANDOM:
            return (int16_t)random_min_max(1, 0xffff);

        case PARAMETER_TYPE_STEERING:
            return channel[ST].normalized;

        case PARAMETER_TYPE_THROTTLE:
            return channel[TH].normalized;

        case PARAMETER_TYPE_GEAR:
            return global_flags.gear;

        default:
            if (diagnostics_enabled()) {
                uart0_send_cstring("UNKNOWN PARAMETER TYPE ");
                uart0_send_uint32(type);
                uart0_send_linefeed();
            }
            return 0;
    }
}


static void execute_skip_if(uint8_t opcode, uint32_t instruction,
    LIGHT_PROGRAM_CPU_T *c)
{
    int16_t cmp1;
    int16_t cmp2;

    cmp1 = get_cmp1(instruction);
    cmp2 = get_parameter_value(instruction);

    switch (opcode) {
        case OPCODE_SKIP_IF_EQ_V:
        case OPCODE_SKIP_IF_EQ_VI:
        case OPCODE_SKIP_IF_EQ_L:
        case OPCODE_SKIP_IF_EQ_LI:
            if (cmp1 == cmp2) {
                ++c->PC;
            }
            break;

        case OPCODE_SKIP_IF_NE_V:
        case OPCODE_SKIP_IF_NE_VI:
        case OPCODE_SKIP_IF_NE_L:
        case OPCODE_SKIP_IF_NE_LI:
            if (cmp1 != cmp2) {
                ++c->PC;
            }
            break;

        case OPCODE_SKIP_IF_GT_V:
        case OPCODE_SKIP_IF_GT_VI:
        case OPCODE_SKIP_IF_GT_L:
        case OPCODE_SKIP_IF_GT_LI:
            if (cmp1 > cmp2) {
                ++c->PC;
            }
            break;

        case OPCODE_SKIP_IF_GE_V:
        case OPCODE_SKIP_IF_GE_VI:
        case OPCODE_SKIP_IF_GE_L:
        case OPCODE_SKIP_IF_GE_LI:
            if (cmp1 >= cmp2) {
                ++c->PC;
            }
            break;

        case OPCODE_SKIP_IF_LT_V:
        case OPCODE_SKIP_IF_LT_VI:
        case OPCODE_SKIP_IF_LT_L:
        case OPCODE_SKIP_IF_LT_LI:
            if (cmp1 < cmp2) {
                ++c->PC;
            }
            break;

        case OPCODE_SKIP_IF_LE_V:
        case OPCODE_SKIP_IF_LE_VI:
        case OPCODE_SKIP_IF_LE_L:
        case OPCODE_SKIP_IF_LE_LI:
            if (cmp1 <= cmp2) {
                ++c->PC;
            }
            break;

        default:
            break;
    }
}


// ****************************************************************************
// Convert percentage into uint8_t 0..255.
// Clamp input between 0 .. 100%
static uint8_t percent_to_uint8(int percentage)
{
    if (percentage < 0) {
        return 0;
    }

    if (percentage >= 100) {
        return 255;
    }

    return percentage * 255 / 100;
}


// ****************************************************************************
static void execute_program(
    const uint32_t *program, LIGHT_PROGRAM_CPU_T *c, uint32_t *leds_used)
{
    uint32_t leds_already_used;
    int instructions_executed = 0;

    leds_already_used = *leds_used;
    *leds_used |= *(program + LEDS_USED_OFFSET);

    if (c->timer) {
        if (--c->timer) {
            return;
        }
    }

    while (instructions_executed < MAX_INSTRUCTIONS_PER_SYSTICK) {
        static uint32_t instruction;
        static uint8_t opcode;

        static uint8_t min;
        static uint8_t max;
        static uint8_t value;

        static uint8_t var_id;
        static int16_t dividend;

        static uint16_t parameter;

        int i;

        ++instructions_executed;

        instruction = *(c->PC++);

        opcode = (instruction >> 24) & 0xff;

        if (opcode >= FIRST_SKIP_IF_OPCODE && opcode <= LAST_SKIP_IF_OPCODE) {
            execute_skip_if(opcode, instruction, c);
            continue;
        }

        if ((opcode & 0xe0) == OPCODE_SKIP_IF_ANY) {
            if (instruction & car_state & 0x1fffffff) {
                c->PC++;
            }
            continue;
        }
        else if ((opcode & 0xe0) == OPCODE_SKIP_IF_ALL) {
            if ((instruction & car_state & 0x1fffffff) ==
                    (instruction & 0x1fffffff)) {
                c->PC++;
            }
            continue;
        }
        else if ((opcode & 0xe0) == OPCODE_SKIP_IF_NONE) {
            if ((instruction & car_state & 0x1fffffff) == 0) {
                c->PC++;
            }
            continue;
        }

        // Fan out commonly used opcode parameters
        max =  var_id = (instruction >> 16) & 0xff;
        min = (instruction >> 8)  & 0xff;
        value = (instruction >> 0)  & 0xff;

        switch (opcode) {
            case OPCODE_SET:
                for (i = min; i <= max; i++) {
                    if ((leds_already_used & (1 << i)) == 0) {
                        light_setpoint[i] = percent_to_uint8(var[value]);
                    }
                }
                break;

            case OPCODE_SET_I:
                for (i = min; i <= max; i++) {
                    if ((leds_already_used & (1 << i)) == 0) {
                        light_setpoint[i] = percent_to_uint8(value);
                    }
                }
                break;

            case OPCODE_FADE:
                for (i = min; i <= max; i++) {
                    if ((leds_already_used & (1 << i)) == 0) {
                        max_change_per_systick[i] =
                            percent_to_uint8(var[value]);
                    }
                }
                break;

            case OPCODE_FADE_I:
                for (i = min; i <= max; i++) {
                    if ((leds_already_used & (1 << i)) == 0) {
                        max_change_per_systick[i] = percent_to_uint8(value);
                    }
                }
                break;

            case OPCODE_SLEEP:
            case OPCODE_SLEEP_I:
                parameter = get_parameter_value(instruction);
                c->timer = (parameter > 0) ? (parameter / 20) : 0;
                return;

            case OPCODE_GOTO:
                c->PC =
                    program + FIRST_OPCODE_OFFSET + (instruction & 0x00ffffff);
                continue;

            case OPCODE_ASSIGN:
            case OPCODE_ASSIGN_I:
                var[var_id] = get_parameter_value(instruction);
                break;

            case OPCODE_ADD:
            case OPCODE_ADD_I:
                var[var_id] += get_parameter_value(instruction);
                break;

            case OPCODE_SUBTRACT:
            case OPCODE_SUBTRACT_I:
                var[var_id] -= get_parameter_value(instruction);
                break;

            case OPCODE_MULTIPLY:
            case OPCODE_MULTIPLY_I:
                var[var_id] *= get_parameter_value(instruction);
                break;

            case OPCODE_DIVIDE:
            case OPCODE_DIVIDE_I:
                dividend = get_parameter_value(instruction);
                if (dividend == 0) {
                    var[var_id] = 0x7fff;   // int16_t max
                }
                else {
                    var[var_id] /= dividend;
                    }
                break;

            case OPCODE_AND:
            case OPCODE_AND_I:
                var[var_id] &= get_parameter_value(instruction);
                break;

            case OPCODE_OR:
            case OPCODE_OR_I:
                var[var_id] |= get_parameter_value(instruction);
                break;

            case OPCODE_XOR:
            case OPCODE_XOR_I:
                var[var_id] ^= get_parameter_value(instruction);
                break;

            case OPCODE_ABS:
                parameter = get_parameter_value(instruction);
                // int16_t requires special handling
                if (parameter & 0x8000) {
                    parameter = ~parameter + 1;
                }
                var[var_id] = parameter;
                break;

            case OPCODE_END_OF_PROGRAM:
                --c->PC;
                c->event = 0;
                return;

            default:
                if (diagnostics_enabled()) {
                    uart0_send_cstring("UNKNOWN OPCODE 0x");
                    uart0_send_uint8_hex(opcode);
                    uart0_send_linefeed();
                }
                c->PC = program + FIRST_OPCODE_OFFSET;
                c->event = 0;
                return;
        }
    }
}


// ****************************************************************************
void process_light_program_events(void)
{
    if (global_flags.gear_changed) {
        int i;
        for (i = 0; i < light_programs.number_of_programs; i++) {
            if (*(light_programs.start[i] + PRIORITY_STATE_OFFSET)
                & RUN_WHEN_GEAR_CHANGED) {
                reset_program(i);
                cpu[i].event = 1;
                break;
            }
        }
    }
}


// ****************************************************************************
uint32_t process_light_programs(void)
{
    int i;
    uint32_t leds_used;

    leds_used = 0;
    load_light_program_environment();

    // Place the current light switch position into a global variable
    // so that light programs can access them
    var[GLOBAL_VAR_LIGHT_SWITCH_POSITION] = light_switch_position;

    // Run all programs that were triggered by an event
    for (i = 0; i < light_programs.number_of_programs; i++) {
        if (cpu[i].event) {
            execute_program(light_programs.start[i], &cpu[i], &leds_used);
            limit_light_switch_position_variable();
        }
    }

    // Run all priority programs where the light controller state matches
    for (i = 0; i < light_programs.number_of_programs; i++) {
        if (*(light_programs.start[i] + PRIORITY_STATE_OFFSET) == RUN_WHEN_NORMAL_OPERATION) {
            continue;
        }

        if (cpu[i].event) {
            continue;
        }

        if (*(light_programs.start[i] + PRIORITY_STATE_OFFSET) &
                priority_run_state) {
            execute_program(light_programs.start[i], &cpu[i], &leds_used);
            limit_light_switch_position_variable();
        }
        else {
            reset_program(i);
        }
    }

    // Run all non-event and non-priority programs
    for (i = 0; i < light_programs.number_of_programs; i++) {
        if (*(light_programs.start[i] + PRIORITY_STATE_OFFSET) != RUN_WHEN_NORMAL_OPERATION) {
            continue;
        }

        if (*(light_programs.start[i] + RUN_STATE_OFFSET) & run_state) {
            execute_program(light_programs.start[i], &cpu[i], &leds_used);
            limit_light_switch_position_variable();
        }
        else {
            reset_program(i);
        }
    }

    // Return the possibly modified value of light switch position
    light_switch_position = var[GLOBAL_VAR_LIGHT_SWITCH_POSITION];

    return leds_used;
}

