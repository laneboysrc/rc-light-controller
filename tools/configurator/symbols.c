#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>

#include "symbols.h"
#include "parser.h"

typedef struct {
    const char *name;
    int token;
    uint32_t opcode;
} identifier_initializer;

static identifier *symbol_table = NULL;

int get_reserved_word(union YYSTYPE *result, const char *yytext);
int get_symbol(union YYSTYPE *result, const char *name);

identifier run_condition_tokens[] = {
    {.name = "always", .token = RUN_CONDITION_ALWAYS, .opcode = (1 << 31)},

    {.name = "light-switch-position-0", .token = RUN_CONDITION, .opcode = (1 << 0)},
    {.name = "light-switch-position-1", .token = RUN_CONDITION, .opcode = (1 << 1)},
    {.name = "light-switch-position-2", .token = RUN_CONDITION, .opcode = (1 << 2)},
    {.name = "light-switch-position-3", .token = RUN_CONDITION, .opcode = (1 << 3)},
    {.name = "light-switch-position-4", .token = RUN_CONDITION, .opcode = (1 << 4)},
    {.name = "light-switch-position-5", .token = RUN_CONDITION, .opcode = (1 << 5)},
    {.name = "light-switch-position-6", .token = RUN_CONDITION, .opcode = (1 << 6)},
    {.name = "light-switch-position-7", .token = RUN_CONDITION, .opcode = (1 << 7)},
    {.name = "light-switch-position-8", .token = RUN_CONDITION, .opcode = (1 << 8)},

    {.name = "neutral", .token = RUN_CONDITION, .opcode = (1 << 9)},
    {.name = "forward", .token = RUN_CONDITION, .opcode = (1 << 10)},
    {.name = "reversing", .token = RUN_CONDITION, .opcode = (1 << 11)},
    {.name = "braking", .token = RUN_CONDITION, .opcode = (1 << 12)},

    {.name = "indicator-left", .token = RUN_CONDITION, .opcode = (1 << 13)},
    {.name = "indicator-right", .token = RUN_CONDITION, .opcode = (1 << 14)},
    {.name = "hazard", .token = RUN_CONDITION, .opcode = (1 << 15)},
    {.name = "blink-flag", .token = RUN_CONDITION, .opcode = (1 << 16)},
    {.name = "blink-left", .token = RUN_CONDITION, .opcode = (1 << 17)},
    {.name = "blink-right", .token = RUN_CONDITION, .opcode = (1 << 18)},

    {.name = "winch-disabled", .token = RUN_CONDITION, .opcode = (1 << 19)},
    {.name = "winch-idle", .token = RUN_CONDITION, .opcode = (1 << 20)},
    {.name = "winch-in", .token = RUN_CONDITION, .opcode = (1 << 21)},
    {.name = "winch-out", .token = RUN_CONDITION, .opcode = (1 << 22)},

    {.name = "gear-1", .token = RUN_CONDITION, .opcode = (1 << 23)},
    {.name = "gear-2", .token = RUN_CONDITION, .opcode = (1 << 24)},

    {.name = "no-signal", .token = PRIORITY_RUN_CONDITION, .opcode = (1 << 0)},
    {.name = "initializing", .token = PRIORITY_RUN_CONDITION, .opcode = (1 << 1)},
    {.name = "servo-output-setup-centre", .token = PRIORITY_RUN_CONDITION, .opcode = (1 << 2)},
    {.name = "servo-output-setup-left", .token = PRIORITY_RUN_CONDITION, .opcode = (1 << 3)},
    {.name = "servo-output-setup-right", .token = PRIORITY_RUN_CONDITION, .opcode = (1 << 4)},
    {.name = "reversing-setup-steering", .token = PRIORITY_RUN_CONDITION, .opcode = (1 << 5)},
    {.name = "reversing-setup-throttle", .token = PRIORITY_RUN_CONDITION, .opcode = (1 << 6)},
    {.name = "gear-changed", .token = PRIORITY_RUN_CONDITION, .opcode = (1 << 7)},

    {.name = NULL, .token = EOF},
};

