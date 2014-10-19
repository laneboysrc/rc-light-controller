#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "light_programs.tab.h"

#define NUMBER_OF_LEDS 32

typedef struct {
    int count;
    uint8_t elements[NUMBER_OF_LEDS];
} led_list_t;

static led_list_t led_list;


// ****************************************************************************
void add_led_to_list(int led_index)
{
    if (led_list.count < NUMBER_OF_LEDS) {
        led_list.elements[led_list.count++] = led_index;
    }
    else {
        printf("####################> ERROR: led_list is full\n");
        exit(1);
    }
}

// ****************************************************************************
void emit_led_instruction(uint32_t instruction)
{
    printf("####################> LED instruction: 0x%08x\n", instruction);
    led_list.count = 0;
    ++pc;
}

// ****************************************************************************
void emit(uint32_t instruction)
{
    printf("####################> INSTRUCTION: 0x%08x\n", instruction);
    ++pc;
}


// ****************************************************************************
void yyerror(const char *s)
{
    fprintf(stderr, "ERROR: %s\n", s);
}


// ****************************************************************************
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("Bison test parser\n");
    yydebug = 1;

    initialize_lexer();
    led_list.count = 0;

    return yyparse();
}
