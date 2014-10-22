#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>

#include "symbols.h"
#include "parser.h"
#include "emitter.h"
#include "log.h"


#define MODULE "symbols"


const char *token2str(int token);
int get_reserved_word(union YYSTYPE *result, const char *yytext);
int get_symbol(union YYSTYPE *result, const char *name);


typedef struct _forward {
    unsigned int location;
    SYMBOL_T *symbol;
    struct _forward *next;
} FORWARD_DECLERATION_T;

typedef struct {
    const char *name;
    int token;
    uint32_t opcode;
} RESERVED_WORD_T;


static SYMBOL_T *symbol_table = NULL;
static FORWARD_DECLERATION_T *forward_declaration_table = NULL;
static int next_variable_index = 0;

static SYMBOL_T undeclared_symbol = {
    .name = NULL,
    .token = UNDECLARED_SYMBOL
};

static RESERVED_WORD_T run_condition_tokens[] = {
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

static RESERVED_WORD_T car_state[] = {
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

static RESERVED_WORD_T reserved_words[] = {
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

    {.name = "=", .token = '=', .opcode = 0x10000000},
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


// ****************************************************************************
static void set_undeclared_symbol(const char *name)
{
    char *name_string;

    if (undeclared_symbol.name != NULL) {
        free((char *)undeclared_symbol.name);
        undeclared_symbol.name = NULL;
    }

    name_string = (char *)calloc(strlen(name), 1);
    if (name_string == NULL) {
        fprintf(stderr,
            "SYMBOLS: ERROR: Out of memory when allocating undeclared symbol name\n");
        exit(1);
    }
    strcpy(name_string, name);

    undeclared_symbol.name = name_string;
    undeclared_symbol.token = UNDECLARED_SYMBOL;
    undeclared_symbol.index = -1;
}


// ****************************************************************************
static void add_forward_declaration(SYMBOL_T *symbol, unsigned int location)
{
    FORWARD_DECLERATION_T *ptr;

    ptr = (FORWARD_DECLERATION_T *)calloc(sizeof(FORWARD_DECLERATION_T), 1);
    if (ptr == NULL) {
        fprintf(stderr,
            "SYMBOLS: ERROR: Out of memory when allocating a forward declaration\n");
        exit(1);
    }

    ptr->symbol = symbol;
    ptr->location = location;
    ptr->next = forward_declaration_table;
    forward_declaration_table = ptr;
}


// ****************************************************************************
void dump_symbol_table(void)
{
    SYMBOL_T *ptr;
    FORWARD_DECLERATION_T *f;

    printf("\n");
    printf("Symbol table:\n");
    for (ptr = symbol_table; ptr != NULL; ptr = ptr->next) {
        printf("name='%s', token=%s index=%d\n",
            ptr->name, token2str(ptr->token), ptr->index);
    }

    printf("\n");
    printf("Forward declarations to resolve:\n");
    if (forward_declaration_table == NULL) {
        printf("(none)\n");
    }
    else {
        for (f = forward_declaration_table; f != NULL; f = f->next) {
            printf("label='%s' location=%u index=%d\n",
                f->symbol->name, f->location, f->symbol->index);
        }
    }
    printf("\n");
}


// ****************************************************************************
void resolve_forward_declarations(uint32_t instructions[])
{
    FORWARD_DECLERATION_T *f;
    for (f = forward_declaration_table; f != NULL; f = f->next) {
        if (f->symbol->index < 0) {
            log_message(MODULE, ERROR,
                "Label '%s' used but not defined.\n", f->symbol->name);
            // FIXME: User YYERROR here!
            // FIXME: track symbol with yylloc
            exit(1);
        }
        else if ((unsigned int)f->symbol->index == f->location) {
            // Skip the declaration of the label
            continue;
        }
        else {
            instructions[f->location] =
                (instructions[f->location] & 0xff000000) |
                    (f->symbol->index & 0xffffff);
        }
    }
}


// ****************************************************************************
void set_symbol(SYMBOL_T *symbol, int token, int index)
{
    if (symbol->index != -1) {
        log_message(MODULE, ERROR,
            "Redefinition of symbol '%s'\n", symbol->name);
        exit(1);
        //FIXME: user YYERROR
    }

    symbol->token = token;
    symbol->index = index;

    log_message(MODULE, INFO, "Set '%s' as token=%s, index=%d\n",
        symbol->name, token2str(symbol->token), symbol->index);
}


// ****************************************************************************
int get_reserved_word(union YYSTYPE *result, const char *yytext)
{
    RESERVED_WORD_T *ptr = reserved_words;

    while (ptr->name != NULL) {
        if (strcmp(ptr->name, yytext) == 0) {
            result->instruction = ptr->opcode;
            return ptr->token;
        }
        ++ptr;
    }

    fprintf(stderr,
        "SYMBOLS: ASSERT: reserved word %s not in the table\n", yytext);
    exit(1);
}


// ****************************************************************************
void add_symbol(const char *name, int token, int index)
{
    SYMBOL_T *ptr;
    char *name_string;

    ptr = (SYMBOL_T *)calloc(sizeof(SYMBOL_T), 1);
    if (ptr == NULL) {
        fprintf(stderr,
            "SYMBOLS: ERROR: Out of memory when allocating a symbol\n");
        exit(1);
    }

    name_string = (char *)calloc(strlen(name), 1);
    if (name_string == NULL) {
        fprintf(stderr,
            "SYMBOLS: ERROR: Out of memory when allocating a symbol name\n");
        exit(1);
    }
    strcpy(name_string, name);

    if (token == VARIABLE  ||  token == GLOBAL_VARIABLE) {
        if (index == -1) {
            index = next_variable_index++;
        }
    }

    ptr->name = name_string;
    ptr->token = token;
    ptr->index = index;
    ptr->next = symbol_table;
    symbol_table = ptr;

    if (ptr->token == LABEL  &&  ptr->index == -1) {
        add_forward_declaration(ptr, pc);
        log_message(MODULE, INFO, "Forward declaration of label %s\n", name);
    }

    log_message(MODULE, INFO, "Added symbol '%s' token=%s, index=%d\n",
        ptr->name, token2str(ptr->token), ptr->index);
}


// ****************************************************************************
int get_symbol(union YYSTYPE *result, const char *name)
{
    SYMBOL_T *ptr;
    RESERVED_WORD_T *r;

    // If we are expect one of the car states or run coditions then check
    // those first. This allows us to be able to use a run condition name
    // also as variable and LED; i.e. we don't pollute so much the reserved
    // word space.
    r = NULL;
    if (parse_state == EXPECTING_RUN_CONDITION) {
        r = run_condition_tokens;
    }
    if (parse_state == EXPECTING_CAR_STATE) {
        r = car_state;
    }

    if (r) {
        while (r->name != NULL) {
            if (strcmp(r->name, name) == 0) {
                result->instruction = r->opcode;
                return r->token;
            }
            ++r;
        }
    }


    // See if we are dealing with a LOCAL symbol
    for (ptr = symbol_table; ptr != NULL; ptr = ptr->next) {
        if (ptr->token == GLOBAL_VARIABLE) {
            continue;
        }
        if (strcmp(ptr->name, name) == 0) {
            if (ptr->token == VARIABLE  ||  ptr->token == LED_ID) {
                result->symbol = ptr;
            }
            else if (ptr->token == LABEL) {
                result->symbol = ptr;
                if (ptr->index == -1) {
                    add_forward_declaration(result->symbol, pc);
                    log_message(MODULE, INFO,
                        "Using forward declared label %s\n", name);
                }
            }
            else {
                fprintf(stderr,
                    "SYMBOLS: ASSERT: Unhandled token '%s' in get_symbol()\n",
                        token2str(ptr->token));
                exit(1);
            }
            return ptr->token;
        }
    }


    // See if we are dealing with a GLOBAL symbol
    for (ptr = symbol_table; ptr != NULL; ptr = ptr->next) {
        if (ptr->token != GLOBAL_VARIABLE) {
            continue;
        }
        if (strcmp(ptr->name, name) == 0) {
            result->symbol = ptr;
            return ptr->token;
        }
    }

    log_message(MODULE, INFO, "Undeclared symbol '%s'\n", name);

    set_undeclared_symbol(name);
    result->symbol = &undeclared_symbol;
    return UNDECLARED_SYMBOL;
}


// ****************************************************************************
void initialize_symbols(void)
{
    // Pre-load global special variable named "clicks" that increments
    // on every six CH3-clicks.
    add_symbol("clicks", GLOBAL_VARIABLE, next_variable_index++);
}