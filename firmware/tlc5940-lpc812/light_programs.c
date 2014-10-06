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


    Test programs:
        Night rider with LEDs 0..3:
            0:  SET LED0 LED3 0
            1:  FADE    LED0    120
            2:  FADE    LED1    120
            3:  FADE    LED2    120
            4:  FADE    LED3    120
            5:  SET     LED0    255
            6:  WAIT    120
            7:  SET     LED0    0
            8:  SET     LED1    255
            9:  WAIT    120
            10: SET     LED1    0
            11: SET     LED2    255
            12: WAIT    120
            13: SET     LED2    0
            14: SET     LED3    255
            15: WAIT    120
            16: SET     LED3    0
            17: SET     LED2    255
            18: WAIT    120
            19: SET     LED2    0
            20: SET     LED1    255
            21: WAIT    120
            22: SET     LED1    0
            23: GOTO    5


******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include <globals.h>
#include <uart0.h>

void process_light_programs(void);


void process_light_programs(void)
{
    const uint32_t *program_pointer = light_programs.programs;
    while (*program_pointer != 0xffffffff) {
        uart0_send_cstring("OPCODE: ");
        uart0_send_uint32_hex(*program_pointer);
        uart0_send_linefeed();
        ++program_pointer;
    }        
}

