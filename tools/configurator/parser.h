#pragma once

#include <stdint.h>

#define PARAMETER_TYPE_VARIABLE 0
#define PARAMETER_TYPE_LED 1
#define PARAMETER_TYPE_RANDOM 2
#define PARAMETER_TYPE_STEERING 3
#define PARAMETER_TYPE_THROTTLE 4

enum {
  UNKNOWN_PARSE_STATE = 0,
  EXPECTING_RUN_CONDITION,
} parse_state;

typedef struct _identifier {
    char *name;
    int token;
    int index;
    struct _identifier *next;
} identifier;

typedef struct {
    const char *name;
    int token;
} identifier_initializer;

extern identifier *symbol_table;
extern unsigned int pc;           // "Program Counter"

int yylex(void);
void yyerror(const char *);
void set_identifier(identifier *id, int token, int index);
void emit(uint32_t instruction);
const char *token2str(int token);


