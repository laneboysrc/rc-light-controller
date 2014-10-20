%{

#include "symbols.h"
#include "parser.h"

int get_reserved_word(union YYSTYPE *result, const char *yytext);
int get_symbol(union YYSTYPE *result, const char *name);
const char *token2str(int token);

// FIXME: add location tracking
// FIXME: test if empty lines and comments fully work!



%}

%option bison-bridge
%option bison-locations
%option noyywrap

%option warn
%option nodefault

%%

0[xX][[:xdigit:]]+ {
    yylval->immediate = (int16_t)strtol(yytext, NULL, 16);
    fprintf(stderr, "SCANNER: Hex-number: %d\n", yylval->immediate);
    return NUMBER;
}

("-"?)[[:digit:]]+ {
    yylval->immediate = (int16_t)strtol(yytext, NULL, 10);
    fprintf(stderr, "SCANNER: Number: %d\n", yylval->immediate);
    return NUMBER;
}

goto|var|led|wait|skip|if|is|any|all|none|not|fade|run|when|or|master|slave|global|random|steering|throttle|abs {
    int token = get_reserved_word(yylval, yytext);
    fprintf(stderr, "SCANNER: Reserved word: %s (%s)\n",
        yytext, token2str(token));
    return token;
}

[[:alpha:]][[:alnum:]_\-]* {
    int token = get_symbol(yylval, yytext);
    fprintf(stderr, "SCANNER: Identifier: %s (%s)\n", yytext, token2str(token));
    return token;
}

"="|"+="|"-="|"*="|"/="|"&="|"|="|"^=" {
    int token = get_reserved_word(yylval, yytext);
    fprintf(stderr, "SCANNER: Assignment %s (%s)\n", yytext, token2str(token));
    return token;
}

"=="|"!="|">="|"<="|">"|"<" {
    int token = get_reserved_word(yylval, yytext);
    fprintf(stderr, "SCANNER: Comparison %s (%s)\n", yytext, token2str(token));
    return token;
}

"["|"]"|","|":" {
    fprintf(stderr, "SCANNER: '%c'\n", *yytext);
    return *yytext;
}

"//"[^\n]*  /* eat up one-line comments */
";"[^\n]*   /* eat up one-line comments */

[ \t]+\n    /* skip emtpy lines */
[ \t]+      /* eat up whitespace */

\n+ {       /* Fold multiple consecutive \n into a single \n */
     fprintf(stderr, "SCANNER: '\\n'\n");
     parse_state = UNKNOWN_PARSE_STATE;
     return *yytext;
}

. {
    fprintf(stderr, "SCANNER: ERROR: Unrecognized character: %s\n", yytext);
    exit(1);
}

%%
