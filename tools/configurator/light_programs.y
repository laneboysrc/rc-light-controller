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

  - SKIP IF IS
  - SKIP IF ANY {car-state} [{car-state} ...]
  - SKIP IF ALL {car-state} [{car-state} ...]
  - SKIP IF NONE {car-state} [{car-state} ...]
  - SKIP IF NOT {car-state}

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
  goto, var, led, wait, skip, if, any, all, none, not, fade, run, when, or,
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
%token <i> VARIABLE
%token <i> LED_ID
%token <i> LABEL
%token <instruction> RANDOM
%token <instruction> STEERING
%token <instruction> THROTTLE
%token <instruction> CAR_STATE
%token <instruction> PRIORITY_RUN_CONDITION
%token <instruction> RUN_CONDITION
%token <instruction> RUN_CONDITION_ALWAYS

%token <instruction> SKIP
%token <instruction> IF
%token <instruction> ANY
%token <instruction> ALL
%token <instruction> NONE
%token <instruction> IS
%token <instruction> NOT

%token <instruction> EQ
%token <instruction> NE
%token <instruction> GT
%token <instruction> LT
%token <instruction> GE
%token <instruction> LE


%token <instruction> RUN
%token <instruction> WHEN
%token <instruction> OR

%token <instruction> ASSIGN
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
%type <instruction> test_parameter car_state_list run_conditions test_operator
%%

/* ========================================================================== */
/* Grammar rules */

program
  : condition_lines decleration_lines code_lines
      { emit(INSTRUCTION_END_OF_PROGRAM); }
  | condition_lines code_lines
      { emit(INSTRUCTION_END_OF_PROGRAM); }
  | %empty
  ;

expect_run_condition
  : %empty  { parse_state = EXPECTING_RUN_CONDITION; }
  ;

expect_car_state
  : %empty  { parse_state = EXPECTING_CAR_STATE; }
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
      { $$ = $1 | $2; }
  | run_conditions OR RUN_CONDITION
      { $$ = $1 | $2; }
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
  | LED IDENTIFIER ASSIGN master_or_slave
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
      { emit($1 | $2->index); }
  | FADE leds variable_or_number
      /* FIXME: deal with list of LEDs */
      { emit($1 | ($2 << 16) | ($2 << 8) | $3); }
  | WAIT variable_or_number
      { emit($1 | $2); }
  | SKIP IF test_expression
  | expression
  ;

test_expression
  : VARIABLE test_operator test_parameter
      { emit($2 | ($1->index << 16) | $3); }
  | LED_ID test_operator test_parameter
      /* All LED relates tests have 0x02 set in the opcode */
      { emit($2 | INSTRUCTION_MODIFIER_LED | ($1->index << 16) | $3); }
  | ANY expect_car_state car_state_list
      { emit($1 | $3); }
  | ALL expect_car_state car_state_list
      { emit($1 | $3); }
  | NONE expect_car_state car_state_list
      { emit($1 | $3); }
  | IS expect_car_state CAR_STATE
      { emit($1 | $3); }
  | NOT expect_car_state CAR_STATE
      { emit($1 | $3); }
  ;
  
test_operator
  : EQ
  | NE
  | GT
  | LT
  | GE
  | LE
  ;
  
test_parameter
  : variable_or_number
  | LED_ID
      { $$ = (PARAMETER_TYPE_LED << 8) | $1->index; }
  | STEERING
      { $$ = (PARAMETER_TYPE_STEERING << 8); }
  | THROTTLE
      { $$ = (PARAMETER_TYPE_THROTTLE << 8); }
  ;
   
car_state_list 
  : CAR_STATE
  | car_state_list CAR_STATE
      { $$ = $1 | $2; }
  ;
  
expression
  : VARIABLE assignment_operator variable_assignment_parameter
      { emit($2 | ($1->index << 16) | $3); }
  | VARIABLE assignment_operator ABS abs_assignment_parameter
      { emit($3 | $4); }
  | leds ASSIGN led_assignment_parameter
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
      { $$ = INSTRUCTION_MODIFIER_IMMEDIATE | ($1 & 0xff); }
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
      { $$ = INSTRUCTION_MODIFIER_IMMEDIATE | ($1 & 0xffff); }
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
  : ASSIGN
  | ADD_ASSIGN
  | SUB_ASSIGN
  | MUL_ASSIGN
  | DIV_ASSIGN
  | AND_ASSIGN
  | OR_ASSIGN
  | XOR_ASSIGN
  ;

%%

/* ========================================================================== */
/* Epilogue */

const char *token2str(int token)
{
    return yytname[YYTRANSLATE(token)];
}
