%{

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include "symbols.h"
#include "parser.h"
#include "log.h"

#define MODULE "scanner"

int get_reserved_word(union YYSTYPE *result, const char *yytext);
int get_symbol(union YYSTYPE *result, const char *name, YYLTYPE *location);
const char *token2str(int token);

static int offset = 1;
static int number_of_input_files;
static char **input_file_list;
static bool line_is_empty;

#define YY_USER_ACTION \
    yylloc->first_line = yylineno; \
    yylloc->last_line = yylineno; \
    yylloc->first_column = offset; \
    offset += yyleng; \
    yylloc->last_column = offset;

void initialize_scanner(int count, char **files)
{
    FILE *f;

    number_of_input_files = count;
    input_file_list = files;

    if (number_of_input_files > 1) {
        fprintf(stderr, "ERROR: Only one input file can be assembled\n");
        exit(1);
    }

    f = fopen(input_file_list[0], "r");
    if (f == NULL) {
        fprintf(stderr, "ERROR: failed to open file %s for reading: %s\n",
            input_file_list[0], strerror(errno));
        exit(1);
    }

    yyin = f;
    line_is_empty = true;
}

%}

%option bison-bridge
%option bison-locations
%option noyywrap
%option never-interactive

%option warn
%option nodefault

%%

0[xX][[:xdigit:]]+ {
    line_is_empty = false;
    yylval->immediate = (int16_t)strtol(yytext, NULL, 16);
    log_message(MODULE, INFO, "Hex-number: %d\n", yylval->immediate);
    return NUMBER;
}

("-"?)[[:digit:]]+ {
    line_is_empty = false;
    yylval->immediate = (int16_t)strtol(yytext, NULL, 10);
    log_message(MODULE, INFO, "Number: %d\n", yylval->immediate);
    return NUMBER;
}

goto|var|led|sleep|skip|if|is|any|all|none|not|fade|stepsize|run|when|or|master|slave|global|random|steering|throttle|abs|__NEXT_PROGRAM__ {
    line_is_empty = false;
    int token = get_reserved_word(yylval, yytext);
    log_message(MODULE, INFO,
        "Reserved word: %s (%s)\n", yytext, token2str(token));
    return token;
}

[[:alpha:]][[:alnum:]_\-]* {
    line_is_empty = false;
    int token = get_symbol(yylval, yytext, yylloc);
    log_message(MODULE, INFO,
        "Identifier: %s (%s)\n", yytext, token2str(token));
    return token;
}

"="|"+="|"-="|"*="|"/="|"&="|"|="|"^=" {
    line_is_empty = false;
    int token = get_reserved_word(yylval, yytext);
    log_message(MODULE, INFO, "Assignment %s (%s)\n", yytext, token2str(token));
    return token;
}

"=="|"!="|">="|"<="|">"|"<" {
    line_is_empty = false;
    int token = get_reserved_word(yylval, yytext);
    log_message(MODULE, INFO,
        "Comparison %s (%s)\n", yytext, token2str(token));
    return token;
}

"["|"]"|","|":"|"%" {
    line_is_empty = false;
    log_message(MODULE, INFO, "'%c'\n", *yytext);
    return *yytext;
}

"//"[^\n]*  /* eat up one-line comments */
";"[^\n]*   /* eat up one-line comments */
[ \t]+      /* eat up whitespace */

\\[ \t]*(("//"|;)[^\n]*)*\n {
    /* eat up continuation characters '\', and the following \n */
    /* Comments can appear in after the '\' character */
    offset = 1;
    ++yylineno;
}

\n {        /* Only ever give back a single in sequence \n */
    parse_state = UNKNOWN_PARSE_STATE;
    offset = 1;
    ++yylineno;

    if (!line_is_empty) {
        line_is_empty = true;
        log_message(MODULE, INFO, "'\\n'\n");
        return *yytext;
    }
}

. {
    log_message(MODULE, WARNING, "Unrecognized character: %c\n", *yytext);
    return *yytext;
}

%%
