#ifndef TERMINAL_H
#define TERMINAL_H

#include "common/types.h"

/**
 * Prints a formated string
 *
 * @param   server  The Terminal Output Driver ID
 * @param   str     The format string 
 * @param   va_args Optional args that match the print string
 */
S32 tprintf(TaskID server, ConstString str, ...);

S32 sprintf(String buffer, ConstString format, ...);

#define printf(a, ...) tprintf(nsWhoIs(Terminal), a,## __VA_ARGS__)

void putc(TaskID server, char c);

char getc(TaskID server);

#endif