identifier car_state[] = {
    {.name = "light-switch-position-0", .token = CAR_STATE, .opcode = (1 << 0)},
    {.name = "light-switch-position-1", .token = CAR_STATE, .opcode = (1 << 1)},
    {.name = "light-switch-position-2", .token = CAR_STATE, .opcode = (1 << 2)},
    {.name = "light-switch-position-3", .token = CAR_STATE, .opcode = (1 << 3)},
    {.name = "light-switch-position-4", .token = CAR_STATE, .opcode = (1 << 4)},
    {.name = "light-switch-position-5", .token = CAR_STATE, .opcode = (1 << 5)},
    {.name = "light-switch-position-6", .token = CAR_STATE, .opcode = (1 << 6)},
    {.name = "light-switch-position-7", .token = CAR_STATE, .opcode = (1 << 7)},
    {.name = "light-switch-position-8", .token = CAR_STATE, .opcode = (1 << 8)},

    {.name = "neutral", .token = CAR_STATE, .opcode = (1 << 9)},
    {.name = "forward", .token = CAR_STATE, .opcode = (1 << 10)},
    {.name = "reversing", .token = CAR_STATE, .opcode = (1 << 11)},
    {.name = "braking", .token = CAR_STATE, .opcode = (1 << 12)},

    {.name = "indicator-left", .token = CAR_STATE, .opcode = (1 << 13)},
    {.name = "indicator-right", .token = CAR_STATE, .opcode = (1 << 14)},
    {.name = "hazard", .token = CAR_STATE, .opcode = (1 << 15)},
    {.name = "blink-flag", .token = CAR_STATE, .opcode = (1 << 16)},
    {.name = "blink-left", .token = CAR_STATE, .opcode = (1 << 17)},
    {.name = "blink-right", .token = CAR_STATE, .opcode = (1 << 18)},

    {.name = "winch-disabled", .token = CAR_STATE, .opcode = (1 << 19)},
    {.name = "winch-idle", .token = CAR_STATE, .opcode = (1 << 20)},
    {.name = "winch-in", .token = CAR_STATE, .opcode = (1 << 21)},
    {.name = "winch-out", .token = CAR_STATE, .opcode = (1 << 22)},

    {.name = "gear-1", .token = CAR_STATE, .opcode = (1 << 23)},
    {.name = "gear-2", .token = CAR_STATE, .opcode = (1 << 24)},

    {.name = NULL, .token = EOF},
};

identifier reserved_words[] = {
    {.name = "goto", .token = GOTO, .opcode = 0x01000000},
    {.name = "var", .token = VAR},
    {.name = "led", .token = LED},
    {.name = "fade", .token = FADE, .opcode = 0x04000000},
    {.name = "wait", .token = WAIT, .opcode = 0x06000000},
    {.name = "skip", .token = SKIP},
    {.name = "if", .token = IF},
    {.name = "any", .token = ANY, .opcode = 0x60000000},
    {.name = "all", .token = ALL, .opcode = 0x80000000},
    {.name = "none", .token = NONE, .opcode = 0xa0000000},
    {.name = "not", .token = NOT, .opcode = 0xa0000000},
    {.name = "is", .token = IS, .opcode = 0x60000000},
    {.name = "run", .token = RUN},
    {.name = "when", .token = WHEN},
    {.name = "or", .token = OR},
    {.name = "master", .token = MASTER},
    {.name = "slave", .token = SLAVE},
    {.name = "global", .token = GLOBAL},
    {.name = "random", .token = RANDOM},
    {.name = "steering", .token = STEERING},
    {.name = "throttle", .token = THROTTLE},
    {.name = "abs", .token = ABS, .opcode = 0x40000000},

    {.name = "=", .token = ASSIGN, .opcode = 0x10000000},
    {.name = ">", .token = GT, .opcode = 0x2c000000},
    {.name = "<", .token = LT, .opcode = 0x34000000},
    {.name = "+=", .token = ADD_ASSIGN, .opcode = 0x12000000},
    {.name = "-=", .token = SUB_ASSIGN, .opcode = 0x14000000},
    {.name = "*=", .token = MUL_ASSIGN, .opcode = 0x16000000},
    {.name = "/=", .token = DIV_ASSIGN, .opcode = 0x18000000},
    {.name = "&=", .token = AND_ASSIGN, .opcode = 0x1a000000},
    {.name = "|=", .token = OR_ASSIGN, .opcode = 0x1c000000},
    {.name = "^=", .token = XOR_ASSIGN, .opcode = 0x1e000000},
    {.name = "==", .token = EQ, .opcode = 0x20000000},
    {.name = "!=", .token = NE, .opcode = 0x24000000},
    {.name = ">=", .token = GE, .opcode = 0x28000000},
    {.name = "<=", .token = LE, .opcode = 0x30000000},

    {.name = NULL, .token = EOF},
};



