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
void set_identifier(identifier *id, int token, int index);

