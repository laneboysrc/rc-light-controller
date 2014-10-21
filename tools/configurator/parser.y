/*
Bison grammar file for light programs

Light programs are simple programs that are interpreted by the
LANE Boys RC light controller (TLC5940/LPC812 version)


// FIXME: add support for multiple programs
// FIXME: can we deal with empty lines (by using a different parser type?)


reserved keywords:
  goto, var, led, wait, skip, if, is, any, all, none, not, fade, run, when, or,
  master, slave, global, random, steering, throttle, abs

  clicks: Pre-defined global variable; increments when 6-clicks on CH3

*/


/* ========================================================================== */
/* Prologue */

%{

#define YYPARSE_PARAM scanner
#define YYLEX_PARAM scanner

#include "symbols.h"
#include "emitter.h"

void yyerror(const char *s);

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
%pure-parser
%defines

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

%token <instruction> PRIORITY_RUN_CONDITION
%token <instruction> RUN_CONDITION
%token <instruction> RUN_CONDITION_ALWAYS

%token <instruction> RANDOM
%token <instruction> STEERING
%token <instruction> THROTTLE

%token <instruction> CAR_STATE

%token <instruction> RUN
%token <instruction> WHEN
%token <instruction> OR

%token <instruction> GLOBAL
%token <instruction> VAR
%token <instruction> LED
%token <instruction> MASTER
%token <instruction> SLAVE

%token <instruction> FADE
%token <instruction> GOTO
%token <instruction> WAIT

%token <instruction> ASSIGN
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
%type <instruction> abs_assignment_parameter variable_assignment_parameter
%type <instruction> led_assignment_parameter leds variable_or_number
%type <instruction> test_parameter car_state_list test_operator
%type <instruction> run_conditions priority_run_conditions
%type <instruction> run_condition_line priority_run_condition_line
%type <instruction> run_condition_lines priority_run_condition_lines
%type <instruction> run_always_condition_line

%start program

%%

/* ========================================================================== */
/* Grammar rules */

program
  : condition_lines decleration_lines code_lines
      { emit_end_of_program(); }
  | condition_lines code_lines
      { emit_end_of_program(); }
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
      { $$ = $1 | $2; }
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
      { add_symbol($2->name, VARIABLE, -1); }
  | VAR GLOBAL_VARIABLE
      /* Declare the variable as local variable, overshadowing the global one */
      { add_symbol($2->name, VARIABLE, -1); }
  | VAR error
      { fprintf(stderr, "'var' not followed by an identifier\n"); }
  | GLOBAL VAR UNDECLARED_SYMBOL
      { add_symbol($3->name, GLOBAL_VARIABLE, -1); }
  | GLOBAL VAR GLOBAL_VARIABLE
      { /* Nothing to do, global variable already declared */ }
  | GLOBAL VAR error
      { fprintf(stderr, "'global var' not followed by an identifier\n"); }
  | LED UNDECLARED_SYMBOL ASSIGN master_or_slave
      {  add_symbol($2->name, LED_ID, $4); }
  | LED error
      { fprintf(stderr, "'led' not followed by = <master|slave>[0..15]\n"); }
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
      { fprintf(stderr, "Unsupported operation\n"); }
  ;

code_line
  /* New label declaration */
  : UNDECLARED_SYMBOL ':' '\n'
      { add_symbol($1->name, LABEL, pc); }
  /* Label that was already forward-declared in a GOTO */
  | LABEL ':' '\n'
      { set_symbol($1, LABEL, pc); }
  | LABEL error
      { fprintf(stderr, "Label used in unsupported operation\n"); }
  | UNDECLARED_SYMBOL error
      { fprintf(stderr, "Undeclared identifier\n"); }
  | command '\n'
  | error '\n'
  ;

command
  : GOTO LABEL
      { emit($1 | ($2->index) & 0xffffff); }
  | GOTO UNDECLARED_SYMBOL
      { add_symbol($2->name, LABEL, -1); emit($1); }
  | GOTO error
      { fprintf(stderr, "%d:%d 'goto' not followed by label\n",
        @2.first_line, @2.first_column); }
  | FADE leds variable_or_number
      { emit_led_instruction($1 | $3); }
  | FADE leds error
      { fprintf(stderr, "'fade' value is not variable or number\n"); }
  | FADE error
      { fprintf(stderr, "'fade' is not followed by list of LED identifiers\n"); }
  | WAIT variable_or_number
      { emit($1 | $2); }
  | WAIT error
      { fprintf(stderr, "'wait' value is not variable or number\n"); }
  | SKIP IF test_expression
  | SKIP IF error
      { fprintf(stderr, "Illegal 'skip if' statement\n"); }
  | expression
  ;

test_expression
  : VARIABLE test_operator test_parameter
      { emit($2 | ($1->index << 16) | $3); }
  | VARIABLE test_operator error
      { fprintf(stderr, "test parameter must be a number, variable, LED, 'steering' or 'throttle'.\n"); }
  | VARIABLE error
      { fprintf(stderr, "unknown test operator\n"); }
  | LED_ID test_operator test_parameter
      /* All LED relates tests have 0x02 set in the opcode */
      { emit($2 | INSTRUCTION_MODIFIER_LED | ($1->index << 16) | $3); }
  | ANY expect_car_state car_state_list
      { emit($1 | $3); }
  | ANY expect_car_state car_state_list error
      { fprintf(stderr, "One or more item is not a car state\n"); }
  | ANY expect_car_state error
      { fprintf(stderr, "One or more item is not a car state\n"); }
  | ALL expect_car_state car_state_list
      { emit($1 | $3); }
  | ALL expect_car_state car_state_list error
      { fprintf(stderr, "One or more item is not a car state\n"); }
  | ALL expect_car_state error
      { fprintf(stderr, "One or more item is not a car state\n"); }
  | NONE expect_car_state car_state_list
      { emit($1 | $3); }
  | NONE expect_car_state car_state_list error
      { fprintf(stderr, "One or more item is not a car state\n"); }
  | NONE expect_car_state error
      { fprintf(stderr, "One or more item is not a car state\n"); }
  | IS expect_car_state CAR_STATE
      { emit($1 | $3); }
  | IS expect_car_state CAR_STATE error
      { fprintf(stderr, "'is' must be followed by a single car-state\n"); }
  | IS expect_car_state error
      { fprintf(stderr, "'is' must be followed by a car-state\n"); }
  | NOT expect_car_state CAR_STATE
      { emit($1 | $3); }
  | NOT expect_car_state CAR_STATE error
      { fprintf(stderr, "'not' must be followed by a single car-state\n"); }
  | NOT expect_car_state error
      { fprintf(stderr, "'not' must be followed by a car-state\n"); }
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
  | VARIABLE assignment_operator error
      { fprintf(stderr, "Unsupported operand\n"); }
  | GLOBAL_VARIABLE assignment_operator variable_assignment_parameter
      { emit($2 | ($1->index << 16) | $3); }
  | GLOBAL_VARIABLE assignment_operator error
      { fprintf(stderr, "Unsupported operand\n"); }
  | VARIABLE assignment_operator ABS abs_assignment_parameter
      { emit($3 | $4); }
  | GLOBAL_VARIABLE assignment_operator ABS abs_assignment_parameter
      { emit($3 | $4); }
  | leds ASSIGN led_assignment_parameter
      { emit_led_instruction(0x02000000 | $3); }
  | leds error
      { fprintf(stderr, "Unsupported operation for LEDs\n"); }
  ;

leds
  : LED_ID
      { add_led_to_list($1->index); }
  | leds ',' LED_ID
      { add_led_to_list($3->index); }
  ;

led_assignment_parameter
  : NUMBER
      /* All opcodes that work with immediates have the lowest bit set */
      { $$ = INSTRUCTION_MODIFIER_IMMEDIATE | ($1 & 0xff); }
  | VARIABLE
      { $$ = $1->index; }
  | GLOBAL_VARIABLE
      { $$ = $1->index; }
  | UNDECLARED_SYMBOL error
      { fprintf(stderr, "Undeclared identifier\n"); }
  | LABEL error
      { fprintf(stderr, "%d:%d Label can not be assigned to an LED\n",
          @2.first_line, @2.first_column); }
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
  | GLOBAL_VARIABLE
      { $$ = (PARAMETER_TYPE_VARIABLE << 8) | $1->index; }
  ;

abs_assignment_parameter
  : VARIABLE
      { $$ = (PARAMETER_TYPE_VARIABLE << 8) | $1->index; }
  | GLOBAL_VARIABLE
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
