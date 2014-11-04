/*

Jison grammar file for light programs

Light programs are simple programs that are interpreted by the
LANE Boys RC light controller (TLC5940/LPC812 version)


reserved keywords:
  goto, var, led, leds, sleep, skip, if, is, any, all, none, not, fade, stepsize,
  run, when, or, master, slave, global, random, steering, throttle, gear, abs

  clicks: Pre-defined global variable; increments when 6-clicks on CH3

*/



%lex


%%
"0"[xX][\da-fA-F]+ {
  line_is_empty = false;
  console.log("Hex-number: ", yytext);
  yytext = parseInt(yytext, 16);
  return "NUMBER";
}

("-"?)[\d]+ {
  line_is_empty = false;
  console.log("Number: ", yytext);
  yytext = parseInt(yytext, 10);
  return "NUMBER";
}

"run" {
  line_is_empty = false;
  parse_state = "EXPECTING_RUN_CONDITION";
  console.log("Reserved word: " + yytext);
  return yytext.toUpperCase();
}

"is"|"any"|"all"|"none"|"not" {
  line_is_empty = false;
  parse_state = "EXPECTING_CAR_STATE";
  console.log("Reserved word: " + yytext);
  return yytext.toUpperCase();
}

"goto"|"var"|"led"|"leds"|"sleep"|"skip"|"if"|"fade"|"stepsize"|"when"|"or"|"master"|"slave"|"global"|"random"|"steering"|"throttle"|"abs"|"end" {
  line_is_empty = false;
  console.log("Reserved word: " + yytext);
  return yytext.toUpperCase();
}

[a-zA-Z][a-zA-Z0-9_\-]* {
  line_is_empty = false;
  var symbol = get_symbol(yytext, parse_state);
  console.log("Identifier: " + yytext + " (" + symbol.token + "=0x" + symbol.opcode.toString(16) + ") parse_state=" + parse_state);
  return symbol.token;
}

"="|"+="|"-="|"*="|"/="|"&="|"|="|"^=" {
  line_is_empty = false;
  var symbol = get_reserved_word(yytext);
  console.log("Assignment " + yytext + " (" + symbol.token + "=0x" + symbol.opcode.toString(16) + ")");
  return symbol.token;
}

"=="|"!="|">="|"<="|">"|"<" {
  line_is_empty = false;
  var symbol = get_reserved_word(yytext);
  console.log("Comparison " + yytext + " (" + symbol.token + "=0x" + symbol.opcode.toString(16) + ")");
  return symbol.token;
}

"["|"]"|","|":"|"%" {
  line_is_empty = false;
  console.log("'" + yytext + "'");
  return yytext;
}

"//"[^\n]*  /* eat up one-line comments */
";"[^\n]*   /* eat up one-line comments */
[ \t]+      /* eat up whitespace */

\\[ \t]*(("//"|;)[^\n]*)*\n {
    /* eat up continuation characters '\', and the following \n */
    /* Comments can appear in after the '\' character */
    //offset = 1;
    //++yylineno;
  console.log("emtpy line");
}

\n {        /* Only ever give back a single in sequence \n */
  parse_state = "UNKNOWN_PARSE_STATE";
  //offset = 1;
  //++yylineno;

  if (!line_is_empty) {
    line_is_empty = true;
    console.log("linefeed");
    return "LINEFEED";
  }
}

. {
  console.log("Unrecognized character " + yytext);
  return yytext;
}

/* <<EOF>>               return 'EOF'; */

/lex


/* ========================================================================== */
/* Prologue */

%{
"use strict";

var PARAMETER_TYPE_VARIABLE = 0;
var PARAMETER_TYPE_LED = 1;
var PARAMETER_TYPE_RANDOM = 2;
var PARAMETER_TYPE_STEERING = 3;
var PARAMETER_TYPE_THROTTLE = 4;
var PARAMETER_TYPE_GEAR = 5;

var INSTRUCTION_MODIFIER_LED = 0x02000000;
var INSTRUCTION_MODIFIER_IMMEDIATE = 0x01000000;

var line_is_empty = true;
var parse_state = "UNKNOWN_PARSE_STATE";

%}


%%

/* ========================================================================== */
/* Grammar rules */

programs
  : program END LINEFEED
  | programs program END LINEFEED
  ;

program
  : condition_lines decleration_lines code_lines
      { emit_end_of_program(); }
  | condition_lines code_lines
      { emit_end_of_program(); }
  ;

expect_run_condition
  : %empty { console.log("EXPECTING_RUN_CONDITION"); parse_state = "EXPECTING_RUN_CONDITION"; }
  ;

expect_car_state
  : %empty { console.log("EXPECTING_CAR_STATE"); parse_state = "EXPECTING_CAR_STATE"; }
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
  : RUN expect_run_condition WHEN priority_run_conditions LINEFEED
    { $$ = $4; }
  ;

priority_run_conditions
  : PRIORITY_RUN_CONDITION
      { $$ = get_symbol($1, "EXPECTING_RUN_CONDITION").opcode; }
  | priority_run_conditions PRIORITY_RUN_CONDITION
    { $$ = $1 | get_symbol($2, "EXPECTING_RUN_CONDITION").opcode; }
  | priority_run_conditions OR PRIORITY_RUN_CONDITION
    { $$ = $1 | get_symbol($3, "EXPECTING_RUN_CONDITION").opcode; }
  ;

run_condition_lines
  : run_condition_line
  | run_condition_lines run_condition_line
    { $$ = $1 | $2; }
  ;

