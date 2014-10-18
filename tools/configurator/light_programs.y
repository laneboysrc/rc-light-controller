/* 
Bison grammar file for light programs

Light programs are simple programs that are interpreted by the 
LANE Boys RC light controller (TLC5940/LPC812 version) 

*/

/*

NUMBER
  decimal | hexadecimal

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
  - VARIABLE = {NUMBER, VARIABLE, LED[x], random-value, TH, ST}
  - VARIABLE += {NUMBER, VARIABLE, LED[x], TH, ST}
  - VARIABLE -= {NUMBER, VARIABLE, LED[x], TH, ST}
  - VARIABLE *= {NUMBER, VARIABLE, LED[x], TH, ST}
  - VARIABLE /= {NUMBER, VARIABLE, LED[x], TH, ST}

  - FADE led [, led ...] time
  - FADE led [, led ...] variable
    - These translate into:
      - FADE start_led stop_led time
      - FADE start_led stop_led VARIABLE

  - WAIT time
  - WAIT VARIABLE

  - SKIP IF {VARIABLE, LED[x]} == {NUMBER, VARIABLE, LED[x]}
  - SKIP IF {VARIABLE, LED[x]} != {NUMBER, VARIABLE, LED[x]}
  - SKIP IF {VARIABLE, LED[x]} > {NUMBER, VARIABLE, LED[x]}
  - SKIP IF {VARIABLE, LED[x]} >= {NUMBER, VARIABLE, LED[x]}
  - SKIP IF {VARIABLE, LED[x]} < {NUMBER, VARIABLE, LED[x]}
  - SKIP IF {VARIABLE, LED[x]} <= {NUMBER, VARIABLE, LED[x]}
    - These translate into:
      - SKIP IF EQUAL {VARIABLE, LED[x]} {NUMBER, VARIABLE, LED[x]}
      - SKIP IF NOT EQUAL {VARIABLE, LED[x]} {NUMBER, VARIABLE, LED[x]}
      - SKIP IF GREATER OR EQUAL {VARIABLE, LED[x]} {NUMBER, VARIABLE, LED[x]}
      - SKIP IF GREATER {VARIABLE, LED[x]} {NUMBER, VARIABLE, LED[x]}
      - SKIP IF SMALLER OR EQUAL  {VARIABLE, LED[x]} {NUMBER, VARIABLE, LED[x]}
      - SKIP IF SMALLER  {VARIABLE, LED[x]} {NUMBER, VARIABLE, LED[x]}

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

#include "parser.h"

%}


/* ========================================================================== */
/* Bison declarations */
%union {
    identifier *i;
    uint32_t instruction;
    int16_t immediate;
}

%locations
%token-table
%defines

%token <instruction> LED
%token <instruction> VAR
%token <instruction> GLOBAL
%token <instruction> MASTER
%token <instruction> SLAVE

%token <instruction> FADE
%token <instruction> GOTO
%token <instruction> WAIT

%token <immediate> NUMBER
%token <i> IDENTIFIER
%token <i> LABEL
%token <instruction> RANDOM
%token <instruction> STEERING
%token <instruction> THROTTLE
%token <i> LED_ID
%token <i> VARIABLE
%token <i> CAR_STATE
%token <i> PRIORITY_RUN_CONDITION
%token <i> RUN_CONDITION
%token <i> RUN_CONDITION_ALWAYS

%token <instruction> SKIP
%token <instruction> IF
%token <instruction> ALL
%token <instruction> NONE
%token <instruction> NOT

%token <instruction> RUN
%token <instruction> WHEN
%token <instruction> OR

%token <instruction> MUL_ASSIGN
%token <instruction> DIV_ASSIGN
%token <instruction> ADD_ASSIGN
%token <instruction> SUB_ASSIGN
%token <instruction> AND_ASSIGN
%token <instruction> OR_ASSIGN
%token <instruction> XOR_ASSIGN
%token <instruction> ABS

%start program

%type <immediate> master_or_slave
%type <instruction> expression assignment_operator abs_assignment_parameter variable_assignment_parameter
%type <instruction> led_assignment_parameter leds variable_or_number
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
  | FADE leds variable_or_number
      /* FIXME: deal with list of LEDs */
      { emit(0x04000000 | ($2 << 16) | ($2 << 8) | $3); }
  | WAIT variable_or_number
      { emit(0x06000000 | $2); }
  | SKIP IF test_expression
  | expression
  ;

test_expression
  : %empty

expression
  : VARIABLE assignment_operator variable_assignment_parameter
      { emit($2 | ($1->index << 16) | $3); }
  | VARIABLE assignment_operator ABS abs_assignment_parameter
      { emit(0x40000000 | $4); }
  | leds '=' led_assignment_parameter
      /* FIXME: deal with list of LEDs */
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
  ;
  
variable_assignment_parameter
  : variable_or_number
  | LED_ID
      { $$ = (PARAMETER_TYPE_LED << 8) | $1->index; }
  | STEERING
      { $$ = (PARAMETER_TYPE_STEERING << 8); }
  | THROTTLE
      { $$ = (PARAMETER_TYPE_THROTTLE << 8); }
  | RANDOM
      { $$ = (PARAMETER_TYPE_RANDOM << 8); }
  ;

variable_or_number
  : NUMBER
      /* All opcodes that work with immediates have the lowest bit set */
      { $$ = 0x01000000 | ($1 & 0xffff); }
  | VARIABLE
      { $$ = (PARAMETER_TYPE_VARIABLE << 8) | $1->index; }
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

const char *token2str(int token)
{
    return yytname[YYTRANSLATE(token)];
}
