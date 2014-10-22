#pragma once

typedef enum {
    DEBUG,
    INFO,
    WARNING,
    ERROR
} LOG_TYPE_T;


void log_message(const char *module, LOG_TYPE_T type, const char *fmt, ...);