run_condition_line
  : RUN expect_run_condition WHEN run_conditions LINEFEED
    { $$ = $4; }
  ;

run_conditions
  : RUN_CONDITION
      { $$ = get_symbol($1, "EXPECTING_RUN_CONDITION").opcode; }
  | run_conditions RUN_CONDITION
      { $$ = $1 | get_symbol($2, "EXPECTING_RUN_CONDITION").opcode; }
  | run_conditions OR RUN_CONDITION
      { $$ = $1 | get_symbol($3, "EXPECTING_RUN_CONDITION").opcode; }
  ;

run_always_condition_line
  : RUN RUN_CONDITION_ALWAYS LINEFEED
      { $$ = get_symbol($2, "EXPECTING_RUN_CONDITION").opcode; }
  ;

decleration_lines
  : decleration_line
  | decleration_lines decleration_line
  ;

decleration_line
  : decleration LINEFEED
  ;

decleration
  : VAR UNDECLARED_SYMBOL
      { add_symbol($2, "VARIABLE", -1, @2); }
  | VAR GLOBAL_VARIABLE
      /* Declare the variable as local variable, overshadowing the global one */
      { add_symbol($2, "VARIABLE", -1, @2); }
/*  | VAR error */
  | GLOBAL VAR UNDECLARED_SYMBOL
      { add_symbol($3, "GLOBAL_VARIABLE", -1, @3); }
  | GLOBAL VAR GLOBAL_VARIABLE
      { /* Nothing to do, global variable already declared */ }
/*  | GLOBAL VAR error */
  | LED UNDECLARED_SYMBOL '=' master_or_slave
      {  add_symbol($2, "LED_ID", $4, @2); }
/*  | LED error */
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
/*  | error */
  ;

code_line
  /* New label declaration */
  : UNDECLARED_SYMBOL ':' LINEFEED
      { add_symbol($1, "LABEL", 0/*pc*/, @1); }
/*  | UNDECLARED_SYMBOL error */
  /* Label that was already forward-declared in a GOTO */
  | LABEL ':' LINEFEED
      { set_symbol($1, "LABEL", 0/*pc*/, @1); }
/*  | LABEL error */
  | command LINEFEED
/*  | error LINEFEED */
  ;

command
  : GOTO LABEL
      { emit(get_reserved_word($1).opcode | (($2) & 0xffffff)); }
  | GOTO UNDECLARED_SYMBOL
      { add_symbol($2, "LABEL", -1, @2); emit(get_reserved_word($1).opcode); }
  | FADE leds STEPSIZE VARIABLE
      { emit_led_instruction(get_reserved_word($1).opcode | $4, @1); }
  | FADE leds STEPSIZE GLOBAL_VARIABLE
      { emit_led_instruction(get_reserved_word($1).opcode | $4, @1); }
  | FADE leds STEPSIZE NUMBER
      { emit_led_instruction(get_reserved_word($1).opcode | INSTRUCTION_MODIFIER_IMMEDIATE | ($4 & 0xff), @1); }
  | FADE leds STEPSIZE NUMBER '%'
      { emit_led_instruction(get_reserved_word($1).opcode | INSTRUCTION_MODIFIER_IMMEDIATE | ($4 & 0xff), @1); }
  | SLEEP parameter
      { emit(get_reserved_word($1).opcode | $2); }
  | SKIP IF test_expression
  | expression
  ;

test_expression
  : VARIABLE test_operator parameter
      { emit($2 | ($1 << 16) | $3); }
  | LED_ID test_operator parameter
      /* All LED relates tests have 0x02 set in the opcode */
      { emit($2 | INSTRUCTION_MODIFIER_LED | ($1 << 16) | $3); }
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
      { emit(get_reserved_word($2).opcode | (get_symbol($1).opcode << 16) | $3); }
  | GLOBAL_VARIABLE assignment_operator parameter
      { emit(get_reserved_word($2).opcode | (get_symbol($1).opcode << 16) | $3); }
  | leds '=' led_assignment_parameter
      { emit_led_instruction(0x02000000 | $3, @1); }
  ;

leds
  : ALL LEDS
    { add_led_to_list(-1); }
  | led_list
  ;

led_list
  : LED_ID
      { add_led_to_list($1); }
  | leds ',' LED_ID
      { add_led_to_list($3); }
  ;

led_assignment_parameter
  : NUMBER
      /* All opcodes that work with immediates have the lowest bit set */
      { $$ = INSTRUCTION_MODIFIER_IMMEDIATE | (Number($1) & 0xff); }
  | NUMBER '%'
      /* All opcodes that work with immediates have the lowest bit set */
      { $$ = INSTRUCTION_MODIFIER_IMMEDIATE | (Number($1) & 0xff); }
  | VARIABLE
      { $$ = get_symbol($1).opcode; }
  | GLOBAL_VARIABLE
      { $$ = get_symbol($1).opcode; }
  ;

parameter
  : NUMBER
      /* All opcodes that work with immediates have the lowest bit set */
      { $$ = INSTRUCTION_MODIFIER_IMMEDIATE | (Number($1) & 0xffff); }
  | VARIABLE
      { $$ = (PARAMETER_TYPE_VARIABLE << 8) | get_symbol($1).opcode; }
  | GLOBAL_VARIABLE
      { $$ = (PARAMETER_TYPE_VARIABLE << 8) | get_symbol($1).opcode; }
  | LED_ID
      { $$ = (PARAMETER_TYPE_LED << 8) | get_symbol($1).opcode; }
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
    { $$ = "ABS" }
  ;


%%

