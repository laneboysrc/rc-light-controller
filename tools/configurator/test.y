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

  - led, [led ...] = value
  - led, [led ...] = variable
    - These translate into:
      - SET start_led stop_led value
      - SET start_led stop_led VARIABLE

  - VARIABLE = abs(VARIABLE, TH, ST)
  - VARIABLE = {integer, VARIABLE, LED[x], random-value, TH, ST}
  - VARIABLE += {integer, VARIABLE, LED[x], TH, ST}
  - VARIABLE -= {integer, VARIABLE, LED[x], TH, ST}
  - VARIABLE *= {integer, VARIABLE, LED[x], TH, ST}
  - VARIABLE /= {integer, VARIABLE, LED[x], TH, ST}

  - FADE led [led ...] time
  - FADE led [led ...] variable
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
  goto, var, led, wait, skip, if, all, none, not, fade, run, when, or, clicks,
  master, slave, global, random, steering, throttle

  clicks: increments when 6-clicks on CH3
*/



/* ========================================================================== */
/* Prologue */

%{

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>

int yylex(void);
void yyerror(char const *);
char *make_string(char *s1, char *s2);
void set_label(char *name);
void emit(uint32_t instruction);

enum {
  UNKNOWN_PARSE_STATE = 0,
  EXPECTING_RUN_CONDITION_IDENTIFIER,
  EXPECTING_LED_IDENTIFIER,
  EXPECTING_VARIABLE_IDENTIFIER
} parse_state;

typedef struct _identifier {
    char *name;
    int token;
    int index;
    struct _identifier *next;
} identifier;

extern identifier *symbol_table;

#define PARAMETER_TYPE_VARIABLE 0
#define PARAMETER_TYPE_LED 1
#define PARAMETER_TYPE_RANDOM 2
#define PARAMETER_TYPE_STEERING 3
#define PARAMETER_TYPE_THROTTLE 4


%}


/* ========================================================================== */
/* Bison declarations */
%define api.value.type union

%token <identifier *> LED
%token <identifier *> VAR
%token <identifier *> GLOBAL
%token <uint32_t> MASTER
%token <uint32_t> SLAVE

%token <uint32_t> FADE
%token <uint32_t> GOTO
%token <uint32_t> WAIT

%token <int16_t> NUMBER
%token <identifier *> LABEL
%token <identifier *> RANDOM
%token <identifier *> STEERING
%token <identifier *> THROTTLE
%token <identifier *> IDENTIFIER
%token <identifier *> LED_IDENTIFIER
%token <identifier *> VARIABLE_IDENTIFIER
%token <identifier *> CLICKS_IDENTIFIER
%token <uint32_t> PRIORITY_RUN_CONDITION_IDENTIFIER
%token <uint32_t> RUN_CONDITION_IDENTIFIER
%token <uint32_t> RUN_CONDITION_IDENTIFIER_ALWAYS

%token <uint32_t> SKIP
%token <uint32_t> IF
%token <uint32_t> ALL
%token <uint32_t> NONE
%token <uint32_t> NOT
%token <uint32_t> CAR_STATE

%token <uint32_t> RUN
%token <uint32_t> WHEN
%token <uint32_t> OR

%token <uint32_t> MUL_ASSIGN
%token <uint32_t> DIV_ASSIGN
%token <uint32_t> ADD_ASSIGN
%token <uint32_t> SUB_ASSIGN

%type <identifier *> decleration
%type <uint32_t> command
%type <uint32_t> expression led_identifiers
%type <uint32_t> assignment_operator
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

expect_run_condition_identifier
  : %empty  { parse_state = EXPECTING_RUN_CONDITION_IDENTIFIER; }
  ;

expect_variable_identifier
  : %empty  { parse_state = EXPECTING_VARIABLE_IDENTIFIER; }
  ;

expect_led_identifier
  : %empty  { parse_state = EXPECTING_LED_IDENTIFIER; }
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
  : RUN expect_run_condition_identifier WHEN priority_run_condition_identifiers
  ;

priority_run_condition_identifiers
  : PRIORITY_RUN_CONDITION_IDENTIFIER
  | priority_run_condition_identifiers PRIORITY_RUN_CONDITION_IDENTIFIER
  | priority_run_condition_identifiers OR PRIORITY_RUN_CONDITION_IDENTIFIER
  ;

run_condition_lines
  : run_condition_line
  | run_condition_lines run_condition_line
  ;

run_condition_line
  : RUN expect_run_condition_identifier WHEN run_condition_identifiers '\n'
  ;

run_condition_identifiers
  : RUN_CONDITION_IDENTIFIER
  | run_condition_identifiers RUN_CONDITION_IDENTIFIER
  | run_condition_identifiers OR RUN_CONDITION_IDENTIFIER
  ;

run_always_condition_line
  : RUN expect_run_condition_identifier RUN_CONDITION_IDENTIFIER_ALWAYS '\n'
  ;

decleration_lines
  : decleration_line
  | decleration_lines decleration_line
  ;

