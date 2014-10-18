/* Bison test */

/*

programs:
  programs | program;

program:
  @empty | run-conditions declerations code;

run-conditions:
  run-conditions | run-condition;


declerations:
  - LED name = master[0..15]|slave[0..15]
  - [GLOBAL] VAR name

code:
  - LABEL: Labels for GOTO
    - Need to detect duplicate definitions
    - Labels are always local to a program

  - GOTO label
    - NOTE: do not allow numbers as one line may translate to several opcodes!
    - ISSUE: needs linking due to forward decleration
    - ISSUE: label may never be defined, linker needs to detect!
    - IDEA: store all used locations in a list to be able to cross-reference

  - led [, led ...] = value
  - led [, led ...] = variable
    - These translate into:
      - SET start_led stop_led value
      - SET start_led stop_led VARIABLE

  - VARIABLE = abs(VARIABLE, TH, ST)
  - VARIABLE = {integer, VARIABLE, LED[x], random-value, TH, ST}
  - VARIABLE += {integer, VARIABLE, LED[x], TH, ST}
  - VARIABLE -= {integer, VARIABLE, LED[x], TH, ST}
  - VARIABLE *= {integer, VARIABLE, LED[x], TH, ST}
  - VARIABLE /= {integer, VARIABLE, LED[x], TH, ST}

  - FADE led [, led ...] time
  - FADE led [, led ...] variable
    - These translate into:
      - FADE start_led stop_led time
      - FADE start_led stop_led VARIABLE

  - WAIT time
  - WAIT VARIABLE

  - SKIP IF {VARIABLE, LED[x]} == {integer, VARIABLE, LED[x]}
  - SKIP IF {VARIABLE, LED[x]} != {integer, VARIABLE, LED[x]}
  - SKIP IF {VARIABLE, LED[x]} > {integer, VARIABLE, LED[x]}
  - SKIP IF {VARIABLE, LED[x]} >= {integer, VARIABLE, LED[x]}
  - SKIP IF {VARIABLE, LED[x]} < {integer, VARIABLE, LED[x]}
  - SKIP IF {VARIABLE, LED[x]} <= {integer, VARIABLE, LED[x]}
    - These translate into:
      - SKIP IF EQUAL {VARIABLE, LED[x]} {integer, VARIABLE, LED[x]}
      - SKIP IF NOT EQUAL {VARIABLE, LED[x]} {integer, VARIABLE, LED[x]}
      - SKIP IF GREATER OR EQUAL {VARIABLE, LED[x]} {integer, VARIABLE, LED[x]}
      - SKIP IF GREATER {VARIABLE, LED[x]} {integer, VARIABLE, LED[x]}
      - SKIP IF SMALLER OR EQUAL  {VARIABLE, LED[x]} {integer, VARIABLE, LED[x]}
      - SKIP IF SMALLER  {VARIABLE, LED[x]} {integer, VARIABLE, LED[x]}

  - SKIP IF ANY {car-state} [{car-state} ...]
  - SKIP IF {car-state}
    - Translates into SKIP IF ANY

  - SKIP IF ALL {car-state} [{car-state} ...]

  - SKIP IF NONE {car-state} [{car-state} ...]
  - SKIP IF NOT {car-state}
    - Translates into SKIP IF NONE

run-condition:
| RUN_WHEN_LIGHT_SWITCH_POSITION
| RUN_WHEN_LIGHT_SWITCH_POSITION_1
| RUN_WHEN_LIGHT_SWITCH_POSITION_2
| RUN_WHEN_LIGHT_SWITCH_POSITION_3
| RUN_WHEN_LIGHT_SWITCH_POSITION_4
| RUN_WHEN_LIGHT_SWITCH_POSITION_5
| RUN_WHEN_LIGHT_SWITCH_POSITION_6
| RUN_WHEN_LIGHT_SWITCH_POSITION_7
| RUN_WHEN_LIGHT_SWITCH_POSITION_8
| RUN_WHEN_NEUTRAL
| RUN_WHEN_FORWARD
| RUN_WHEN_REVERSING
| RUN_WHEN_BRAKING
| RUN_WHEN_INDICATOR_LEFT
| RUN_WHEN_INDICATOR_RIGHT
| RUN_WHEN_HAZARD
| RUN_WHEN_BLINK_FLAG
| RUN_WHEN_BLINK_LEFT
| RUN_WHEN_BLINK_RIGHT
| RUN_WHEN_WINCH_DISABLERD
| RUN_WHEN_WINCH_IDLE
| RUN_WHEN_WINCH_IN
| RUN_WHEN_WINCH_OUT
| RUN_WHEN_GEAR_1
| RUN_WHEN_GEAR_2
| RUN_ALWAYS

| RUN_WHEN_NO_SIGNAL
| RUN_WHEN_INITIALIZING
| RUN_WHEN_SERVO_OUTPUT_SETUP_CENTRE
| RUN_WHEN_SERVO_OUTPUT_SETUP_LEFT
| RUN_WHEN_SERVO_OUTPUT_SETUP_RIGHT
| RUN_WHEN_REVERSING_SETUP_STEERING
| RUN_WHEN_REVERSING_SETUP_THROTTLE
| RUN_WHEN_GEAR_CHANGED

  - car-state:
    - LIGHT_SWITCH_POSITION_0
    - LIGHT_SWITCH_POSITION_1
    - LIGHT_SWITCH_POSITION_2
    - LIGHT_SWITCH_POSITION_3
    - LIGHT_SWITCH_POSITION_4
    - LIGHT_SWITCH_POSITION_5
    - LIGHT_SWITCH_POSITION_6
    - LIGHT_SWITCH_POSITION_7
    - LIGHT_SWITCH_POSITION_8
    - NEUTRAL
    - FORWARD
    - REVERSING
    - BRAKING
    - INDICATOR_LEFT
    - INDICATOR_RIGHT
    - HAZARD
    - BLINK_FLAG
    - BLINK_LEFT
    - BLINK_RIGHT
    - WINCH_DISABLERD
    - WINCH_IDLE
    - WINCH_IN
    - WINCH_OUT
    - GEAR_1
    - GEAR_2


reserved keywords:
  goto, var, led, wait, skip, if, all, none, not, fade, run, when, or,
  master, slave, global, random, steering, throttle, abs

  clicks: increments when 6-clicks on CH3. pre-defined global variable
*/



