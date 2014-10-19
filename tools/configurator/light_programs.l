/* scanner for a toy Pascal-like language */

%{
    int yywrap(void);
%}

DIGIT    [0-9]
HEXDIGIT [0-9a-fA-F]
ID       [a-z][a-z0-9]*

%%

"0x"{HEXDIGIT}+ {
        printf("HEX: %s (%d)\n", yytext, atoi(yytext));
    }

"-"?{DIGIT}+ {
        printf("INTEGER: %s (%d)\n", yytext, atoi(yytext));
    }

goto|var|led|wait|skip|if|any|all|none|not|fade|run|when|or|master|slave|global|random|steering|throttle|abs {
        printf("RESERVED WORKD: %s\n", yytext);
    }

{ID} {
         printf("IDENTIFIER: %s\n", yytext);
    }

"="|"+="|"-="|"*="|"/="|"&="|"|="|"^=" {
        printf("ASSIGNMENT: %s\n", yytext);
    }

"//"[^\n]*  /* eat up one-line comments */
";"[^\n]*   /* eat up one-line comments */

[ \t]+      /* eat up whitespace */

"["|"]" {
        printf("BRACKETS: %s\n", yytext);
    }

\n {
        printf("New line\n");
    }

. {
        printf("Unrecognized character: %s\n", yytext);
    }



%%

int yywrap(void)
{
    return 1;
}

int main(int argc, char **argv)
{
    /* Skip over program name */
    ++argv;
    --argc;
    if ( argc > 0 ) {
        yyin = fopen( argv[0], "r" );
    }
    else {
        yyin = stdin;
    }

    yylex();
}