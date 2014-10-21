#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "symbols.h"
#include "emitter.h"
#include "parser.h"

bool error_occured = false;

void yyerror(struct YYLTYPE *loc, const char *msg);


// ****************************************************************************
void yyerror(struct YYLTYPE *loc, const char *msg)
{
    fprintf(stderr, "SYNTAX ERROR: %d:%d: %s\n",
        loc->first_line, loc->first_column, msg);
    error_occured = true;
}


// ****************************************************************************
int main(int argc, char *argv[])
{
    int result;

    (void)argv;

    fprintf(stderr, "DIY RC Light Controller test parser\n\n");

    if (argc > 1) {
        yydebug = 1;
    }

    initialize_emitter();
    initialize_symbols();

    result = yyparse();

    output_programs();

    if (error_occured) {
        return 1;
    }

    return result;
}