/* ========================================================================== */
/* Prologue */

%{

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>

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


%}


/* ========================================================================== */
/* Bison declarations */
%define api.value.type union

%locations

%token <identifier *> LED
%token <identifier *> VAR
%token <identifier *> GLOBAL
%token <uint32_t> MASTER
%token <uint32_t> SLAVE

%token <uint32_t> FADE
%token <uint32_t> GOTO
%token <uint32_t> WAIT

%token <int16_t> NUMBER
%token <identifier *> IDENTIFIER
%token <identifier *> LABEL
%token <identifier *> RANDOM
%token <identifier *> STEERING
%token <identifier *> THROTTLE
%token <identifier *> LED_ID
%token <identifier *> VARIABLE
%token <uint32_t> CAR_STATE
%token <uint32_t> PRIORITY_RUN_CONDITION
%token <uint32_t> RUN_CONDITION
%token <uint32_t> RUN_CONDITION_ALWAYS

%token <uint32_t> SKIP
%token <uint32_t> IF
%token <uint32_t> ALL
%token <uint32_t> NONE
%token <uint32_t> NOT

%token <uint32_t> RUN
%token <uint32_t> WHEN
%token <uint32_t> OR

%token <uint32_t> MUL_ASSIGN
%token <uint32_t> DIV_ASSIGN
%token <uint32_t> ADD_ASSIGN
%token <uint32_t> SUB_ASSIGN
%token <uint32_t> AND_ASSIGN
%token <uint32_t> OR_ASSIGN
%token <uint32_t> XOR_ASSIGN
%token <uint32_t> ABS

%type <identifier *> decleration
%type <uint32_t> command
%type <uint32_t> expression master_or_slave leds
%type <uint32_t> assignment_operator abs_assignment_parameter
%type <uint32_t> variable_assignment_parameter led_assignment_parameter

%start program

%%

/* ========================================================================== */
/* Grammar rules */

program
  : condition_lines decleration_lines code_lines
      { emit(0xfe000000); }
  | condition_lines code_lines
      { emit(0xfe000000); }
  | %empty
  ;

