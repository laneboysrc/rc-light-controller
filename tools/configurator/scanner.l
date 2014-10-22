%{

#include "symbols.h"
#include "parser.h"
#include "log.h"

#define MODULE "scanner"

int get_reserved_word(union YYSTYPE *result, const char *yytext);
int get_symbol(union YYSTYPE *result, const char *name);
const char *token2str(int token);

static int offset = 1;

// FIXME: test if empty lines and comments fully work!


#define YY_USER_ACTION \
    yylloc->first_line = yylineno; \
    yylloc->last_line = yylineno; \
    yylloc->first_column = offset; \
    offset += yyleng; \
    yylloc->last_column = offset;


%}

%option bison-bridge
%option bison-locations
%option noyywrap

%option warn
%option nodefault

%%

0[xX][[:xdigit:]]+ {
    yylval->immediate = (int16_t)strtol(yytext, NULL, 16);
    log_message(MODULE, INFO, "Hex-number: %d\n", yylval->immediate);
    return NUMBER;
}

("-"?)[[:digit:]]+ {
    yylval->immediate = (int16_t)strtol(yytext, NULL, 10);
    log_message(MODULE, INFO, "Number: %d\n", yylval->immediate);
    return NUMBER;
}

goto|var|led|wait|skip|if|is|any|all|none|not|fade|run|when|or|master|slave|global|random|steering|throttle|abs {
    int token = get_reserved_word(yylval, yytext);
    log_message(MODULE, INFO,
        "Reserved word: %s (%s)\n", yytext, token2str(token));
    return token;
}

[[:alpha:]][[:alnum:]_\-]* {
    int token = get_symbol(yylval, yytext);
    log_message(MODULE, INFO,
        "Identifier: %s (%s)\n", yytext, token2str(token));
    return token;
}

"="|"+="|"-="|"*="|"/="|"&="|"|="|"^=" {
    int token = get_reserved_word(yylval, yytext);
    log_message(MODULE, INFO, "Assignment %s (%s)\n", yytext, token2str(token));
    return token;
}

"=="|"!="|">="|"<="|">"|"<" {
    int token = get_reserved_word(yylval, yytext);
    log_message(MODULE, INFO,
        "Comparison %s (%s)\n", yytext, token2str(token));
    return token;
}

"["|"]"|","|":" {
    log_message(MODULE, INFO, "'%c'\n", *yytext);
    return *yytext;
}

"//"[^\n]*  /* eat up one-line comments */
";"[^\n]*   /* eat up one-line comments */

[ \t]+\n { /* skip emtpy lines */
    offset = 1;
     ++yylineno;
}

[ \t]+      /* eat up whitespace */

\n+ {       /* Fold multiple consecutive \n into a single \n */
     offset = 1;
     yylineno += yyleng;
     log_message(MODULE, INFO, "'\\n'\n");
     parse_state = UNKNOWN_PARSE_STATE;
     return *yytext;
}

. {
    log_message(MODULE, WARNING, "Unrecognized character: %c\n", *yytext);
    return *yytext;
}

%%
