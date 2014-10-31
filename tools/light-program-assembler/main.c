#include <stdio.h>
#include <stdlib.h>

#include "symbols.h"
#include "emitter.h"
#include "parser.h"
#include "log.h"


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
    yyparse();
    output_programs();
    return has_error_occured() ? 1 : 0;
}
