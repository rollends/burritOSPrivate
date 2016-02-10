#ifndef PRINT_H
#define PRINT_H

#include "common/common.h"

/**
 * Prints a formated string over blocking io
 *
 * @param   str     The format string 
 * @param   va_args Optional args that match the print string
 */
S32 printBlocking(char const * str, ...);

/**
 * Prints a sad taco to the screen
 */
void printTaco();

#endif
