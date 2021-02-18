#include <stdbool.h>
#include <stdint.h>

#define BUF_SIZE 64

#define STDOUT_UART ((void *) 0)
#define STDOUT_USB ((void *) 1)

extern void watchdog_reset(void);
extern bool command_handler(uint16_t wValue);
