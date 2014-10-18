#include <stdio.h>

#include "parser.h"
#include "light_programs.tab.h"


// ****************************************************************************
void emit(uint32_t instruction)
{
    printf("####################> INSTRUCTION: 0x%08x\n", instruction);
    ++pc;
}


// ****************************************************************************
void yyerror(const char *s)
{
    fprintf(stderr, "ERROR: %s\n", s);
}


// ****************************************************************************
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("Bison test parser\n");
    // yydebug = 1;

    initialize_lexer();

    return yyparse();
}
