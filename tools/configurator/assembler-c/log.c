#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#include "log.h"
#include "parser.h"


static bool error_occured = false;


// ****************************************************************************
static const char *get_log_type_string(LOG_TYPE_T t)
{
    switch (t) {
        case DEBUG:
            return "DEBUG";

        case INFO:
            return "INFO";

        case WARNING:
            return "WARNING";

        case ERROR:
            return "ERROR";

        default:
            return "???";
    }
}


// ****************************************************************************
int has_error_occured(void)
{
    return error_occured ? 1 : 0;
}


// ****************************************************************************
void log_message(const char *module, LOG_TYPE_T type, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);

    fprintf(stderr, "%-8s [%-7s]: ", module, get_log_type_string(type));
    vfprintf(stderr, fmt, ap);

    va_end(ap);
}



// ****************************************************************************
void yyerror(struct YYLTYPE *loc, const char *msg)
{
    if (loc != NULL) {
        fprintf(stderr, "%d:%d: ERROR: %s\n", loc->first_line, loc->first_column, msg);
    }
    else {
        fprintf(stderr, "ERROR: %s\n", msg);
    }

    error_occured = true;
}