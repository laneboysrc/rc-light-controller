#pragma once

#include "symbols.h"
#include "parser.h"

typedef enum {
    DEBUG,
    INFO,
    WARNING,
    ERROR
} LOG_TYPE_T;


void log_printf(const char *fmt, ...);
void log_message(const char *module, LOG_TYPE_T type, const char *fmt, ...);
void yyerror(struct YYLTYPE *loc, const char *msg);
int has_error_occured(void);
void log_enable(void);