decleration_line
  : decleration '\n'
        { printf("===========> Decleration: %s\n", $1->name); }
  ;

decleration
  : VAR expect_variable_identifier VARIABLE_IDENTIFIER
  | GLOBAL VAR expect_variable_identifier VARIABLE_IDENTIFIER
  | LED expect_led_identifier LED_IDENTIFIER
  ;

code_lines
  : code_line
  | code_lines code_line
  ;

code_line
  : IDENTIFIER ':' '\n'
      { printf("===========> Label: %s\n", $1->name); set_label($1->name); }
  | command '\n'
  ;

command
  : GOTO LABEL
      { emit(0x01000000 | $1); }
  | FADE
      { emit(0x04000000); }
  | WAIT
      { emit(0x06000000); }
  | expression
  ;

expression
  : VARIABLE_IDENTIFIER assignment_operator variable_assignment_parameter
      { emit($2 | ($1->index << 16) | $3); }
  | led_identifiers '=' led_assignment_parameter
      { emit(0x02000000 | ($1 << 16) | ($1 << 8) | $3); }
  ;

led_identifiers
  : LED_IDENTIFIER
      { $$ = $1->index; }
  | led_identifiers ',' LED_IDENTIFIER
      { $$ = $3->index; }
  ;

led_assignment_parameter
  : NUMBER
      /* All opcodes that work with immediates have the lowest bit set */
      { $$ = 0x01000000 | ($1 & 0xff); }
  | VARIABLE_IDENTIFIER
      { $$ = $1->index; }

variable_assignment_parameter
  : NUMBER
      /* All opcodes that work with immediates have the lowest bit set */
      { $$ = 0x01000000 | ($1 & 0xffff); }
  | VARIABLE_IDENTIFIER
      { $$ = (PARAMETER_TYPE_VARIABLE << 8) | $1->index; }
  | LED_IDENTIFIER
      { $$ = (PARAMETER_TYPE_LED << 8) | $1->index; }
  | STEERING
      { $$ = (PARAMETER_TYPE_STEERING << 8); }
  | THROTTLE
      { $$ = (PARAMETER_TYPE_THROTTLE << 8); }
  | RANDOM
      { $$ = (PARAMETER_TYPE_RANDOM << 8); }
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
  ;

%%

/* ========================================================================== */
/* Epilogue */


const identifier run_condition_tokens[] = {
  {.name = "always", .token = RUN_CONDITION_IDENTIFIER_ALWAYS},

  {.name = "light-switch-position-0", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "light-switch-position-1", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "light-switch-position-2", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "light-switch-position-3", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "light-switch-position-4", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "light-switch-position-5", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "light-switch-position-6", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "light-switch-position-7", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "light-switch-position-8", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "neutral", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "forward", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "reversing", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "braking", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "indicator-left", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "indicator-right", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "hazard", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "blink-flag", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "blink-left", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "blink-right", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "winch-disabled", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "winch-idle", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "winch-in", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "winch-out", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "gear-1", .token = RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "gear-2", .token = RUN_CONDITION_IDENTIFIER, .index = 0},

  {.name = "no-signal", .token = PRIORITY_RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "initializing", .token = PRIORITY_RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "servo-output-setup-centre", .token = PRIORITY_RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "servo-output-setup-left", .token = PRIORITY_RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "servo-output-setup-right", .token = PRIORITY_RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "reversing-setup-steering", .token = PRIORITY_RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "reversing-setup-throttle", .token = PRIORITY_RUN_CONDITION_IDENTIFIER, .index = 0},
  {.name = "gear-changed", .token = PRIORITY_RUN_CONDITION_IDENTIFIER, .index = 0},

  {.name = NULL, .token = EOF, .index = 0},
};

const identifier car_state[] = {
  {.name = "light-switch-position-0", .token = CAR_STATE, .index = 0},
  {.name = "light-switch-position-1", .token = CAR_STATE, .index = 0},
  {.name = "light-switch-position-2", .token = CAR_STATE, .index = 0},
  {.name = "light-switch-position-3", .token = CAR_STATE, .index = 0},
  {.name = "light-switch-position-4", .token = CAR_STATE, .index = 0},
  {.name = "light-switch-position-5", .token = CAR_STATE, .index = 0},
  {.name = "light-switch-position-6", .token = CAR_STATE, .index = 0},
  {.name = "light-switch-position-7", .token = CAR_STATE, .index = 0},
  {.name = "light-switch-position-8", .token = CAR_STATE, .index = 0},
  {.name = "neutral", .token = CAR_STATE, .index = 0},
  {.name = "forward", .token = CAR_STATE, .index = 0},
  {.name = "reversing", .token = CAR_STATE, .index = 0},
  {.name = "braking", .token = CAR_STATE, .index = 0},
  {.name = "indicator-left", .token = CAR_STATE, .index = 0},
  {.name = "indicator-right", .token = CAR_STATE, .index = 0},
  {.name = "hazard", .token = CAR_STATE, .index = 0},
  {.name = "blink-flag", .token = CAR_STATE, .index = 0},
  {.name = "blink-left", .token = CAR_STATE, .index = 0},
  {.name = "blink-right", .token = CAR_STATE, .index = 0},
  {.name = "winch-disabled", .token = CAR_STATE, .index = 0},
  {.name = "winch-idle", .token = CAR_STATE, .index = 0},
  {.name = "winch-in", .token = CAR_STATE, .index = 0},
  {.name = "winch-out", .token = CAR_STATE, .index = 0},
  {.name = "gear-1", .token = CAR_STATE, .index = 0},
  {.name = "gear-2", .token = CAR_STATE, .index = 0},

  {.name = NULL, .token = EOF, .index = 0},
};