unsigned int pc = 0;
static int next_variable_index = 0;


// ****************************************************************************
void set_identifier(identifier *s, int token, int index)
{
    s->token = token;
    s->index = (index != -1) ? index : next_variable_index++;

   fprintf(stderr, "SYMBOLS: Set '%s' as token=%s, index=%d\n",
        s->name, token2str(s->token), s->index);
}


// ****************************************************************************
int get_reserved_word(union YYSTYPE *result, const char *yytext)
{
    identifier *w = reserved_words;

    while (w->name != NULL) {
        if (strcmp(w->name, yytext) == 0) {
            result->instruction = w->opcode;
            return w->token;
        }
        ++w;
    }

    fprintf(stderr,
        "SYMBOLS: ERROR: reserved word %s not in the table\n", yytext);
    exit(1);
}


// ****************************************************************************
static int add_symbol(union YYSTYPE *result, const char *name, int token, int index)
{
    identifier *ptr;
    char *name_string;

    ptr = (identifier *)calloc(sizeof(identifier), 1);
    if (ptr == NULL) {
        fprintf(stderr,
            "SYMBOLS: ERROR: Out of memory when allocating an identifier\n");
        exit(1);
    }

    name_string = (char *)calloc(strlen(name) + 1, 1);
    if (name_string == NULL) {
        fprintf(stderr,
            "SYMBOLS: ERROR: Out of memory when allocating an identifier name\n");
        exit(1);
    }
    strcpy(name_string, name);

    ptr->name = name_string;
    ptr->token = token;
    ptr->index = index;
    ptr->next = symbol_table;
    symbol_table = ptr;

    result->i = ptr;
    return token;
}


// ****************************************************************************
int get_symbol(union YYSTYPE *result, const char *name)
{
    identifier *ptr;
    int token;

    ptr = NULL;
    if (parse_state == EXPECTING_RUN_CONDITION) {
        ptr = run_condition_tokens;
    }
    if (parse_state == EXPECTING_CAR_STATE) {
        ptr = car_state;
    }

    if (ptr) {
        while (ptr->name != NULL) {
            if (strcmp(ptr->name, name) == 0) {
                result->instruction = ptr->opcode;
                return ptr->token;
            }
            ++ptr;
        }
    }

    for (ptr = symbol_table; ptr != NULL; ptr = ptr->next) {
        if (strcmp(ptr->name, name) == 0) {
            if (ptr->token == IDENTIFIER  ||
                ptr->token == VARIABLE  ||
                ptr->token == LED_ID  ||
              ptr->token == LABEL) {
                result->i = ptr;
            }
            else {
                result->instruction = ptr->opcode;
            }
            return ptr->token;
        }
    }

    token = (parse_state == EXPECTING_LABEL) ? LABEL : IDENTIFIER;

    return add_symbol(result, name, token, 0);
}


// ****************************************************************************
void initialize_symbols(void)
{
    union YYSTYPE dummy;

    // Pre-load global special variable named "clicks" that increments
    // on every six CH3-clicks.
    add_symbol(&dummy, "clicks", VARIABLE, next_variable_index++);
}
