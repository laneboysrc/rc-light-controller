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

%options flex

%%
"0"[xX][\da-fA-F]+ {
  line_is_empty = false;
  yy.logger.log(MODULE, "DEBUG", "Hex-number: ", yytext);
  yytext = parseInt(yytext, 16);
  return "NUMBER";
}

("-"?)[\d]+ {
  line_is_empty = false;
  yy.logger.log(MODULE, "DEBUG", "Number: ", yytext);
  yytext = parseInt(yytext, 10);
  return "NUMBER";
}

"run" {
  line_is_empty = false;
  parse_state = "EXPECTING_RUN_CONDITION";
  yy.logger.log(MODULE, "DEBUG", "Reserved word: " + yytext);
  return yytext.toUpperCase();
}

"is"|"any"|"all"|"none"|"not" {
  line_is_empty = false;
  parse_state = "EXPECTING_CAR_STATE";
  yy.logger.log(MODULE, "DEBUG", "Reserved word: " + yytext);
  return yytext.toUpperCase();
}

"goto"|"var"|"leds"|"led"|"sleep"|"skip"|"if"|"fade"|"stepsize"|"when"|"or"|"master"|"slave"|"global"|"random"|"steering"|"throttle"|"abs"|"end" {
  line_is_empty = false;
  yy.logger.log(MODULE, "DEBUG", "Reserved word: " + yytext);
  return yytext.toUpperCase();
}

[a-zA-Z][a-zA-Z0-9_\-]* {
  line_is_empty = false;
  var symbol = yy.symbols.get_symbol(yytext, parse_state);
  yy.logger.log(MODULE, "DEBUG", "Identifier: " + yytext + " (" + symbol.token + "=0x" + symbol.opcode.toString(16) + ") parse_state=" + parse_state);
  return symbol.token;
}

"="|"+="|"-="|"*="|"/="|"&="|"|="|"^=" {
  line_is_empty = false;
  var symbol = yy.symbols.get_reserved_word(yytext);
  yy.logger.log(MODULE, "DEBUG", "Assignment " + yytext + " (" + symbol.token + "=0x" + symbol.opcode.toString(16) + ")");
  return symbol.token;
}

"=="|"!="|">="|"<="|">"|"<" {
  line_is_empty = false;
  var symbol = yy.symbols.get_reserved_word(yytext);
  yy.logger.log(MODULE, "DEBUG", "Comparison " + yytext + " (" + symbol.token + "=0x" + symbol.opcode.toString(16) + ")");
  return symbol.token;
}

"["|"]"|","|":"|"%" {
  line_is_empty = false;
  yy.logger.log(MODULE, "DEBUG", "'" + yytext + "'");
  return yytext;
}

"//"[^\n]*  /* eat up one-line comments */
";"[^\n]*   /* eat up one-line comments */
[ \t]+      /* eat up whitespace */

\\[ \t]*(("//"|;)[^\n]*)*\n {
  /* eat up continuation characters '\', and the following \n */
  /* Comments can appear in after the '\' character */
  //offset = 1;
  ++yylineno;
  yy.logger.log(MODULE, "DEBUG", "emtpy line");
}

\n %{        /* Only ever give back a single in sequence \n */
  parse_state = "UNKNOWN_PARSE_STATE";
  //offset = 1;
  ++yylineno;

  if (!line_is_empty) {
    line_is_empty = true;
    yy.logger.log(MODULE, "DEBUG", "linefeed");
    return "LINEFEED";
  }
%}

. {
  yy.logger.log(MODULE, "DEBUG", "Unrecognized character " + yytext);
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

var MODULE = "LEX";

%}


%%

/* ========================================================================== */
/* Grammar rules */

parse_entity
  : programs
    { return yy.emitter.output_programs(); }
  ;

programs
  : program END LINEFEED
  | programs program END LINEFEED
  ;

program
  : condition_lines decleration_lines code_lines
      { yy.emitter.emit_end_of_program(); }
  | condition_lines code_lines
      { yy.emitter.emit_end_of_program(); }
  ;