const identifier reserved_words[] = {
  {.name = "goto", .token = GOTO, .index = 0},
  {.name = "var", .token = VAR, .index = 0},
  {.name = "led", .token = LED, .index = 0},
  {.name = "wait", .token = WAIT, .index = 0},
  {.name = "skip", .token = SKIP, .index = 0},
  {.name = "if", .token = IF, .index = 0},
  {.name = "all", .token = ALL, .index = 0},
  {.name = "none", .token = NONE, .index = 0},
  {.name = "not", .token = NOT, .index = 0},
  {.name = "fade", .token = FADE, .index = 0},
  {.name = "run", .token = RUN, .index = 0},
  {.name = "when", .token = WHEN, .index = 0},
  {.name = "or", .token = OR, .index = 0},
  {.name = "clicks", .token = CLICKS_IDENTIFIER, .index = 0},
  {.name = "master", .token = MASTER, .index = 0},
  {.name = "slave", .token = SLAVE, .index = 0},
  {.name = "global", .token = GLOBAL, .index = 0},
  {.name = "random", .token = RANDOM, .index = 0},
  {.name = "steering", .token = STEERING, .index = 0},
  {.name = "throttle", .token = THROTTLE, .index = 0},

  {.name = NULL, .token = EOF, .index = 0},
};

identifier *symbol_table = NULL;
identifier *run_condition_table = NULL;
identifier *car_state_table = NULL;
identifier *reserved_words_table = NULL;

int next_variable_index = 1;


identifier *add_symbol(identifier **table, char *name, int token, int index)
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


identifier *get_symbol(identifier **table, const char *name)
{
  identifier *ptr;
  for (ptr = *table; ptr != NULL; ptr = ptr->next) {
    if (strcmp(ptr->name, name) == 0) {
      return ptr;
    }
  }
  return NULL;
}

void set_label(char *name)
{
    identifier *s;
    s = get_symbol(&symbol_table, name);
    if (s) {
        s->token = LABEL;
        printf("++++++++++> Set identifier '%s' as LABEL\n", name);
    }
    else {
        printf("WARNING: label '%s' is not in symbol table?!?\n", name);
    }
}


void initialize_symbol_table(const identifier *source, identifier **destination)
{
    while (source->name) {
        add_symbol(destination, source->name, source->token, 0);
        ++source;
    }
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

    case EXPECTING_RUN_CONDITION_IDENTIFIER:
      printf("----------> Expecting run condition\n");
      break;

    case EXPECTING_VARIABLE_IDENTIFIER:
      printf("----------> Expecting variable identifier\n");
      break;

    case EXPECTING_LED_IDENTIFIER:
      printf("----------> Expecting LED identifier\n");
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
    int count = 0;

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
    int count = 0;
    int id;
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

    if (parse_state == EXPECTING_RUN_CONDITION_IDENTIFIER) {
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
        if (parse_state == EXPECTING_LED_IDENTIFIER) {
            s = add_symbol(&symbol_table, symbuf, LED_IDENTIFIER, 0);
            printf("++++++++++> Added LED %s (%d)\n", s->name, s->token);
        }
        else if (parse_state == EXPECTING_VARIABLE_IDENTIFIER) {
            s = add_symbol(&symbol_table, symbuf, VARIABLE_IDENTIFIER,
                    next_variable_index++);
            printf("++++++++++> Added VARIABLE %s (%d)\n", s->name, s->token);
        }
        else {
            s = add_symbol(&symbol_table, symbuf, IDENTIFIER, 0);
            printf("++++++++++> Added IDENTIFIER %s (%d)\n", s->name, s->token);
        }
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


/* Called by yyparse on error.  */
void yyerror(char const *s)
{
  fprintf(stderr, "ERROR: %s\n", s);
}


void emit(uint32_t instruction)
{
  printf("===============> INSTRUCTION: 0x%08x\n", instruction);
}


int main(int argc, char *argv[])
{
  printf("Bison test parser\n");
  //yydebug = 1;

  initialize_symbol_table(run_condition_tokens, &run_condition_table);
  initialize_symbol_table(reserved_words, &reserved_words_table);
  initialize_symbol_table(car_state, &car_state_table);

  return yyparse();
}
