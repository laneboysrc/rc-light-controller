
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>

#include "parser.h"
#include "light_programs.tab.h"


typedef struct {
    const char *name;
    int token;
    uint32_t opcode;
} identifier_initializer;

identifier_initializer run_condition_tokens[] = {
  {.name = "always", .token = RUN_CONDITION_ALWAYS, .opcode = (1 << 31)},

  {.name = "light-switch-position-0", .token = RUN_CONDITION, .opcode = (1 << 0)},
  {.name = "light-switch-position-1", .token = RUN_CONDITION, .opcode = (1 << 1)},
  {.name = "light-switch-position-2", .token = RUN_CONDITION, .opcode = (1 << 2)},
  {.name = "light-switch-position-3", .token = RUN_CONDITION, .opcode = (1 << 3)},
  {.name = "light-switch-position-4", .token = RUN_CONDITION, .opcode = (1 << 4)},
  {.name = "light-switch-position-5", .token = RUN_CONDITION, .opcode = (1 << 5)},
  {.name = "light-switch-position-6", .token = RUN_CONDITION, .opcode = (1 << 6)},
  {.name = "light-switch-position-7", .token = RUN_CONDITION, .opcode = (1 << 7)},
  {.name = "light-switch-position-8", .token = RUN_CONDITION, .opcode = (1 << 8)},
  
  {.name = "neutral", .token = RUN_CONDITION, .opcode = (1 << 9)},
  {.name = "forward", .token = RUN_CONDITION, .opcode = (1 << 10)},
  {.name = "reversing", .token = RUN_CONDITION, .opcode = (1 << 11)},
  {.name = "braking", .token = RUN_CONDITION, .opcode = (1 << 12)},
  
  {.name = "indicator-left", .token = RUN_CONDITION, .opcode = (1 << 13)},
  {.name = "indicator-right", .token = RUN_CONDITION, .opcode = (1 << 14)},
  {.name = "hazard", .token = RUN_CONDITION, .opcode = (1 << 15)},
  {.name = "blink-flag", .token = RUN_CONDITION, .opcode = (1 << 16)},
  {.name = "blink-left", .token = RUN_CONDITION, .opcode = (1 << 17)},
  {.name = "blink-right", .token = RUN_CONDITION, .opcode = (1 << 18)},

  {.name = "winch-disabled", .token = RUN_CONDITION, .opcode = (1 << 19)},
  {.name = "winch-idle", .token = RUN_CONDITION, .opcode = (1 << 20)},
  {.name = "winch-in", .token = RUN_CONDITION, .opcode = (1 << 21)},
  {.name = "winch-out", .token = RUN_CONDITION, .opcode = (1 << 22)},

  {.name = "gear-1", .token = RUN_CONDITION, .opcode = (1 << 23)},
  {.name = "gear-2", .token = RUN_CONDITION, .opcode = (1 << 24)},

  {.name = "no-signal", .token = PRIORITY_RUN_CONDITION, .opcode = (1 << 0)},
  {.name = "initializing", .token = PRIORITY_RUN_CONDITION, .opcode = (1 << 1)},
  {.name = "servo-output-setup-centre", .token = PRIORITY_RUN_CONDITION, .opcode = (1 << 2)},
  {.name = "servo-output-setup-left", .token = PRIORITY_RUN_CONDITION, .opcode = (1 << 3)},
  {.name = "servo-output-setup-right", .token = PRIORITY_RUN_CONDITION, .opcode = (1 << 4)},
  {.name = "reversing-setup-steering", .token = PRIORITY_RUN_CONDITION, .opcode = (1 << 5)},
  {.name = "reversing-setup-throttle", .token = PRIORITY_RUN_CONDITION, .opcode = (1 << 6)},
  {.name = "gear-changed", .token = PRIORITY_RUN_CONDITION, .opcode = (1 << 7)},

  {.name = NULL, .token = EOF},
};