expect_run_condition
  : %empty  { parse_state = EXPECTING_RUN_CONDITION; }
  ;

condition_lines
  : priority_run_condition_lines
        { printf("===========> SET priority run\n"); }
  | run_condition_lines
        { printf("===========> SET run\n"); }
  | run_always_condition_line
        { printf("===========> SET run always\n"); }
  ;

priority_run_condition_lines
  : priority_run_condition_line
  | priority_run_condition_lines priority_run_condition_line
  ;

priority_run_condition_line
  : priority_run_condition '\n'
  ;

priority_run_condition
  : RUN expect_run_condition WHEN priority_run_conditions
  ;

priority_run_conditions
  : PRIORITY_RUN_CONDITION
  | priority_run_conditions PRIORITY_RUN_CONDITION
  | priority_run_conditions OR PRIORITY_RUN_CONDITION
  ;

run_condition_lines
  : run_condition_line
  | run_condition_lines run_condition_line
  ;

run_condition_line
  : RUN expect_run_condition WHEN run_conditions '\n'
  ;

run_conditions
  : RUN_CONDITION
  | run_conditions RUN_CONDITION
  | run_conditions OR RUN_CONDITION
  ;

run_always_condition_line
  : RUN expect_run_condition RUN_CONDITION_ALWAYS '\n'
  ;

decleration_lines
  : decleration_line
  | decleration_lines decleration_line
  ;

decleration_line
  : decleration '\n'
  ;

decleration
  : VAR IDENTIFIER
      { set_identifier($2, VARIABLE, -1); }
  | GLOBAL VAR IDENTIFIER
      { set_identifier($3, VARIABLE, -1); }
  | LED IDENTIFIER '=' master_or_slave
      { set_identifier($2, LED_ID, $4); }
  ;

master_or_slave
  : MASTER '[' NUMBER ']'
      { $$ = $3; }
  | SLAVE '[' NUMBER ']'
      { $$ = $3 + 16; }
  ;

code_lines
  : code_line
  | code_lines code_line
  ;

code_line
  : IDENTIFIER ':' '\n'
      { set_identifier($1, LABEL, pc); }
  | command '\n'
  ;

command
  : GOTO LABEL /* FIXME: need to be able to deal with not yet defined labels */
      { emit(0x01000000 | $2->index); }
  | FADE
      { emit(0x04000000); }
  | WAIT
      { emit(0x06000000); }
  | expression
  ;

expression
  : VARIABLE assignment_operator variable_assignment_parameter
      { emit($2 | ($1->index << 16) | $3); }
  | VARIABLE assignment_operator ABS abs_assignment_parameter
      { emit(0x40000000 | $4); }
  | leds '=' led_assignment_parameter
      { emit(0x02000000 | ($1 << 16) | ($1 << 8) | $3); }
  ;

leds
  : LED_ID
      { $$ = $1->index; }
  | leds ',' LED_ID
      { $$ = $3->index; }
  ;

led_assignment_parameter
  : NUMBER
      /* All opcodes that work with immediates have the lowest bit set */
      { $$ = 0x01000000 | ($1 & 0xff); }
  | VARIABLE
      { $$ = $1->index; }

variable_assignment_parameter
  : NUMBER
      /* All opcodes that work with immediates have the lowest bit set */
      { $$ = 0x01000000 | ($1 & 0xffff); }
  | VARIABLE
      { $$ = (PARAMETER_TYPE_VARIABLE << 8) | $1->index; }
  | LED_ID
      { $$ = (PARAMETER_TYPE_LED << 8) | $1->index; }
  | STEERING
      { $$ = (PARAMETER_TYPE_STEERING << 8); }
  | THROTTLE
      { $$ = (PARAMETER_TYPE_THROTTLE << 8); }
  | RANDOM
      { $$ = (PARAMETER_TYPE_RANDOM << 8); }
  ;

abs_assignment_parameter
  : VARIABLE
      { $$ = (PARAMETER_TYPE_VARIABLE << 8) | $1->index; }
  | STEERING
      { $$ = (PARAMETER_TYPE_STEERING << 8); }
  | THROTTLE
      { $$ = (PARAMETER_TYPE_THROTTLE << 8); }
  ;