condition_lines
  : priority_run_condition_lines
        { yy.emitter.emit_run_condition($1, 0); }
  | run_condition_lines
        { yy.emitter.emit_run_condition(0, $1); }
  | run_always_condition_line
        { yy.emitter.emit_run_condition(0, $1); }
  ;

priority_run_condition_lines
  : priority_run_condition_line
  | priority_run_condition_lines priority_run_condition_line
    { $$ = $1 | $2; }
  ;

priority_run_condition_line
  : RUN WHEN priority_run_conditions LINEFEED
    { $$ = $3; }
  ;

priority_run_conditions
  : PRIORITY_RUN_CONDITION
      { $$ = yy.symbols.get_symbol($1, "EXPECTING_RUN_CONDITION").opcode; }
  | priority_run_conditions PRIORITY_RUN_CONDITION
    { $$ = $1 | yy.symbols.get_symbol($2, "EXPECTING_RUN_CONDITION").opcode; }
  | priority_run_conditions OR PRIORITY_RUN_CONDITION
    { $$ = $1 | yy.symbols.get_symbol($3, "EXPECTING_RUN_CONDITION").opcode; }
  ;

run_condition_lines
  : run_condition_line
  | run_condition_lines run_condition_line
    { $$ = $1 | $2; }
  ;

run_condition_line
  : RUN WHEN run_conditions LINEFEED
    { $$ = $3; }
  ;

run_conditions
  : RUN_CONDITION
      { $$ = yy.symbols.get_symbol($1, "EXPECTING_RUN_CONDITION").opcode; }
  | run_conditions RUN_CONDITION
      { $$ = $1 | yy.symbols.get_symbol($2, "EXPECTING_RUN_CONDITION").opcode; }
  | run_conditions OR RUN_CONDITION
      { $$ = $1 | yy.symbols.get_symbol($3, "EXPECTING_RUN_CONDITION").opcode; }
  ;

run_always_condition_line
  : RUN RUN_CONDITION_ALWAYS LINEFEED
      { $$ = yy.symbols.get_symbol($2, "EXPECTING_RUN_CONDITION").opcode; }
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
      { yy.symbols.add_symbol($2, "VARIABLE", -1, @2); }
  | VAR GLOBAL_VARIABLE
      /* Declare the variable as local variable, overshadowing the global one */
      { yy.symbols.add_symbol($2, "VARIABLE", -1, @2); }
/*  | VAR error */
  | GLOBAL VAR UNDECLARED_SYMBOL
      { yy.symbols.add_symbol($3, "GLOBAL_VARIABLE", -1, @3); }
  | GLOBAL VAR GLOBAL_VARIABLE
      { /* Nothing to do, global variable already declared */ }
/*  | GLOBAL VAR error */
  | LED UNDECLARED_SYMBOL '=' master_or_slave
      {  yy.symbols.add_symbol($2, "LED_ID", $4, @2); }
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
      { yy.symbols.add_symbol($1, "LABEL", yy.emitter.pc(), @1); }
/*  | UNDECLARED_SYMBOL error */
  /* Label that was already forward-declared in a GOTO */
  | LABEL ':' LINEFEED
      { yy.symbols.set_symbol($1, "LABEL", yy.emitter.pc(), @1); }
/*  | LABEL error */
  | command LINEFEED
/*  | error LINEFEED */
  ;

command
  : GOTO LABEL
      { yy.emitter.emit(yy.symbols.get_reserved_word($1).opcode | (($2) & 0xffffff)); }
  | GOTO UNDECLARED_SYMBOL
      { yy.symbols.add_symbol($2, "LABEL", -1, @2);
        yy.emitter.emit(yy.symbols.get_reserved_word($1).opcode); }
  | FADE leds STEPSIZE VARIABLE
      { yy.emitter.emit_led_instruction(yy.symbols.get_reserved_word($1).opcode | $4, @1); }
  | FADE leds STEPSIZE GLOBAL_VARIABLE
      { yy.emitter.emit_led_instruction(yy.symbols.get_reserved_word($1).opcode | $4, @1); }
  | FADE leds STEPSIZE NUMBER
      { yy.emitter.emit_led_instruction(yy.symbols.get_reserved_word($1).opcode | INSTRUCTION_MODIFIER_IMMEDIATE | ($4 & 0xff), @1); }
  | FADE leds STEPSIZE NUMBER '%'
      { yy.emitter.emit_led_instruction(yy.symbols.get_reserved_word($1).opcode | INSTRUCTION_MODIFIER_IMMEDIATE | ($4 & 0xff), @1); }
  | SLEEP parameter
      { yy.emitter.emit(yy.symbols.get_reserved_word($1).opcode | $2); }
  | SKIP IF test_expression
  | expression
  ;