identifier_initializer car_state[] = {
  {.name = "light-switch-position-0", .token = CAR_STATE, .opcode = (1 << 0)},
  {.name = "light-switch-position-1", .token = CAR_STATE, .opcode = (1 << 1)},
  {.name = "light-switch-position-2", .token = CAR_STATE, .opcode = (1 << 2)},
  {.name = "light-switch-position-3", .token = CAR_STATE, .opcode = (1 << 3)},
  {.name = "light-switch-position-4", .token = CAR_STATE, .opcode = (1 << 4)},
  {.name = "light-switch-position-5", .token = CAR_STATE, .opcode = (1 << 5)},
  {.name = "light-switch-position-6", .token = CAR_STATE, .opcode = (1 << 6)},
  {.name = "light-switch-position-7", .token = CAR_STATE, .opcode = (1 << 7)},
  {.name = "light-switch-position-8", .token = CAR_STATE, .opcode = (1 << 8)},
  
  {.name = "neutral", .token = CAR_STATE, .opcode = (1 << 9)},
  {.name = "forward", .token = CAR_STATE, .opcode = (1 << 10)},
  {.name = "reversing", .token = CAR_STATE, .opcode = (1 << 11)},
  {.name = "braking", .token = CAR_STATE, .opcode = (1 << 12)},
  
  {.name = "indicator-left", .token = CAR_STATE, .opcode = (1 << 13)},
  {.name = "indicator-right", .token = CAR_STATE, .opcode = (1 << 14)},
  {.name = "hazard", .token = CAR_STATE, .opcode = (1 << 15)},
  {.name = "blink-flag", .token = CAR_STATE, .opcode = (1 << 16)},
  {.name = "blink-left", .token = CAR_STATE, .opcode = (1 << 17)},
  {.name = "blink-right", .token = CAR_STATE, .opcode = (1 << 18)},
  
  {.name = "winch-disabled", .token = CAR_STATE, .opcode = (1 << 19)},
  {.name = "winch-idle", .token = CAR_STATE, .opcode = (1 << 20)},
  {.name = "winch-in", .token = CAR_STATE, .opcode = (1 << 21)},
  {.name = "winch-out", .token = CAR_STATE, .opcode = (1 << 22)},
  
  {.name = "gear-1", .token = CAR_STATE, .opcode = (1 << 23)},
  {.name = "gear-2", .token = CAR_STATE, .opcode = (1 << 24)},

  {.name = NULL, .token = EOF},
};

identifier_initializer reserved_words[] = {
  {.name = "goto", .token = GOTO, .opcode = 0x01000000},
  {.name = "var", .token = VAR},
  {.name = "led", .token = LED},
  {.name = "fade", .token = FADE, .opcode = 0x04000000},
  {.name = "wait", .token = WAIT, .opcode = 0x06000000},
  {.name = "skip", .token = SKIP},
  {.name = "if", .token = IF},
  {.name = "any", .token = ANY, .opcode = 0x60000000},
  {.name = "all", .token = ALL, .opcode = 0x80000000},
  {.name = "none", .token = NONE, .opcode = 0xa0000000},
  {.name = "not", .token = NOT, .opcode = 0xa0000000},
  {.name = "is", .token = IS, .opcode = 0x60000000},
  {.name = "run", .token = RUN},
  {.name = "when", .token = WHEN},
  {.name = "or", .token = OR},
  {.name = "master", .token = MASTER},
  {.name = "slave", .token = SLAVE},
  {.name = "global", .token = GLOBAL},
  {.name = "random", .token = RANDOM},
  {.name = "steering", .token = STEERING},
  {.name = "throttle", .token = THROTTLE},
  {.name = "abs", .token = ABS, .opcode = 0x40000000},

  {.name = NULL, .token = EOF},
};

identifier *symbol_table = NULL;
identifier *run_condition_table = NULL;
identifier *car_state_table = NULL;
identifier *reserved_words_table = NULL;

int next_variable_index = 0;
unsigned int pc = 0;


static identifier *add_symbol(identifier **table, const char *name, int token, 
    int index, uint32_t opcode)
{
  identifier *ptr = (identifier *)calloc(sizeof(identifier), 1);
  //FIXME: check allocation fail
  ptr->name = (char *)calloc(strlen(name) + 1, 1);
  //FIXME: check allocation fail
  strcpy(ptr->name, name);
  ptr->token = token;
  ptr->index = index;
  ptr->opcode = opcode;
  ptr->next = *table;
  *table = ptr;
  return ptr;
}


