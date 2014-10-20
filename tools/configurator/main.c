#include <stdio.h>
#include <stdlib.h>

#include "symbols.h"
#include "emitter.h"
#include "parser.h"

void yyerror(const char *s);


// ****************************************************************************
void yyerror(const char *s)
{
    fprintf(stderr, "PARSER ERROR: %s\n", s);
}


// ****************************************************************************
int main(int argc, char *argv[])
{
    (void)argv;

    fprintf(stderr, "DIY RC Light Controller test parser\n\n");

    if (argc > 1) {
        yydebug = 1;
    }

    initialize_emitter();
    initialize_symbols();

    return yyparse();
}
