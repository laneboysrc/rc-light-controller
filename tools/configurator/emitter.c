#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "symbols.h"
#include "emitter.h"


// LPC812 has 16 Kbytes flash, 4 bytes per "instruction".
// In reality the flash is of course only partly available...
#define MAX_NUMBER_OF_INSTRUCTIONS (16 * 1024 / 4)

#define NUMBER_OF_LEDS 32

typedef struct {
    int count;
    uint8_t elements[NUMBER_OF_LEDS];
} led_list_t;

unsigned int pc = 0;

static led_list_t led_list;

static uint32_t *instruction_list;
static uint32_t *last_instruction;


// ****************************************************************************
void add_led_to_list(int led_index)
{
    int i;

    // Discard duplicates
    for (i = 0; i < led_list.count; i++) {
        if (led_list.elements[i] == led_index) {
            fprintf(stderr, "WARNING: Duplicate LED %d in list\n",
                led_index);
            return;
        }
    }


    if (led_list.count < NUMBER_OF_LEDS) {
        led_list.elements[led_list.count++] = led_index;
    }
    else {
        fprintf(stderr, "ERROR: led_list is full\n");
        exit(1);
    }
}


// ****************************************************************************
void emit_led_instruction(uint32_t instruction)
{
    int i;
    int n;
    int count;
    uint8_t *ptr;
    uint8_t start;
    uint8_t stop;
    uint8_t temp;

    fprintf(stderr, "INFO: LED instruction: 0x%08x (%d leds)\n",
        instruction, led_list.count);

    if (led_list.count == 0) {
        fprintf(stderr, "ERROR: led_list.count is 0!\n");
        exit(1);
    }

    // Step 1: Simple Bubble Sort to sort LEDs by their index.
    count = led_list.count;
    ptr = led_list.elements;

    n = count;
    do {
        int new_n;
        new_n = 0;
        for (i = 1; i < n; i++) {
            if (ptr[i - 1] > ptr[i]) {
                temp = ptr[i - 1];
                ptr[i - 1] = ptr[i];
                ptr[i] = temp;
                new_n = i;
            }
        }
        n = new_n;
    } while (n != 0);

    // Step 2: Iterate through all items. If discontinuity is found emit a
    //         single LED statement.

    start = stop = ptr[0];
    for (i = 1; i < count; i++) {
        if (ptr[i] != (stop + 1)) {
            emit(instruction | (stop << 16) | (start << 8));
            start = stop = ptr[i];
        }
        else {
            ++stop;
        }
    }
    emit(instruction | (stop << 16) | (start << 8));

    led_list.count = 0;
    ++pc;
}


// ****************************************************************************
void emit_run_condition(uint32_t priority, uint32_t run)
{
    fprintf(stderr, "PRIORITY   : 0x%08x\n", priority);
    fprintf(stderr, "RUN        : 0x%08x\n", run);

    *last_instruction++ = priority;
    *last_instruction++ = run;
}

// ****************************************************************************
void emit(uint32_t instruction)
{
    fprintf(stderr, "INSTRUCTION: 0x%08x\n", instruction);

    *last_instruction++ = instruction;
    ++pc;
}


// ****************************************************************************
void initialize_emitter(void)
{
    led_list.count = 0;

    instruction_list = (uint32_t *)calloc(
        sizeof(uint32_t), MAX_NUMBER_OF_INSTRUCTIONS);

    if (instruction_list == NULL) {
        fprintf(stderr, "ERROR: Not enough memory to allocation instruction cache\n");
        exit(1);
    }

    last_instruction = instruction_list;
}


// ****************************************************************************
void output_programs(void)
{
    uint32_t *ptr = instruction_list;

    dump_symbol_table();

    while (ptr != last_instruction) {
        printf("0x%08x,\n", *ptr++);
    }
}
