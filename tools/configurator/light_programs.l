%{
#include "parser.h"
#include "light_programs.tab.h"
int get_reserved_word(union YYSTYPE *result, const char *yytext);
int get_symbol(union YYSTYPE *result, const char *name);

%}

DIGIT    [0-9]
HEXDIGIT [0-9a-fA-F]
ID       [a-z][a-z0-9]*

%option bison-bridge
%option bison-locations
%option noyywrap

%option warn nodefault
/*%option outfile="scanner.c" header-file="scanner.h"*/

%option nounput noinput

/*
;    input, unput
;    yy_push_state, yy_pop_state, yy_top_state
;    yy_scan_buffer, yy_scan_bytes, yy_scan_string
;
;    yyget_extra, yyset_extra, yyget_leng, yyget_text,
;    yyget_lineno, yyset_lineno, yyget_in, yyset_in,
;    yyget_out, yyset_out, yyget_lval, yyset_lval,
;    yyget_lloc, yyset_lloc, yyget_debug, yyset_debug

*/
%%

"0x"{HEXDIGIT}+ {
    yylval->immediate = (int16_t)strtol(yytext, NULL, 16);
    fprintf(stderr, "scanner: NUMBER %d\n", yylval->immediate);
    return NUMBER;
}

"-"?{DIGIT}+ {
    yylval->immediate = (int16_t)strtol(yytext, NULL, 16);
    fprintf(stderr, "scanner: NUMBER %d\n", yylval->immediate);
    return NUMBER;
}

goto|var|led|wait|skip|if|any|all|none|not|fade|run|when|or|master|slave|global|random|steering|throttle|abs {
    int token = get_reserved_word(&yylval, yytext);
    printf("scanner: RESERVED WORD %s (%s)\n", yytext, token2str(token));
    return token;
}

{ID} {
    int token = get_symbol(&yylval, yytext);
    printf("scanner: IDENTIFIER: %s (%s)\n", yytext, token2str(token));
    return token;
}

"="|"+="|"-="|"*="|"/="|"&="|"|="|"^=" {
    printf("ASSIGNMENT: %s\n", yytext);
}

"//"[^\n]*  /* eat up one-line comments */
";"[^\n]*   /* eat up one-line comments */

[ \t]+\n    /* skip emtpy lines */
[ \t]+      /* eat up whitespace */

"["|"]" {
    printf("BRACKETS: %s\n", yytext);
}


\n {
     parse_state = UNKNOWN_PARSE_STATE;
     return '\n';
}

. {
    printf("Unrecognized character: %s\n", yytext);
}



%%
