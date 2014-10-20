%{

#include "symbols.h"
#include "parser.h"
int get_reserved_word(union YYSTYPE *result, const char *yytext);
int get_symbol(union YYSTYPE *result, const char *name);

%}

DIGIT    [0-9]
HEXDIGIT [0-9a-fA-F]
ID       [a-z][a-z0-9_\-]*

%option bison-bridge
%option bison-locations
%option noyywrap

%option warn
%option nodefault

%%

"0x"{HEXDIGIT}+ {
    yylval->immediate = (int16_t)strtol(yytext, NULL, 16);
    fprintf(stderr, "SCANNER: Hex-number %d\n", yylval->immediate);
    return NUMBER;
}

"-"?{DIGIT}+ {
    yylval->immediate = (int16_t)strtol(yytext, NULL, 10);
    fprintf(stderr, "SCANNER: Number %d\n", yylval->immediate);
    return NUMBER;
}

goto|var|led|wait|skip|if|is|any|all|none|not|fade|run|when|or|master|slave|global|random|steering|throttle|abs {
    int token = get_reserved_word(yylval, yytext);
    printf("SCANNER: Reserved word %s (%s)\n", yytext, token2str(token));
    return token;
}

{ID} {
    int token = get_symbol(yylval, yytext);
    printf("SCANNER: Identifier: %s (%s)\n", yytext, token2str(token));
    return token;
}

"="|"+="|"-="|"*="|"/="|"&="|"|="|"^=" {
    int token = get_reserved_word(yylval, yytext);
    printf("SCANNER: Assignment %s (%s)\n", yytext, token2str(token));
    return token;
}

"=="|"!="|">="|"<="|">"|"<" {
    int token = get_reserved_word(yylval, yytext);
    printf("SCANNER: Comparison %s (%s)\n", yytext, token2str(token));
    return token;
}

"["|"]"|","|":" {
    printf("SCANNER: '%c'\n", *yytext);
    return *yytext;
}

"//"[^\n]*  /* eat up one-line comments */
";"[^\n]*   /* eat up one-line comments */

[ \t]+\n    /* skip emtpy lines */
[ \t]+      /* eat up whitespace */

\n+ {       /* Fold multiple consecutive \n into a single \n */
     printf("SCANNER: '\\n'\n");
     parse_state = UNKNOWN_PARSE_STATE;
     return *yytext;
}

. {
    printf("SCANNER: ERROR: Unrecognized character: %s\n", yytext);
    exit(1);
}

%%
