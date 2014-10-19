#pragma once

#include <stdint.h>

#define PARAMETER_TYPE_VARIABLE 0
#define PARAMETER_TYPE_LED 1
#define PARAMETER_TYPE_RANDOM 2
#define PARAMETER_TYPE_STEERING 3
#define PARAMETER_TYPE_THROTTLE 4

#define INSTRUCTION_END_OF_PROGRAM 0xfe000000
#define INSTRUCTION_MODIFIER_LED 0x02000000
#define INSTRUCTION_MODIFIER_IMMEDIATE 0x01000000
#define INSTRUCTION_RUN_ALWAYS (1 << 31)
#define INSTRUCTION_RUN_WHEN_NORMAL_OPERATION 0x00000000

enum {
    UNKNOWN_PARSE_STATE = 0,
    EXPECTING_RUN_CONDITION,
    EXPECTING_CAR_STATE,
    EXPECTING_LABEL
} parse_state;

typedef struct _identifier {
    const char *name;
    int token;
    int index;
    uint32_t opcode;
    struct _identifier *next;
} identifier;

void yyerror(const char *);

void initialize_symbols(void);
void set_identifier(identifier *id, int token, int index);
const char *token2str(int token);

void add_led_to_list(int led_index);
void emit(uint32_t instruction);
void emit_led_instruction(uint32_t instruction);

extern unsigned int pc;           // "Program Counter"

