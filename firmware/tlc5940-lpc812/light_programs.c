/******************************************************************************

    Light programs:
        * Bogdan's idea regarding flame simulation, depending on throttle
        * Programs reside at the end of flash space
        * Do we need to limit the number of programs?
            * Most likely yes, otherwise we need to scan the whole flash
              every systick to find all programs
        * Mini programming language
            * GOTO to implement loops
            * SET led value (monochrome, translates to command below)
            * SET start_led stop_led value
            * FADE led time (translates to command below)
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

            * Different sequence depending on gear value?
            * Different sequence depending on state of roof lights?
            * Random value
            * "Next sequence"
            * 0x00 and 0xff should not be used (empty flash, 0 initialized)
        * Every opcode is 4 bytes
            * This means that 1 byte command + 3 bytes RGB is possible
            * End-of-program marker to find different programs in the flash
        * The lights used in a program are automatically removed from normal
          car light processing
        * Issue: how to return to the normal program if a light program has
          an IF .. GOTO loop that waits for a certain condition?
            * Return if false?
            * Detect if a GOTO lands on a IF?
            * Return after a number of instructions?
        * Programs are active because of an event, or because of a match state
        * Program triggering events
            * Gearbox change event
            * There can only be one event active
            * New events stop currently running events
            * Event programs have priority over other programs regarding light use
        * Programs states
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

typedef struct {
    const uint32_t *PC;
    uint32_t timer;
    bool running;
} LIGHT_PROGRAM_CPU_T;

static LIGHT_PROGRAM_CPU_T cpu[MAX_LIGHT_PROGRAMS];

extern LED_T tlc5940_light_data[16];

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
static uint32_t get_current_state(void)
{
    return 0x00000001;
}


// ****************************************************************************
static void execute_program(
    int program_number, LIGHT_PROGRAM_CPU_T *c)
{
    uint32_t instruction;
    uint8_t min;
    uint8_t max;
    uint8_t value;
    int i;

    if (c->timer) {
        --c->timer;
        return;
    }

    while (1) {
        instruction = *(c->PC++);

        switch (instruction & OPCODE_MASK) {
            case OPCODE_SET:
                max = (instruction >> 16) & 0xff;
                min  = (instruction >> 8)  & 0xff;
                value = (instruction >> 0)  & 0xff;
                for (i = min; i <= max; i++) {
                    tlc5940_light_data[i] = value;
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
    uint32_t state;
    uint32_t leds_used;

    leds_used = 0;
    state = get_current_state();

    for (i = 0; i < light_programs.number_of_programs; i++) {
        if (*light_programs.start[i] & state) {
            leds_used |= *(light_programs.start[i] + 1);
            execute_program(i, &cpu[i]);
        }
        else {
            reset_program(i);
        }
    }

    return leds_used;
}

