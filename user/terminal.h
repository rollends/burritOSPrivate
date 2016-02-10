#ifndef TERMINAL_H
#define TERMINAL_H

/**
 * Prints a formated string
 *
 * @param   server  The Terminal Output Driver ID
 * @param   str     The format string 
 * @param   va_args Optional args that match the print string
 */
S32 printf(TaskID server, char const * str, ...);

U32 strlen(char const *);

void strcpy(char *, char const *);

void putc(TaskID server, char c);

char getc(TaskID server);

#endif
