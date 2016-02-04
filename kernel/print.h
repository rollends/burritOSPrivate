#ifndef PRINT_H
#define PRINT_H

#include "common/types.h"

/**
 * Prints a formated string
 *
 * @param   str     The format string 
 * @param   va_args Optional args that match the print string
 */
S32 printString(char const * str, ...);

#endif
