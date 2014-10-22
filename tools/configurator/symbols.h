#pragma once

#include <stdint.h>


enum {
    UNKNOWN_PARSE_STATE = 0,
    EXPECTING_RUN_CONDITION,
    EXPECTING_CAR_STATE,
} parse_state;


typedef struct _symbol {
    const char *name;
    int token;
    int index;
    struct _symbol *next;
} SYMBOL_T;

#include "parser.h"


void initialize_symbols(void);
void add_symbol(const char *name, int token, int index, YYLTYPE *loc);
void set_symbol(SYMBOL_T *symbol, int token, int index, YYLTYPE *loc);
void dump_symbol_table(void);
void remove_local_symbols(void);
void resolve_forward_declarations(uint32_t instructions[]);
uint32_t get_leds_used(void);
