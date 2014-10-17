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
  - VAR name [GLOBAL]

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

int yylex(void);
void yyerror(char const *);
char *make_string(char *s1, char *s2);

enum {
  UNKNOWN = 0,
  EXPECTING_RUN_CONDITION_IDENTIFIER,
  EXPECTING_LABEL,
} parse_state;


%}


/* ========================================================================== */
/* Bison declarations */
%define api.value.type {char *}

%token LED
%token VAR

%token FADE
%token GOTO
%token WAIT

%token NUMBER
%token LABEL
%token RANDOM
%token STEERING
%token THROTTLE
%token IDENTIFIER
%token LED_IDENTIFIER
%token VARIABLE_IDENTIFIER
%token CLICKS_IDENTIFIER
%token PRIORITY_RUN_CONDITION_IDENTIFIER
%token RUN_CONDITION_IDENTIFIER
%token RUN_CONDITION_IDENTIFIER_ALWAYS

%token MASTER
%token SLAVE
%token GLOBAL

%token SKIP
%token IF
%token ALL
%token NONE
%token NOT
%token CAR_STATE

%token RUN
%token WHEN
%token OR

%token MUL_ASSIGN
%token DIV_ASSIGN
%token ADD_ASSIGN
%token SUB_ASSIGN

%start program

%%

/* ========================================================================== */
/* Grammar rules */

program
  : condition_lines decleration_lines code_lines
  | condition_lines code_lines
  | %empty
  ;

expect_run_condition_identifier
  : %empty  { parse_state = EXPECTING_RUN_CONDITION_IDENTIFIER; }
  ;

expect_label_identifier
  : %empty  { parse_state = EXPECTING_LABEL; }
  ;

condition_lines
  : priority_run_condition_lines
  | run_condition_lines
  | run_always_condition_line
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
  : RUN expect_run_condition_identifier WHEN run_condition_identifiers '\n' { printf("run_condition: %s\n", $1); }
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
  : decleration '\n'      { printf("decleration: %s\n", $1); }
  ;

decleration
  : VAR IDENTIFIER
      { $$ = make_string("var ", $1); }
  | LED IDENTIFIER
      { $$ = make_string("led ", $1); }
  ;

code_lines
  : code_line
  | code_lines code_line
  ;

code_line
  : IDENTIFIER ':' '\n'   { printf("label: %s\n", $1); }
  | command '\n'     { printf("command: %s\n", $1); }
  ;

command
  : GOTO expect_label_identifier LABEL
      { $$ = make_string("goto", $2); }
  | WAIT number_or_identifier
      { $$ = make_string("wait", $2); }
  | FADE
      { $$ = "fade"; }
  | expression
  ;

expression
  : IDENTIFIER assignment_operator number_or_identifier
      { $$ = make_string($2, $3); printf("1:%s 2:%s 3:%s\n", $1, $2, $3); }
  ;

/*
  : VARIABLE_IDENTIFIER assignment_operator number_or_identifier
      { $$ = make_string($2, $3); printf("1:%s 2:%s 3:%s\n", $1, $2, $3); }
  ;
  | led_identifiers assignment_operator number_or_identifier
      { $$ = make_string($2, $3); printf("1:%s 2:%s 3:%s\n", $1, $2, $3); }
  ;

led_identifiers
  : LED_IDENTIFIER
  | led_identifiers ',' LED_IDENTIFIER
  ;
*/

number_or_identifier
  : NUMBER                { $$ = $1; }
  | VARIABLE_IDENTIFIER   { $$ = $1; }
  ;

assignment_operator
  : '='                   { $$ = "="; }
  | MUL_ASSIGN            { $$ = "*="; }
  | DIV_ASSIGN            { $$ = "/="; }
  | ADD_ASSIGN            { $$ = "+="; }
  | SUB_ASSIGN            { $$ = "-="; }
  ;

%%

/* ========================================================================== */
/* Epilogue */


typedef struct {
  const char *value;
  const int token;
} identifiers;

const identifiers run_condition_tokens[] = {
  {.value = "always", .token = RUN_CONDITION_IDENTIFIER_ALWAYS},

  {.value = "light-switch-position-0", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "light-switch-position-1", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "light-switch-position-2", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "light-switch-position-3", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "light-switch-position-4", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "light-switch-position-5", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "light-switch-position-6", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "light-switch-position-7", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "light-switch-position-8", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "neutral", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "forward", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "reversing", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "braking", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "indicator-left", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "indicator-right", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "hazard", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "blink-flag", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "blink-left", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "blink-right", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "winch-disabled", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "winch-idle", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "winch-in", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "winch-out", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "gear-1", .token = RUN_CONDITION_IDENTIFIER},
  {.value = "gear-2", .token = RUN_CONDITION_IDENTIFIER},

  {.value = "no-signal", .token = PRIORITY_RUN_CONDITION_IDENTIFIER},
  {.value = "initializing", .token = PRIORITY_RUN_CONDITION_IDENTIFIER},
  {.value = "servo-output-setup-centre", .token = PRIORITY_RUN_CONDITION_IDENTIFIER},
  {.value = "servo-output-setup-left", .token = PRIORITY_RUN_CONDITION_IDENTIFIER},
  {.value = "servo-output-setup-right", .token = PRIORITY_RUN_CONDITION_IDENTIFIER},
  {.value = "reversing-setup-steering", .token = PRIORITY_RUN_CONDITION_IDENTIFIER},
  {.value = "reversing-setup-throttle", .token = PRIORITY_RUN_CONDITION_IDENTIFIER},
  {.value = "gear-changed", .token = PRIORITY_RUN_CONDITION_IDENTIFIER},

  {.value = NULL, .token = EOF},
};