test_expression
  : VARIABLE test_operator parameter
      { yy.emitter.emit(yy.symbols.get_reserved_word($2).opcode | ($1 << 16) | $3); }
  | LED_ID test_operator parameter
      /* All LED relates tests have 0x02 set in the opcode */
      { yy.emitter.emit(yy.symbols.get_reserved_word($2).opcode | INSTRUCTION_MODIFIER_LED | ($1 << 16) | $3); }
  | ANY car_state_list
      { yy.emitter.emit(yy.symbols.get_reserved_word($1).opcode | yy.symbols.get_symbol($2, "EXPECTING_CAR_STATE").opcode); }
  | ALL car_state_list
      { yy.emitter.emit(yy.symbols.get_reserved_word($1).opcode | yy.symbols.get_symbol($2, "EXPECTING_CAR_STATE").opcode); }
  | NONE car_state_list
      { yy.emitter.emit(yy.symbols.get_reserved_word($1).opcode | yy.symbols.get_symbol($2, "EXPECTING_CAR_STATE").opcode); }
  | IS CAR_STATE
      { yy.emitter.emit(yy.symbols.get_reserved_word($1).opcode | yy.symbols.get_symbol($2, "EXPECTING_CAR_STATE").opcode); }
  | NOT CAR_STATE
      { yy.emitter.emit(yy.symbols.get_reserved_word($1).opcode | yy.symbols.get_symbol($2, "EXPECTING_CAR_STATE").opcode); }
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
      { yy.emitter.emit(yy.symbols.get_reserved_word($2).opcode | (yy.symbols.get_symbol($1).opcode << 16) | $3); }
  | GLOBAL_VARIABLE assignment_operator parameter
      { yy.emitter.emit(yy.symbols.get_reserved_word($2).opcode | (yy.symbols.get_symbol($1).opcode << 16) | $3); }
  | leds '=' led_assignment_parameter
      { yy.emitter.emit_led_instruction(0x02000000 | $3, @1); }
  ;

leds
  : ALL LEDS
    { yy.emitter.add_led_to_list(-1); }
  | led_list
  ;

led_list
  : LED_ID
      { yy.emitter.add_led_to_list(yy.symbols.get_symbol($1).opcode); }
  | leds ',' LED_ID
      { yy.emitter.add_led_to_list(yy.symbols.get_symbol($3).opcode); }
  ;

led_assignment_parameter
  : NUMBER
      /* All opcodes that work with immediates have the lowest bit set */
      { $$ = INSTRUCTION_MODIFIER_IMMEDIATE | (Number($1) & 0xff); }
  | NUMBER '%'
      /* All opcodes that work with immediates have the lowest bit set */
      { $$ = INSTRUCTION_MODIFIER_IMMEDIATE | (Number($1) & 0xff); }
  | VARIABLE
      { $$ = yy.symbols.get_symbol($1).opcode; }
  | GLOBAL_VARIABLE
      { $$ = yy.symbols.get_symbol($1).opcode; }
  ;

parameter
  : NUMBER
      /* All opcodes that work with immediates have the lowest bit set */
      { $$ = INSTRUCTION_MODIFIER_IMMEDIATE | (Number($1) & 0xffff); }
  | VARIABLE
      { $$ = (PARAMETER_TYPE_VARIABLE << 8) | yy.symbols.get_symbol($1).opcode; }
  | GLOBAL_VARIABLE
      { $$ = (PARAMETER_TYPE_VARIABLE << 8) | yy.symbols.get_symbol($1).opcode; }
  | LED_ID
      { $$ = (PARAMETER_TYPE_LED << 8) | yy.symbols.get_symbol($1).opcode; }
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