static identifier *get_symbol(identifier **table, const char *name)
{
  identifier *ptr;
  for (ptr = *table; ptr != NULL; ptr = ptr->next) {
    if (strcmp(ptr->name, name) == 0) {
      return ptr;
    }
  }
  return NULL;
}


void set_identifier(identifier *s, int token, int index)
{
    s->token = token;
    s->index = (index != -1) ? index : next_variable_index++;

    printf("++++++++++> Set IDENTIFIER '%s' as token=%s, index=%d\n",
      s->name, token2str(s->token), s->index);
}


static void initialize_symbol_table(const identifier_initializer *source,
  identifier **destination)
{
    while (source->name) {
        add_symbol(destination, source->name, source->token, 0, source->opcode);
        ++source;
    }
}


void emit(uint32_t instruction)
{
  printf("===============> INSTRUCTION: 0x%08x\n", instruction);
  ++pc;
}


void yyerror(const char *s)
{
  fprintf(stderr, "ERROR: %s\n", s);
}


int yylex(void)
{
  static size_t length = 40;
  static char *symbuf = NULL;
  int c;
  static int empty_line = 1;

  if (symbuf == NULL){
    symbuf = (char *)calloc(length + 1, 1);
    // FIXME: need to add check for malloc failed...
  }

  switch (parse_state) {
    case UNKNOWN_PARSE_STATE:
      break;

    case EXPECTING_RUN_CONDITION:
      printf("----------> Expecting run condition\n");
      break;

    case EXPECTING_CAR_STATE:
      printf("----------> Expecting car state\n");
      break;

    default:
      printf("----------> FORGOT CASE FOR %d\n", parse_state);
      break;
  }


  /* Skip white space and empty lines */
  if (empty_line) {
    while ((c = getchar ()) == ' ' || c == '\t' || c == '\n') {
      continue;
    }
  }
  else {
    while ((c = getchar ()) == ' ' || c == '\t') {
      continue;
    }
  };

  empty_line = 0;


  // ===========================================================================
  // IDENTIFIERS

  if (isalpha(c)) {
    size_t count = 0;
    identifier *s;

    do {
      if (count == length) {
        length *= 2;
        symbuf = (char *) realloc(symbuf, length + 1);
        // FIXME: need to add check for malloc failed...
      }

      // Add this character to the buffer.
      symbuf[count++] = c;

      c = getchar();
    } while (isalnum(c) || c == '-');

    ungetc(c, stdin);
    symbuf[count] = '\0';

    // Check RUN CONDITION special symbols if we are expecting any

    if (parse_state == EXPECTING_RUN_CONDITION) {
      printf("++++++++++> Testing RUN CONDITION %s\n", symbuf);
      s = get_symbol(&run_condition_table, symbuf);
      if (s) {
          printf("++++++++++> Found %s %s\n", token2str(s->token), s->name);
        yylval.instruction = s->opcode;
        return s->token;
      }
    }

    // Check CAR STATE special symbols if we are expecting any

    if (parse_state == EXPECTING_CAR_STATE) {
      printf("++++++++++> Testing CAR STATE %s\n", symbuf);
      s = get_symbol(&car_state_table, symbuf);
      if (s) {
        printf("++++++++++> Found %s %s\n", token2str(s->token), s->name);
        yylval.instruction = s->opcode;
        return s->token;
      }
    }

    // Check reserved words

    printf("++++++++++> Testing RESERVED WORD %s\n", symbuf);
    s = get_symbol(&reserved_words_table, symbuf);
    if (s) {
      printf("++++++++++> Found RESERVED WORD %s\n", symbuf);
      yylval.instruction = s->opcode;
      return s->token;
    }

    // Check labels, variables and LEDs

    printf("++++++++++> Testing IDENTIFIER %s\n", symbuf);
    s = get_symbol(&symbol_table, symbuf);
    if (s == NULL) {
        s = add_symbol(&symbol_table, symbuf, IDENTIFIER, 0, 0);
        printf("++++++++++> Added IDENTIFIER %s (%s)\n", 
          s->name, token2str(s->token));
    }
    else {
      printf("++++++++++> Found IDENTIFIER %s (%s)\n", 
        s->name, token2str(s->token));
    }
    yylval.i = s;
    return s->token;
  }


  // ===========================================================================
  // NUMBERS

  // FIXME: add hexadecimal support

  if (isdigit(c)) {
    size_t count = 0;

    do {
      if (count == length) {
        length *= 2;
        symbuf = (char *) realloc(symbuf, length + 1);
        // FIXME: need to add check for malloc failed...
      }

      // Add this character to the buffer.
      symbuf[count++] = c;

      c = getchar();
    } while (isdigit(c));

    ungetc(c, stdin);
    symbuf[count] = '\0';

    yylval.immediate = (int16_t)strtol(symbuf, NULL, 10);
    return NUMBER;
  }

  if (c == '\n') {
    empty_line = 1;
    parse_state = UNKNOWN_PARSE_STATE;
    return c;
  }

  if (c == '+') {
    char n;
    if ((n = getchar()) == '=') {
      yylval.instruction = 0x12000000;
      return ADD_ASSIGN;
    }
    ungetc(n, stdin);
  }

  if (c == '-') {
    char n;
    if ((n = getchar()) == '=') {
      yylval.instruction = 0x14000000;
      return SUB_ASSIGN;
    }
    ungetc(n, stdin);
  }

  if (c == '*') {
    char n;
    if ((n = getchar()) == '=') {
      yylval.instruction = 0x16000000;
      return MUL_ASSIGN;
    }
    ungetc(n, stdin);
  }

  if (c == '/') {
    char n;
    if ((n = getchar()) == '=') {
      yylval.instruction = 0x18000000;
      return DIV_ASSIGN;
    }
    ungetc(n, stdin);
  }

  if (c == '&') {
    char n;
    if ((n = getchar()) == '=') {
      yylval.instruction = 0x1a000000;
      return AND_ASSIGN;
    }
    ungetc(n, stdin);
  }

  if (c == '|') {
    char n;
    if ((n = getchar()) == '=') {
      yylval.instruction = 0x1c000000;
      return OR_ASSIGN;
    }
    ungetc(n, stdin);
  }

  if (c == '^') {
    char n;
    if ((n = getchar()) == '=') {
      yylval.instruction = 0x1e000000;
      return XOR_ASSIGN;
    }
    ungetc(n, stdin);
  }

  if (c == '=') {
    char n;
    if ((n = getchar()) == '=') {
      yylval.instruction = 0x20000000;
      return EQ;
    }
    ungetc(n, stdin);
    yylval.instruction = 0x10000000;
    return ASSIGN;
  }

  if (c == '>') {
    char n;
    if ((n = getchar()) == '=') {
      yylval.instruction = 0x28000000;
      return GE;
    }
    ungetc(n, stdin);
    yylval.instruction = 0x2c000000;
    return GT;
  }

  if (c == '<') {
    char n;
    if ((n = getchar()) == '=') {
      yylval.instruction = 0x30000000;
      return LE;
    }
    ungetc(n, stdin);
    yylval.instruction = 0x34000000;
    return LT;
  }

  if (c == '!') {
    char n;
    if ((n = getchar()) == '=') {
      yylval.instruction = 0x24000000;
      return NE;
    }
    ungetc(n, stdin);
  }
  
  if (c == ':') {
    return c;
  }

  if (c == EOF) {
    return 0;
  }

  return c;
}


int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  printf("Bison test parser\n");
  yydebug = 1;

  initialize_symbol_table(run_condition_tokens, &run_condition_table);
  initialize_symbol_table(reserved_words, &reserved_words_table);
  initialize_symbol_table(car_state, &car_state_table);

  // Pre-load global special variable named "clicks" that increments
  // on every six CH3-clicks.
  add_symbol(
    &symbol_table, "clicks", VARIABLE, next_variable_index++, 0);

  return yyparse();
}