assignment_operator
  : '='
        { $$ = 0x10000000; }
  | ADD_ASSIGN
        { $$ = 0x12000000; }
  | SUB_ASSIGN
        { $$ = 0x14000000; }
  | MUL_ASSIGN
        { $$ = 0x16000000; }
  | DIV_ASSIGN
        { $$ = 0x18000000; }
  | AND_ASSIGN
        { $$ = 0x1a000000; }
  | OR_ASSIGN
        { $$ = 0x1c000000; }
  | XOR_ASSIGN
        { $$ = 0x1e000000; }
  ;

%%

/* ========================================================================== */
/* Epilogue */


identifier_initializer run_condition_tokens[] = {
  {.name = "always", .token = RUN_CONDITION_ALWAYS},

  {.name = "light-switch-position-0", .token = RUN_CONDITION},
  {.name = "light-switch-position-1", .token = RUN_CONDITION},
  {.name = "light-switch-position-2", .token = RUN_CONDITION},
  {.name = "light-switch-position-3", .token = RUN_CONDITION},
  {.name = "light-switch-position-4", .token = RUN_CONDITION},
  {.name = "light-switch-position-5", .token = RUN_CONDITION},
  {.name = "light-switch-position-6", .token = RUN_CONDITION},
  {.name = "light-switch-position-7", .token = RUN_CONDITION},
  {.name = "light-switch-position-8", .token = RUN_CONDITION},
  {.name = "neutral", .token = RUN_CONDITION},
  {.name = "forward", .token = RUN_CONDITION},
  {.name = "reversing", .token = RUN_CONDITION},
  {.name = "braking", .token = RUN_CONDITION},
  {.name = "indicator-left", .token = RUN_CONDITION},
  {.name = "indicator-right", .token = RUN_CONDITION},
  {.name = "hazard", .token = RUN_CONDITION},
  {.name = "blink-flag", .token = RUN_CONDITION},
  {.name = "blink-left", .token = RUN_CONDITION},
  {.name = "blink-right", .token = RUN_CONDITION},
  {.name = "winch-disabled", .token = RUN_CONDITION},
  {.name = "winch-idle", .token = RUN_CONDITION},
  {.name = "winch-in", .token = RUN_CONDITION},
  {.name = "winch-out", .token = RUN_CONDITION},
  {.name = "gear-1", .token = RUN_CONDITION},
  {.name = "gear-2", .token = RUN_CONDITION},

  {.name = "no-signal", .token = PRIORITY_RUN_CONDITION},
  {.name = "initializing", .token = PRIORITY_RUN_CONDITION},
  {.name = "servo-output-setup-centre", .token = PRIORITY_RUN_CONDITION},
  {.name = "servo-output-setup-left", .token = PRIORITY_RUN_CONDITION},
  {.name = "servo-output-setup-right", .token = PRIORITY_RUN_CONDITION},
  {.name = "reversing-setup-steering", .token = PRIORITY_RUN_CONDITION},
  {.name = "reversing-setup-throttle", .token = PRIORITY_RUN_CONDITION},
  {.name = "gear-changed", .token = PRIORITY_RUN_CONDITION},

  {.name = NULL, .token = EOF},
};

identifier_initializer car_state[] = {
  {.name = "light-switch-position-0", .token = CAR_STATE},
  {.name = "light-switch-position-1", .token = CAR_STATE},
  {.name = "light-switch-position-2", .token = CAR_STATE},
  {.name = "light-switch-position-3", .token = CAR_STATE},
  {.name = "light-switch-position-4", .token = CAR_STATE},
  {.name = "light-switch-position-5", .token = CAR_STATE},
  {.name = "light-switch-position-6", .token = CAR_STATE},
  {.name = "light-switch-position-7", .token = CAR_STATE},
  {.name = "light-switch-position-8", .token = CAR_STATE},
  {.name = "neutral", .token = CAR_STATE},
  {.name = "forward", .token = CAR_STATE},
  {.name = "reversing", .token = CAR_STATE},
  {.name = "braking", .token = CAR_STATE},
  {.name = "indicator-left", .token = CAR_STATE},
  {.name = "indicator-right", .token = CAR_STATE},
  {.name = "hazard", .token = CAR_STATE},
  {.name = "blink-flag", .token = CAR_STATE},
  {.name = "blink-left", .token = CAR_STATE},
  {.name = "blink-right", .token = CAR_STATE},
  {.name = "winch-disabled", .token = CAR_STATE},
  {.name = "winch-idle", .token = CAR_STATE},
  {.name = "winch-in", .token = CAR_STATE},
  {.name = "winch-out", .token = CAR_STATE},
  {.name = "gear-1", .token = CAR_STATE},
  {.name = "gear-2", .token = CAR_STATE},

  {.name = NULL, .token = EOF},
};

