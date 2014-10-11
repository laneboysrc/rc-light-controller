/******************************************************************************

    Light programs:
        * Bogdan's idea regarding flame simulation, depending on throttle
        - Programs reside at the end of flash space
        - The maximum number of programs is predetermined as we need to
          assign memory like program counter to each program, and so far
          there is no heap (malloc)
        * Mini programming language
            - GOTO to implement loops
            - SET start_led stop_led value
            - SET start_led stop_led VARIABLE
            - FADE start_led stop_led time
            - FADE start_led stop_led VARIABLE
            - WAIT time
            - WAIT VARIABLE
            * VARIABLE = {integer, VARIABLE, LED[x], random-value}
            * VARIABLE += {integer, VARIABLE, LED[x]}
            * VARIABLE -= {integer, VARIABLE, LED[x]}
            * VARIABLE *= {integer, VARIABLE, LED[x]}
            * VARIABLE /= {integer, VARIABLE, LED[x]}
            * SKIP IF EQUAL {integer, VARIABLE, LED[x]} {integer, VARIABLE, LED[x]} 
            * SKIP IF NOT EQUAL {integer, VARIABLE, LED[x]} {integer, VARIABLE, LED[x]} 
            * SKIP IF GREATER OR EQUAL {integer, VARIABLE, LED[x]} {integer, VARIABLE, LED[x]} 
            * SKIP IF GREATER {integer, VARIABLE, LED[x]} {integer, VARIABLE, LED[x]} 
            * SKIP IF SMALLER OR EQUAL {integer, VARIABLE, LED[x]} {integer, VARIABLE, LED[x]} 
            * SKIP IF SMALLER {integer, VARIABLE, LED[x]} {integer, VARIABLE, LED[x]} 
            * SKIP IF ANY {run-state-mask} (compiler shortcut: SKIP IF {single-run-state}) 
            * SKIP IF ALL {run-state-mask} 
            * SKIP IF NONE {run-state-mask} (compiler shortcut: SKIP IF NOT {single-run-state}) 


        * INSTRUCTIONS and OPCODES
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
                 

        * VARIABLES
            * Global pool of variables, assigned at "compile time"
            * Does it make sense to have the same variable be accessible
              by multiple programs?
                * Possibly, so lets make the "compiler" recognize local and
                  global variables
            * Variables are int16_t
                * Because we can have immediates only be 24 bits max anyway
            * Special variable that increments on 6 clicks * "Next sequence"
                          

        * It should be possible to "name" the variables and lights for
          human friendly programming, and to be able to share programs between
          projects without having to adjust each and every opcode

        - The lights used in a program are automatically removed from normal
          car light processing, and from following programs

        - Issue: how to return to the normal program if a light program has
          an IF .. GOTO loop that waits for a certain condition?
            * Only execute a certain number of instructions per systick
        - Programs are active because of an event, or because of a match state
        - Program triggering events
            * Gearbox change event
            * There can only be one event active
            * New events stop currently running events
            * Event programs have priority over other programs regarding light use
        - Program states
            * Any of the car states
                - light switch position[9]
                - tail light (shortcut to light switch position > 0)
                - neutral
                - forward
                - reversing
                - braking
                - indicator left (static flag)
                - indicator left (static flag)
                - hazard (static flag)
                - blink flag
                - blink indicator left
                - blink indicator right
                - gear 1
                - gear 2
                - winch states (including disabled)
            - Multiple programs can be running in parallel
                * If multiple programs run then the first program using a
                  particular light wins, the other can not use that light


    The first word of a light program is a bit-field, each bit indicating
    whether the corresponding LED is used by the light program.
    If a light is used that is not specified here, weird things may happen.

    The second word defines the states and events that cause the program to
    be ran.

    Test programs:
        Night rider with LEDs 0..3:
            0:  0x0000000f
            1:  0x........
            2:  SET     LED0 LED3 0
            3:  FADE    LED0    120
            4:  FADE    LED1    120
            5:  FADE    LED2    120
            6:  FADE    LED3    120
            7:  SET     LED0    255
            8:  WAIT    120
            9:  SET     LED0    0
            10: SET     LED1    255
            11: WAIT    120
            12: SET     LED1    0
            13: SET     LED2    255
            14: WAIT    120
            15: SET     LED2    0
            16: SET     LED3    255
            17: WAIT    120
            18: SET     LED3    0
            19: SET     LED2    255
            20: WAIT    120
            21: SET     LED2    0
            22: SET     LED1    255
            23: WAIT    120
            24: SET     LED1    0
            25: GOTO    7


******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include <globals.h>
#include <uart0.h>

#define MAX_INSTRUCTIONS_PER_SYSTICK 30


typedef struct {
    const uint32_t *PC;
    uint16_t timer;
    unsigned event : 1;
} LIGHT_PROGRAM_CPU_T;

static LIGHT_PROGRAM_CPU_T cpu[MAX_LIGHT_PROGRAMS];
static uint32_t run_state;
static uint32_t priority_run_state;

static uint16_t var[MAX_LIGHT_PROGRAM_VARIABLES];

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

    run_state |= (RUN_WHEN_LIGHT_SWITCH_POSITION << light_switch_position);

    if (global_flags.gear) {
        run_state |= RUN_WHEN_GEAR_1;
    }
    else {
        run_state |= RUN_WHEN_GEAR_2;
    }
}


// ****************************************************************************
static void execute_program(
    const uint32_t *program, LIGHT_PROGRAM_CPU_T *c, uint32_t *leds_used)
{
    uint32_t instruction;
    uint8_t opcode;
    uint8_t min;
    uint8_t max;
    uint8_t value;
    int i;
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
        ++instructions_executed;

        instruction = *(c->PC++);

        opcode = (instruction >> 24) & 0xff;

        // Fan out commonly used opcode parameters
        max = (instruction >> 16) & 0xff;
        min  = (instruction >> 8)  & 0xff;
        value = (instruction >> 0)  & 0xff;

        switch (opcode) {
            case OPCODE_SET:
                for (i = min; i <= max; i++) {
                    if ((leds_already_used & (1 << i)) == 0) {
                        light_setpoint[i] = value;
                    }
                }
                break;

            case OPCODE_FADE:
                for (i = min; i <= max; i++) {
                    if ((leds_already_used & (1 << i)) == 0) {
                        max_change_per_systick[i] = value;
                    }
                }
                break;

            case OPCODE_GOTO:
                c->PC =
                    program + FIRST_OPCODE_OFFSET + (instruction & 0x00ffffff);
                continue;

            case OPCODE_WAIT:
                c->timer = (instruction & 0x0000ffff);
                return;

            case OPCODE_END_OF_PROGRAM:
                --c->PC;
                c->event = 0;
                return;

            default:
                uart0_send_cstring("UNKNOWN OPCODE 0x");
                uart0_send_uint8_hex(opcode);
                uart0_send_linefeed();
                c->PC = program + FIRST_OPCODE_OFFSET;
                c->event = 0;
                return;
        }
    }
}


// ****************************************************************************
void process_light_program_events(void)
{
    int i;
    if (global_flags.gear_changed) {
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

    // Run all programs that were triggered by an event
    for (i = 0; i < light_programs.number_of_programs; i++) {
        if (cpu[i].event) {
            execute_program(light_programs.start[i], &cpu[i], &leds_used);
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
        }
        else {
            reset_program(i);
        }
    }

    return leds_used;
}

