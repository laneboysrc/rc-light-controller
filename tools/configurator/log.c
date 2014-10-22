#include <stdio.h>
#include <stdarg.h>

#include "log.h"


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


void log_message(const char *module, LOG_TYPE_T type, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);

    fprintf(stderr, "%-8s [%-7s]: ", module, get_log_type_string(type));
    vfprintf(stderr, fmt, ap);

    va_end(ap);
}