identifier_initializer reserved_words[] = {
  {.name = "goto", .token = GOTO},
  {.name = "var", .token = VAR},
  {.name = "led", .token = LED},
  {.name = "wait", .token = WAIT},
  {.name = "skip", .token = SKIP},
  {.name = "if", .token = IF},
  {.name = "all", .token = ALL},
  {.name = "none", .token = NONE},
  {.name = "not", .token = NOT},
  {.name = "fade", .token = FADE},
  {.name = "run", .token = RUN},
  {.name = "when", .token = WHEN},
  {.name = "or", .token = OR},
  {.name = "master", .token = MASTER},
  {.name = "slave", .token = SLAVE},
  {.name = "global", .token = GLOBAL},
  {.name = "random", .token = RANDOM},
  {.name = "steering", .token = STEERING},
  {.name = "throttle", .token = THROTTLE},
  {.name = "abs", .token = ABS},

  {.name = NULL, .token = EOF},
};

identifier *symbol_table = NULL;
identifier *run_condition_table = NULL;
identifier *car_state_table = NULL;
identifier *reserved_words_table = NULL;

int next_variable_index = 0;
unsigned int pc = 0;


static identifier *add_symbol(identifier **table, const char *name, int token, int index)
{
  identifier *ptr = (identifier *)calloc(sizeof(identifier), 1);
  //FIXME: check allocation fail
  ptr->name = (char *)calloc(strlen(name) + 1, 1);
  //FIXME: check allocation fail
  strcpy(ptr->name, name);
  ptr->token = token;
  ptr->index = index;
  ptr->next = *table;
  *table = ptr;
  return ptr;
}


static identifier *get_symbol(identifier **table, const char *name)
{
  identifier *ptr;
  for (ptr = *table; ptr != NULL; ptr = ptr->next) {
    if (strcmp(ptr->name, name) == 0) {
      return ptr;
    }
  }
  return NULL;
}


void set_identifier(identifier *s, int token, int index)
{
    s->token = token;
    s->index = (index != -1) ? index : next_variable_index++;

    printf("++++++++++> Set IDENTIFIER '%s' as token=%d, index=%d\n",
      s->name, s->token, s->index);
}


static void initialize_symbol_table(const identifier_initializer *source,
  identifier **destination)
{
    while (source->name) {
        add_symbol(destination, source->name, source->token, 0);
        ++source;
    }
}


void emit(uint32_t instruction)
{
  printf("===============> INSTRUCTION: 0x%08x\n", instruction);
  ++pc;
}


void yyerror(const char *s)
{
  fprintf(stderr, "ERROR: %s\n", s);
}


