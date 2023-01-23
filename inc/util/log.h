#ifndef LOG_H__
#define LOG_H__

#include <stdio.h>

// internal: do not use.
#define LOG_ARGS__ __FILE__, __func__, __LINE__

// convenience macros, use these instead of the functions they expand to.
#define LOG(msg) _log_log(LOG_ARGS__, msg)
#define LOG_F(fmt, ...) _log_log(LOG_ARGS__, fmt, __VA_ARGS__)
#define WARNING(msg) _log_warning(LOG_ARGS__, msg)
#define WARNING_F(fmt, ...) _log_warning(LOG_ARGS__, fmt, __VA_ARGS__)
#define ERROR(msg) _log_error(LOG_ARGS__, msg)
#define ERROR_F(fmt, ...) _log_error(LOG_ARGS__, fmt, __VA_ARGS__)

// this is set to `stdout` upon `log_init()`.
// all logging will be written to this stream.
extern FILE *log_ostream;

void log_init(void);

// internal: do not call.
void _log_log(char const *file, char const *fn, unsigned long line,
              char const *fmt, ...);
void _log_warning(char const *file, char const *fn, unsigned long line,
                  char const *fmt, ...);
void _log_error(char const *file, char const *fn, unsigned long line,
                char const *fmt, ...);

#endif
