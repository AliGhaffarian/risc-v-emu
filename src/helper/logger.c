#define _GNU_SOURCE
#include "logger.h"
#include <dlfcn.h>
#include <string.h>

volatile int current_log_level = LOG_INFO;

const char *LOG_LEVELS2STR[] = {
    [LOG_UNSPEC] = "ERR: out of bound enum",
    [LOG_NOLOG]  = "NOLOG",
    [LOG_INFO]   = "INFO",
    [LOG_WARN]   = "WARN",
    [LOG_ERROR]  = "ERROR",
    [LOG_DEBUG]  = "DEBUG",
    [LOG_SIZE]   = "ERR:out of bound enum",
};

enum LOG_LEVELS enum_from_string_log_levels(char *str)
{
    for(int i = LOG_UNSPEC + 1; i < LOG_SIZE; i++) {
        if(!strcmp(str, LOG_LEVELS2STR[i])) {
            return i;
        }
    }
    return 0;
}

#define NOINST __attribute__((no_instrument_function))

// NOLINTNEXTLINE
NOINST void __cyg_profile_func_enter(void *fn, void *caller)
{
    Dl_info callee;
    dladdr(fn, &callee);
    logger(LOG_DEBUG, stdout, "%s was called\n", callee.dli_sname);
}
