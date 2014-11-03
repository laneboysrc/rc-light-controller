/*
Bison grammar file for light programs

Light programs are simple programs that are interpreted by the
LANE Boys RC light controller (TLC5940/LPC812 version)


reserved keywords:
  goto, var, led, leds, sleep, skip, if, is, any, all, none, not, fade, stepsize,
  run, when, or, master, slave, global, random, steering, throttle, gear, abs

  clicks: Pre-defined global variable; increments when 6-clicks on CH3

*/


/* ========================================================================== */
/* Prologue */

%{

#define YYPARSE_PARAM scanner
#define YYLEX_PARAM scanner

#include "symbols.h"
#include "emitter.h"
#include "log.h"

%}


/* ========================================================================== */
/* Bison declarations */

%code {
/* This can not go into the prologue as YYSTYPE and YYLTYPE are only available
 * at this point!
 */
extern int yylex(YYSTYPE * yylval_param, YYLTYPE * yylloc_param);

}

%locations
%token-table
%defines
%define api.pure full
%define parse.lac full
%define parse.error verbose

%union {
    SYMBOL_T *symbol;
    uint32_t instruction;
    int16_t immediate;
}

%token <immediate> NUMBER

%token <symbol> UNDECLARED_SYMBOL
%token <symbol> GLOBAL_VARIABLE
%token <symbol> VARIABLE
%token <symbol> LED_ID
%token <symbol> LABEL

%token <instruction> END

%token <instruction> PRIORITY_RUN_CONDITION
%token <instruction> RUN_CONDITION
%token <instruction> RUN_CONDITION_ALWAYS

%token <instruction> RANDOM
%token <instruction> STEERING
%token <instruction> THROTTLE
%token <instruction> GEAR

%token <instruction> CAR_STATE

%token <instruction> RUN
%token <instruction> WHEN
%token <instruction> OR

%token <instruction> GLOBAL
%token <instruction> VAR
%token <instruction> LED
%token <instruction> LEDS
%token <instruction> MASTER
%token <instruction> SLAVE

%token <instruction> FADE
%token <instruction> STEPSIZE
%token <instruction> GOTO
%token <instruction> SLEEP

%token <instruction> '='
%token <instruction> MUL_ASSIGN
%token <instruction> DIV_ASSIGN
%token <instruction> ADD_ASSIGN
%token <instruction> SUB_ASSIGN
%token <instruction> AND_ASSIGN
%token <instruction> OR_ASSIGN
%token <instruction> XOR_ASSIGN
%token <instruction> ABS

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

%type <immediate> master_or_slave
%type <instruction> expression
%type <instruction> assignment_operator
%type <instruction> led_assignment_parameter leds led_list
%type <instruction> parameter car_state_list test_operator
%type <instruction> run_conditions priority_run_conditions
%type <instruction> run_condition_line priority_run_condition_line
%type <instruction> run_condition_lines priority_run_condition_lines
%type <instruction> run_always_condition_line

%start programs

%%

/* ========================================================================== */
/* Grammar rules */

programs
  : program END '\n'
  | programs program END '\n'
  ;

program
  : condition_lines decleration_lines code_lines
      { emit_end_of_program(); }
  | condition_lines code_lines
      { emit_end_of_program(); }
  ;

expect_run_condition
  : %empty  { parse_state = EXPECTING_RUN_CONDITION; }
  ;

expect_car_state
  : %empty  { parse_state = EXPECTING_CAR_STATE; }
  ;

condition_lines
  : priority_run_condition_lines
        { emit_run_condition($1, 0); }
  | run_condition_lines
        { emit_run_condition(0, $1); }
  | run_always_condition_line
        { emit_run_condition(0, $1); }
  ;

priority_run_condition_lines
  : priority_run_condition_line
  | priority_run_condition_lines priority_run_condition_line
    { $$ = $1 | $2; }
  ;

priority_run_condition_line
  : RUN expect_run_condition WHEN priority_run_conditions '\n'
    { $$ = $4; }
  ;

priority_run_conditions
  : PRIORITY_RUN_CONDITION
  | priority_run_conditions PRIORITY_RUN_CONDITION
    { $$ = $1 | $2; }
  | priority_run_conditions OR PRIORITY_RUN_CONDITION
    { $$ = $1 | $3; }
  ;

run_condition_lines
  : run_condition_line
  | run_condition_lines run_condition_line
    { $$ = $1 | $2; }
  ;

run_condition_line
  : RUN expect_run_condition WHEN run_conditions '\n'
    { $$ = $4; }
  ;

run_conditions
  : RUN_CONDITION
  | run_conditions RUN_CONDITION
      { $$ = $1 | $2; }
  | run_conditions OR RUN_CONDITION
      { $$ = $1 | $3; }
  ;

run_always_condition_line
  : RUN expect_run_condition RUN_CONDITION_ALWAYS '\n'
      { $$ = $3; }
  ;

decleration_lines
  : decleration_line
  | decleration_lines decleration_line
  ;

decleration_line
  : decleration '\n'
  ;