int yylex(void)
{
  static size_t length = 40;
  static char *symbuf = NULL;
  int c;
  static int empty_line = 1;

  if (symbuf == NULL){
    symbuf = (char *)calloc(length + 1, 1);
    // FIXME: need to add check for malloc failed...
  }

  switch (parse_state) {
    case UNKNOWN_PARSE_STATE:
      break;

    case EXPECTING_RUN_CONDITION:
      printf("----------> Expecting run condition\n");
      break;

    default:
      printf("----------> FORGOT CASE FOR %d\n", parse_state);
      break;
  }


  /* Skip white space and empty lines */
  if (empty_line) {
    while ((c = getchar ()) == ' ' || c == '\t' || c == '\n') {
      continue;
    }
  }
  else {
    while ((c = getchar ()) == ' ' || c == '\t') {
      continue;
    }
  };

  empty_line = 0;


  // ===========================================================================
  // NUMBERS

  if (isdigit(c)) {
    size_t count = 0;

    do {
      if (count == length) {
        length *= 2;
        symbuf = (char *) realloc(symbuf, length + 1);
        // FIXME: need to add check for malloc failed...
      }

      // Add this character to the buffer.
      symbuf[count++] = c;

      c = getchar();
    } while (isdigit(c));

    ungetc(c, stdin);
    symbuf[count] = '\0';

    parse_state = UNKNOWN_PARSE_STATE;
    yylval.NUMBER = (int16_t)strtol(symbuf, NULL, 10);
    return NUMBER;
  }

  // ===========================================================================
  // IDENTIFIERS

  if (isalpha(c)) {
    size_t count = 0;
    identifier *s;

    do {
      if (count == length) {
        length *= 2;
        symbuf = (char *) realloc(symbuf, length + 1);
        // FIXME: need to add check for malloc failed...
      }

      // Add this character to the buffer.
      symbuf[count++] = c;

      c = getchar();
    } while (isalnum(c) || c == '-');

    ungetc(c, stdin);
    symbuf[count] = '\0';

    // Check RUN CONDITION special symbols if we are expecting any

    if (parse_state == EXPECTING_RUN_CONDITION) {
      printf("++++++++++> Testing RUN CONDITION %s\n", symbuf);
      s = get_symbol(&run_condition_table, symbuf);
      if (s) {
          printf("++++++++++> Found RUN CONDITION %s (%d)\n",
            s->name, s->token);
        yylval.IDENTIFIER = s;
        return s->token;
      }
    }

    // Check reserved words

    printf("++++++++++> Testing RESERVED WORD %s\n", symbuf);
    s = get_symbol(&reserved_words_table, symbuf);
    if (s) {
      printf("++++++++++> Found RESERVED WORD %s (%d)\n", s->name, s->token);
      yylval.IDENTIFIER = s;
      return s->token;
    }

    // Check labels, variables and LEDs

    printf("++++++++++> Testing IDENTIFIER %s\n", symbuf);
    s = get_symbol(&symbol_table, symbuf);
    if (s == NULL) {
        s = add_symbol(&symbol_table, symbuf, IDENTIFIER, 0);
        printf("++++++++++> Added IDENTIFIER %s (%d)\n", s->name, s->token);
    }
    else {
      printf("++++++++++> Found IDENTIFIER %s (%d)\n", s->name, s->token);
    }
    yylval.IDENTIFIER = s;
    return s->token;
  }

  if (c == '\n') {
    empty_line = 1;
    parse_state = UNKNOWN_PARSE_STATE;
    return c;
  }

  if (c == '+') {
    char n;
    if ((n = getchar()) == '=') {
      return ADD_ASSIGN;
    }
    ungetc(n, stdin);
  }

  if (c == '-') {
    char n;
    if ((n = getchar()) == '=') {
      return SUB_ASSIGN;
    }
    ungetc(n, stdin);
  }

  if (c == '*') {
    char n;
    if ((n = getchar()) == '=') {
      return MUL_ASSIGN;
    }
    ungetc(n, stdin);
  }

  if (c == '/') {
    char n;
    if ((n = getchar()) == '=') {
      return DIV_ASSIGN;
    }
    ungetc(n, stdin);
  }

  if (c == '&') {
    char n;
    if ((n = getchar()) == '=') {
      return AND_ASSIGN;
    }
    ungetc(n, stdin);
  }

  if (c == '|') {
    char n;
    if ((n = getchar()) == '=') {
      return OR_ASSIGN;
    }
    ungetc(n, stdin);
  }

  if (c == '^') {
    char n;
    if ((n = getchar()) == '=') {
      return XOR_ASSIGN;
    }
    ungetc(n, stdin);
  }

  if (c == ':') {
    parse_state = UNKNOWN_PARSE_STATE;
    return c;
  }

  if (c == EOF) {
    parse_state = UNKNOWN_PARSE_STATE;
    return 0;
  }

  return c;
}


int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  printf("Bison test parser\n");
  yydebug = 1;

  initialize_symbol_table(run_condition_tokens, &run_condition_table);
  initialize_symbol_table(reserved_words, &reserved_words_table);
  initialize_symbol_table(car_state, &car_state_table);

  // Pre-load global special variable named "clicks" that increments
  // on every six CH3-clicks.
  add_symbol(
    &symbol_table, "clicks", VARIABLE, next_variable_index++);

  return yyparse();
}
