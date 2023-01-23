#include "util/log.h"

#include <stdlib.h>
#include <stdarg.h>

FILE *log_ostream;

void log_init(void)
{
    log_ostream = stdout;
}

void _log_log(char const *file, char const *fn, unsigned long line,
              char const *fmt, ...)
{
    fprintf(log_ostream, "[ LL ] (%s) %s:%lu ", file, fn, line);
    
    va_list args;
    va_start(args, fmt);
    vfprintf(log_ostream, fmt, args);
    va_end(args);
    
    putc('\n', log_ostream);
}

void _log_warning(char const *file, char const *fn, unsigned long line,
                  char const *fmt, ...)
{
    fprintf(log_ostream, "[ WW ] (%s) %s:%lu ", file, fn, line);
    
    va_list args;
    va_start(args, fmt);
    vfprintf(log_ostream, fmt, args);
    va_end(args);
    
    putc('\n', log_ostream);
}

void _log_error(char const *file, char const *fn, unsigned long line,
                char const *fmt, ...)
{
    fprintf(log_ostream, "[ EE ] (%s) %s:%lu ", file, fn, line);
    
    va_list args;
    va_start(args, fmt);
    vfprintf(log_ostream, fmt, args);
    va_end(args);
    
    putc('\n', log_ostream);
    exit(-1);
}