decleration
  : VAR UNDECLARED_SYMBOL
      { add_symbol($2->name, VARIABLE, -1, &@2); }
  | VAR GLOBAL_VARIABLE
      /* Declare the variable as local variable, overshadowing the global one */
      { add_symbol($2->name, VARIABLE, -1, &@2); }
  | VAR error
  | GLOBAL VAR UNDECLARED_SYMBOL
      { add_symbol($3->name, GLOBAL_VARIABLE, -1, &@3); }
  | GLOBAL VAR GLOBAL_VARIABLE
      { /* Nothing to do, global variable already declared */ }
  | GLOBAL VAR error
  | LED UNDECLARED_SYMBOL '=' master_or_slave
      {  add_symbol($2->name, LED_ID, $4, &@2); }
  | LED error
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
  | error
  ;

code_line
  /* New label declaration */
  : UNDECLARED_SYMBOL ':' '\n'
      { add_symbol($1->name, LABEL, pc, &@1); }
  | UNDECLARED_SYMBOL error
  /* Label that was already forward-declared in a GOTO */
  | LABEL ':' '\n'
      { set_symbol($1, LABEL, pc, &@1); }
  | LABEL error
  | command '\n'
  | error '\n'
  ;

command
  : GOTO LABEL
      { emit($1 | (($2->index) & 0xffffff)); }
  | GOTO UNDECLARED_SYMBOL
      { add_symbol($2->name, LABEL, -1, &@2); emit($1); }
  | FADE leds STEPSIZE VARIABLE
      { emit_led_instruction($1 | $4->index, &@1); }
  | FADE leds STEPSIZE GLOBAL_VARIABLE
      { emit_led_instruction($1 | $4->index, &@1); }
  | FADE leds STEPSIZE NUMBER
      { emit_led_instruction($1 | INSTRUCTION_MODIFIER_IMMEDIATE | ($4 & 0xff), &@1); }
  | FADE leds STEPSIZE NUMBER '%'
      { emit_led_instruction($1 | INSTRUCTION_MODIFIER_IMMEDIATE | ($4 & 0xff), &@1); }
  | SLEEP parameter
      { emit($1 | $2); }
  | SKIP IF test_expression
  | expression
  ;

test_expression
  : VARIABLE test_operator parameter
      { emit($2 | ($1->index << 16) | $3); }
  | LED_ID test_operator parameter
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

car_state_list
  : CAR_STATE
  | car_state_list CAR_STATE
      { $$ = $1 | $2; }
  ;

expression
  : VARIABLE assignment_operator parameter
      { emit($2 | ($1->index << 16) | $3); }
  | GLOBAL_VARIABLE assignment_operator parameter
      { emit($2 | ($1->index << 16) | $3); }
  | leds '=' led_assignment_parameter
      { emit_led_instruction(0x02000000 | $3, &@1); }
  ;

leds
  : ALL LEDS
    { add_led_to_list(-1); }
  | led_list
  ;

led_list
  : LED_ID
      { add_led_to_list($1->index); }
  | leds ',' LED_ID
      { add_led_to_list($3->index); }
  ;

led_assignment_parameter
  : NUMBER
      /* All opcodes that work with immediates have the lowest bit set */
      { $$ = INSTRUCTION_MODIFIER_IMMEDIATE | ($1 & 0xff); }
  | NUMBER '%'
      /* All opcodes that work with immediates have the lowest bit set */
      { $$ = INSTRUCTION_MODIFIER_IMMEDIATE | ($1 & 0xff); }
  | VARIABLE
      { $$ = $1->index; }
  | GLOBAL_VARIABLE
      { $$ = $1->index; }
  ;

parameter
  : NUMBER
      /* All opcodes that work with immediates have the lowest bit set */
      { $$ = INSTRUCTION_MODIFIER_IMMEDIATE | ($1 & 0xffff); }
  | VARIABLE
      { $$ = (PARAMETER_TYPE_VARIABLE << 8) | $1->index; }
  | GLOBAL_VARIABLE
      { $$ = (PARAMETER_TYPE_VARIABLE << 8) | $1->index; }
  | LED_ID
      { $$ = (PARAMETER_TYPE_LED << 8) | $1->index; }
  | STEERING
      { $$ = (PARAMETER_TYPE_STEERING << 8); }
  | THROTTLE
      { $$ = (PARAMETER_TYPE_THROTTLE << 8); }
  | GEAR
      { $$ = (PARAMETER_TYPE_GEAR << 8); }
  | RANDOM
      { $$ = (PARAMETER_TYPE_RANDOM << 8); }
  ;

assignment_operator
  : '='
  | ADD_ASSIGN
  | SUB_ASSIGN
  | MUL_ASSIGN
  | DIV_ASSIGN
  | AND_ASSIGN
  | OR_ASSIGN
  | XOR_ASSIGN
  | ABS '='
  ;

%%


/* ========================================================================== */
/* Epilogue */

const char *token2str(int token)
{
    return yytname[YYTRANSLATE(token)];
}
