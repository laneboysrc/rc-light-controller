/* Bison test */

%{
  /* Prologue */
  #include <stdlib.h>
  #include <stdio.h>
  #include <ctype.h>

  int yylex(void);
  void yyerror(char const *);
%}


/* Bison declarations */
%define api.value.type {char *}
%token COMMAND


%%
/* Grammar rules */

input:
  %empty
| input line
;

line:
  '\n'
| command '\n'  { printf("command: %s\n", $1); }
;

command:
  COMMAND
;

%%
/* Epilogue */

int yylex(void)
{
  int c;

  c = getchar();

  if (c == EOF) {
    return 0;
  }

  if (c == '\n') {
    return c;
  }

  else {
    static size_t length = 40;
    static char *symbuf = NULL;
    int count = 0;

    if (symbuf == NULL){
      symbuf = (char *) malloc(length + 1);
      // FIXME: need to add check for malloc failed...
    }

    do {
      if (count == length) {
        length *= 2;
        symbuf = (char *) realloc(symbuf, length + 1);
        // FIXME: need to add check for malloc failed...
      }

      // Add this character to the buffer.
      symbuf[count++] = c;

      c = getchar();
    } while (c != '\n'  &&  c != EOF);

    ungetc(c, stdin);
    symbuf[count] = '\0';

    yylval = symbuf;
    return COMMAND;
  }
}

/* Called by yyparse on error.  */
void yyerror(char const *s)
{
  fprintf(stderr, "ERROR: %s\n", s);
}

int main(int argc, char *argv[])
{
  printf("Bison test parser\n");
  return yyparse();
}
