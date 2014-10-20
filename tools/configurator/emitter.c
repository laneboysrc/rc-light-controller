#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "emitter.h"


#define NUMBER_OF_LEDS 32

typedef struct {
    int count;
    uint8_t elements[NUMBER_OF_LEDS];
} led_list_t;

static led_list_t led_list;


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
void emit(uint32_t instruction)
{
    printf("INSTRUCTION: 0x%08x\n", instruction);
    ++pc;
}

// ****************************************************************************
void initialize_emitter(void)
{
    led_list.count = 0;
}
