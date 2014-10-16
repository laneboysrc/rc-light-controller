/* Bison test */

/*

programs:
  programs | program;

program:
  @empty | run-conditions declerations code;

run-conditions:
  run-conditions | run-condition;

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
    - IDEA: store used locations in a list to be able to cross-reference

  - led [led ...] = value
  - led [led ...] = variable
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
  goto, var, led, wait, skip, if, all, none, not, fade
  FIXME: add variable identifier for sequencer!
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
%}


/* ========================================================================== */
/* Bison declarations */
%define api.value.type {char *}

%token LED
%token VAR

%token FADE
%token GOTO
%token WAIT

%token LABEL
%token NUMBER
%token IDENTIFIER

%token RUN_WHEN_LIGHT_SWITCH_POSITION_0
%token RUN_WHEN_LIGHT_SWITCH_POSITION_1
%token RUN_WHEN_LIGHT_SWITCH_POSITION_2
%token RUN_WHEN_LIGHT_SWITCH_POSITION_3
%token RUN_WHEN_LIGHT_SWITCH_POSITION_4
%token RUN_WHEN_LIGHT_SWITCH_POSITION_5
%token RUN_WHEN_LIGHT_SWITCH_POSITION_6
%token RUN_WHEN_LIGHT_SWITCH_POSITION_7
%token RUN_WHEN_LIGHT_SWITCH_POSITION_8
%token RUN_WHEN_NEUTRAL
%token RUN_WHEN_FORWARD
%token RUN_WHEN_REVERSING
%token RUN_WHEN_BRAKING
%token RUN_WHEN_INDICATOR_LEFT
%token RUN_WHEN_INDICATOR_RIGHT
%token RUN_WHEN_HAZARD
%token RUN_WHEN_BLINK_FLAG
%token RUN_WHEN_BLINK_LEFT
%token RUN_WHEN_BLINK_RIGHT
%token RUN_WHEN_WINCH_DISABLERD
%token RUN_WHEN_WINCH_IDLE
%token RUN_WHEN_WINCH_IN
%token RUN_WHEN_WINCH_OUT
%token RUN_WHEN_GEAR_1
%token RUN_WHEN_GEAR_2
%token RUN_ALWAYS

%token RUN_WHEN_NO_SIGNAL
%token RUN_WHEN_INITIALIZING
%token RUN_WHEN_SERVO_OUTPUT_SETUP_CENTRE
%token RUN_WHEN_SERVO_OUTPUT_SETUP_LEFT
%token RUN_WHEN_SERVO_OUTPUT_SETUP_RIGHT
%token RUN_WHEN_REVERSING_SETUP_STEERING
%token RUN_WHEN_REVERSING_SETUP_THROTTLE
%token RUN_WHEN_GEAR_CHANGED

%start program

%%

/* ========================================================================== */
/* Grammar rules */

program
  : condition_lines decleration_lines code_lines
  | condition_lines code_lines
  ;

condition_lines
  : condition_line
  | condition_lines condition_line
  ;

condition_line
  : RUN_ALWAYS '\n'
  ;

decleration_lines
  : decleration_line
  | decleration_lines decleration_line
  ;

decleration_line
  : VAR '\n'
  ;

code_lines
  : code_line
  | code_lines code_line
  ;

code_line
  : FADE '\n'
  ;


/*
conditions
  : priority_run_conditions
  | run_conditions
  ;

priority_run_conditions
  : RUN_WHEN_NO_SIGNAL
  | RUN_WHEN_INITIALIZING
  ;

run_conditions
  : RUN_ALWAYS
  | RUN_WHEN_NEUTRAL
  ;

declerations
  : VAR
  | LED
  ;

code
  : code line
  ;

line
  :'\n'
  | command '\n'  { printf("command: %s\n", $1); }
  ;

command
  : IDENTIFIER ':'    { $$ = make_string("label", $1); }
  | GOTO IDENTIFIER   { $$ = make_string("goto", $2); }
  | WAIT NUMBER       { $$ = make_string("wait number=", $2); }
  | WAIT IDENTIFIER   { $$ = make_string("wait var=", $2); }
  | FADE              { $$ = "fade"; }
  ;
priority_run_conditions
  : priority_run_conditions priority_run_condition_line
  ;

priority_run_condition_line
  : '\n'
  | priority_run_condition '\n'  { printf("priority_run_condition: %s\n", $1); }
  | error '\n'
  ;

priority_run_condition
  : RUN_WHEN_NO_SIGNAL
  | RUN_WHEN_INITIALIZING
  | RUN_WHEN_SERVO_OUTPUT_SETUP_CENTRE
  | RUN_WHEN_SERVO_OUTPUT_SETUP_LEFT
  | RUN_WHEN_SERVO_OUTPUT_SETUP_RIGHT
  | RUN_WHEN_REVERSING_SETUP_STEERING
  | RUN_WHEN_REVERSING_SETUP_THROTTLE
  | RUN_WHEN_GEAR_CHANGED
  ;

run_conditions:
  %empty
| run_conditions run_condition_line
;

run_condition_line:
  '\n'
| run_condition '\n'  { printf("run_condition: %s\n", $1); }
| error '\n'
;

run_condition:
  RUN_WHEN_LIGHT_SWITCH_POSITION_0
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
;


declerations
  : declerations decleration_line
  ;

decleration_line:
  '\n'
| decleration '\n'  { printf("decleration: %s\n", $1); }
| error '\n'
;

decleration:
  VAR IDENTIFIER    { $$ = make_string("var", $2); }
| LED IDENTIFIER    { $$ = make_string("led", $2); }
;

code
  : code line
  ;

line:
  '\n'
| command '\n'  { printf("command: %s\n", $1); }
| error '\n'
;

command:
  IDENTIFIER ':'    { $$ = make_string("label", $1); }
| GOTO IDENTIFIER   { $$ = make_string("goto", $2); }
| WAIT NUMBER       { $$ = make_string("wait number=", $2); }
| WAIT IDENTIFIER   { $$ = make_string("wait var=", $2); }
| FADE              { $$ = "fade"; }
;

*/



%%

/* ========================================================================== */
/* Epilogue */

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

    return NUMBER;
  }

  if (isalpha(c)) {
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
    } while (isalnum(c) || c == '-');

    ungetc(c, stdin);
    symbuf[count] = '\0';

    yylval = symbuf;
    if (strcmp(symbuf, "goto") == 0) {
      return GOTO;
    }
    if (strcmp(symbuf, "wait") == 0) {
      return WAIT;
    }
    if (strcmp(symbuf, "fade") == 0) {
      return FADE;
    }
    if (strcmp(symbuf, "var") == 0) {
      return VAR;
    }
    if (strcmp(symbuf, "led") == 0) {
      return LED;
    }
    if (strcmp(symbuf, "run-always") == 0) {
      return RUN_ALWAYS;
    }
    if (strcmp(symbuf, "no-signal") == 0) {
      return RUN_WHEN_NO_SIGNAL;
    }
    return IDENTIFIER;
  }

  if (c == '\n') {
    empty_line = 1;
    return c;
  }

  if (c == EOF) {
    return 0;
  }
}

/* Called by yyparse on error.  */
void yyerror(char const *s)
{
  fprintf(stderr, "ERROR: %s\n", s);
}


char *make_string(char *s1, char *s2)
{
    static char buf[256];
    snprintf(buf, 256, "%s (%s)", s1,  s2);
    return buf;
}


int main(int argc, char *argv[])
{
  printf("Bison test parser\n");
  return yyparse();
}
