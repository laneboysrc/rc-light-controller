/******************************************************************************

    Light programs:
        * Bogdan's idea regarding flame simulation, depending on throttle
        * Programs reside at the end of flash space
        * The maximum number of programs is predetermined as we need to 
          assign memory like program counter to each program, and so far
          there is no heap (malloc)
        * Mini programming language
            * GOTO to implement loops
            * SET start_led stop_led value
            * FADE start_led stop_led time
            * WAIT time
            * IF condition (skips next instruction if false)
                * VARIABLE == integer, VAR2
                * VARIABLE > integer, VAR2
                * VARIABLE < integer, VAR2
                * VARIABLE != integer, VAR2
                * car state: implement as AND and OR mask?
                * ANY car state
                * ALL car state
                * NONE car state
            * IF NOT condition (skips next instruction if true)
            * VARIABLE = integer
            * VARIABLE += integer (signed, so -= can work with same opcode!)
            * Reading values of lights?

            * Different sequence depending on gear value
                * Gear 1/2 is one of the states
                * Can also be read as a variable at program execution time?
            * Different sequence depending on state of roof lights
                * Can be done by reading actual LED values back
            * Random value
            * "Next sequence"
            - 0x00 and 0xff should not be used (empty flash, 0 initialized)
              for opcodes
        * Every opcode is 4 bytes
            * This means that 1 byte command + 3 bytes parameters is feasible
            * End-of-program marker to find different programs in the flash
        * The lights used in a program are automatically removed from normal
          car light processing, and from following programs
        * Issue: how to return to the normal program if a light program has
          an IF .. GOTO loop that waits for a certain condition?
            * Return if false?
            * Detect if a GOTO lands on a IF?
            * Return after a number of instructions?
                * Should be feasible because we are executing in a while loop,
                  so we can decrement a max_instructions counter
        * Programs are active because of an event, or because of a match state
        * Program triggering events
            * Gearbox change event
            * There can only be one event active
            * New events stop currently running events
            * Event programs have priority over other programs regarding light use
        * Programs states
            * Is it beneficial to do a AND and XOR mask to check for a combination
              of states, including NOT?
        
            * Always
            * Winch active
            * Any of the car states
                * light switch position[9]
                * tail light (shortcut to light switch position > 0)
                * neutral (not available yet)
                * forward
                * reversing
                * braking
                * indicator left (static flag)
                * indicator left (static flag)
                * hazard (static flag)
                * blink flag
                * blink indicator left
                * blink indicator right
                * gear 1
                * gear 2
            * Multiple programs can be running in parallel
                * If multiple programs run then the first program using a
                  particular light wins, the other can not use that light
        Program metadata
            * FLASH: State or event the program runs
            * FLASH: LEDs used (16 + 32 + 16 + 32 = 96 bits = 12 bytes)
            * FLASH: RAM used
            * RAM: Shadow values for all used LEDs
            * RAM: fade time, start time, led, start value, end value
            * RAM: program counter
                * Gets reset every time a program is not running
            * RAM: variables


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
    uint32_t timer;
    bool running;
} LIGHT_PROGRAM_CPU_T;

static LIGHT_PROGRAM_CPU_T cpu[MAX_LIGHT_PROGRAMS];
static uint32_t environment;


extern LED_T light_setpoint[];
extern LED_T light_actual[];
extern uint8_t max_change_per_systick[];
extern uint8_t light_switch_position;


uint32_t process_light_programs(void);
void init_light_programs(void);


// ****************************************************************************
static void reset_program(int program_number)
{
    cpu[program_number].PC = light_programs.start[program_number] + 2;
    cpu[program_number].running = false;
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
static uint32_t get_current_environment(void)
{
    uint32_t state;
/*
    Bit 0:      Always
    Bit 1:      Winch disarmed
    Bit 2:      Winch idle
    Bit 3:      Winch in
    Bit 4:      Winch out
    Bit 5..13:  light switch position[9]
    Bit 14:     tail light (shortcut to light switch position > 0)
    Bit 15:     neutral
    Bit 16:     forward
    Bit 17:     reversing
    Bit 18:     braking
    Bit 19:     indicator left (static flag)
    Bit 20:     indicator left (static flag)
    Bit 21:     hazard (static flag)
    Bit 22:     blink flag (toggles with 1.5 Hz)
    Bit 23:     blink indicator left (may be indicator or hazard, combined with blink_flag)
    Bit 24:     blink indicator right
    Bit 25:     gear 1
    Bit 26:     gear 2
    
    5 bits left (for events?)
*/
    state = (1 << 0);   // Run progrogram always
    
    if (global_flags.forward) {
        state |= (1 << 16);
    }
    else if (global_flags.reversing) {
        state |= (1 << 17);
    }
    else {
        state |= (1 << 15);
    }

    if (global_flags.blink_flag) {
        state |= (1 << 22);
    }

    if (global_flags.blink_indicator_left) {
        state |= (1 << 19);
        if (global_flags.blink_flag) {
            state |= (1 << 23);
        }
    }

    if (global_flags.blink_indicator_right) {
        state |= (1 << 20);
        if (global_flags.blink_flag) {
            state |= (1 << 24);
        }
    }

    if (global_flags.blink_hazard) {
        state |= (1 << 22);
        if (global_flags.blink_flag) {
            state |= (1 << 23);
            state |= (1 << 24);
        }
    }

    if (light_switch_position > 0) {
        state |= (1 << 14);
    }

    state |= ((1 << 5) << light_switch_position);

    if (global_flags.gear) {
        state |= (1 << 26);
    }
    else {
        state |= (1 << 25);
    }

    return state;
}


// ****************************************************************************
static void execute_program(
    int program_number, LIGHT_PROGRAM_CPU_T *c, uint32_t *leds_used)
{
    uint32_t instruction;
    uint8_t min;
    uint8_t max;
    uint8_t value;
    int i;
    uint32_t leds_already_used;
    int instructions_executed = 0;

    leds_already_used = *leds_used;
    *leds_used |= *(light_programs.start[program_number] + 1);

    if (c->timer) {
        --c->timer;
        return;
    }

    while (instructions_executed < MAX_INSTRUCTIONS_PER_SYSTICK) {
        ++instructions_executed;
        instruction = *(c->PC++);
        
        // Fan out commonly used opcode parameters
        max = (instruction >> 16) & 0xff;
        min  = (instruction >> 8)  & 0xff;
        value = (instruction >> 0)  & 0xff;

        switch (instruction & OPCODE_MASK) {
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
                c->PC = light_programs.start[program_number] +
                    (instruction & ~OPCODE_MASK);
                continue;

            case OPCODE_WAIT:
                c->timer = (instruction & ~OPCODE_MASK);
                return;

            case OPCODE_END_OF_PROGRAM:
                --c->PC;
                return;

            default:
                uart0_send_cstring("UNKNOWN OPCODE 0x");
                uart0_send_uint32_hex(instruction);
                uart0_send_linefeed();
                reset_program(program_number);
                return;
        }
    }
}


// ****************************************************************************
uint32_t process_light_programs(void)
{
    int i;
    uint32_t leds_used;

    leds_used = 0;
    environment = get_current_environment();

    for (i = 0; i < light_programs.number_of_programs; i++) {
        if (*light_programs.start[i] & environment) {
            execute_program(i, &cpu[i], &leds_used);
        }
        else {
            reset_program(i);
        }
    }

    return leds_used;
}

