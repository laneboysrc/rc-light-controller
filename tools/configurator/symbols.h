#pragma once

#include <stdint.h>


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


void initialize_symbols(void);
void add_symbol(const char *name, int token, int index);
void set_symbol(identifier *id, int token, int index);
void dump_symbol_table(void);
void resolve_forward_declarations(uint32_t instructions[]);
