%{
#include "parser.h"
#include "light_programs.tab.h"
%}

DIGIT    [0-9]
HEXDIGIT [0-9a-fA-F]
ID       [a-z][a-z0-9]*

%option bison-bridge
%option bison-locations
%option noyywrap
%option warn

%%

"0x"{HEXDIGIT}+ {
    yylval = (int16_t)strtol(yytext, NULL, 16);
    fprintf(stderr, "lexer: NUMBER %d\n", yylval);
    return NUMBER;
}

"-"?{DIGIT}+ {
    yylval = (int16_t)strtol(yytext, NULL, 16);
    fprintf(stderr, "lexer: NUMBER %d\n", yylval);
    return NUMBER;
}

goto|var|led|wait|skip|if|any|all|none|not|fade|run|when|or|master|slave|global|random|steering|throttle|abs {
    int token = get_reserved_word(&yylval, yytext);
    printf("lexer: RESERVED WORD %s (%s)\n", yytext, token2str(token));
    return token;
}

{ID} {
    int token = get_symbol(&yylval, yytext);
    printf("lexer: IDENTIFIER: %s (%s)\n", yytext, token2str(token));
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