const identifiers car_state[] = {
  {.value = "light-switch-position-0", .token = CAR_STATE},
  {.value = "light-switch-position-1", .token = CAR_STATE},
  {.value = "light-switch-position-2", .token = CAR_STATE},
  {.value = "light-switch-position-3", .token = CAR_STATE},
  {.value = "light-switch-position-4", .token = CAR_STATE},
  {.value = "light-switch-position-5", .token = CAR_STATE},
  {.value = "light-switch-position-6", .token = CAR_STATE},
  {.value = "light-switch-position-7", .token = CAR_STATE},
  {.value = "light-switch-position-8", .token = CAR_STATE},
  {.value = "neutral", .token = CAR_STATE},
  {.value = "forward", .token = CAR_STATE},
  {.value = "reversing", .token = CAR_STATE},
  {.value = "braking", .token = CAR_STATE},
  {.value = "indicator-left", .token = CAR_STATE},
  {.value = "indicator-right", .token = CAR_STATE},
  {.value = "hazard", .token = CAR_STATE},
  {.value = "blink-flag", .token = CAR_STATE},
  {.value = "blink-left", .token = CAR_STATE},
  {.value = "blink-right", .token = CAR_STATE},
  {.value = "winch-disabled", .token = CAR_STATE},
  {.value = "winch-idle", .token = CAR_STATE},
  {.value = "winch-in", .token = CAR_STATE},
  {.value = "winch-out", .token = CAR_STATE},
  {.value = "gear-1", .token = CAR_STATE},
  {.value = "gear-2", .token = CAR_STATE},

  {.value = NULL, .token = EOF},
};

const identifiers reserved_words[] = {
  {.value = "goto", .token = GOTO},
  {.value = "var", .token = VAR},
  {.value = "led", .token = LED},
  {.value = "wait", .token = WAIT},
  {.value = "skip", .token = SKIP},
  {.value = "if", .token = IF},
  {.value = "all", .token = ALL},
  {.value = "none", .token = NONE},
  {.value = "not", .token = NOT},
  {.value = "fade", .token = FADE},
  {.value = "run", .token = RUN},
  {.value = "when", .token = WHEN},
  {.value = "or", .token = OR},
  {.value = "clicks", .token = CLICKS_IDENTIFIER},
  {.value = "master", .token = MASTER},
  {.value = "slave", .token = SLAVE},
  {.value = "global", .token = GLOBAL},
  {.value = "random", .token = RANDOM},
  {.value = "steering", .token = STEERING},
  {.value = "throttle", .token = THROTTLE},

  {.value = NULL, .token = EOF},
};


int get_predefined_identifier(const identifiers *r, const char *id)
{
  while (r->value != NULL) {
    if (strcasecmp(r->value, id) == 0) {
      return r->token;
    }
    ++r;
  }
  return EOF;
}

int yylex(void)
{
  static size_t length = 40;
  static char *symbuf = NULL;
  int c;
  static int empty_line = 1;

  if (symbuf == NULL){
    symbuf = (char *) malloc(length + 1);
    // FIXME: need to add check for malloc failed...
  }

  switch (parse_state) {
    case UNKNOWN:
      break;

    case EXPECTING_LABEL:
      printf("Expecting label\n");
      break;

    case EXPECTING_RUN_CONDITION_IDENTIFIER:
      printf("Expecting run condition\n");
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

  if (c == ':') {
    parse_state = UNKNOWN;
    return c;
  }

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

    parse_state = UNKNOWN;
    return NUMBER;
  }

  if (isalpha(c)) {
    int count = 0;
    int id;

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

    yylval = symbuf;

    if (parse_state == EXPECTING_RUN_CONDITION_IDENTIFIER) {
      id = get_predefined_identifier(run_condition_tokens, yylval);
      if (id != EOF) {
        return id;
      }
    }

    id = get_predefined_identifier(reserved_words, yylval);
    if (id != EOF) {
      return id;
    }

    if (parse_state == EXPECTING_LABEL) {
      return LABEL;
    }

    return IDENTIFIER;
  }

  if (c == '\n') {
    empty_line = 1;
    parse_state = UNKNOWN;
    return c;
  }

  if (c == EOF) {
    parse_state = UNKNOWN;
    return 0;
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

  return c;
}

/* Called by yyparse on error.  */
void yyerror(char const *s)
{
  fprintf(stderr, "ERROR: %s\n", s);
}


char *make_string(char *s1, char *s2)
{
    static char buf[256];
    snprintf(buf, 256, "%s %s", s1,  s2);
    return buf;
}


int main(int argc, char *argv[])
{
  printf("Bison test parser\n");
  yydebug = 1;
  return yyparse();
}
