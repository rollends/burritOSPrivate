#ifndef ASSERT_H
#define ASSERT_H

#include "common/common.h"

#ifdef ASSERT_BUILD
    #define assert(expr) do {if (!expr){assertHandler(__LINE__, __FILE__);}} while(0)
    #define assertOk(exp) assert(exp == OK)
#else
    #define assert(expr) expr
    #define assertOk(expr) expr
#endif

/**
 * Handles an assertion by printing, hanging and reseting the board
 *
 * @param   line    The line number of the assertion occured at
 * @param   file    The file that the error occured in
 */
void assertHandler(const U32 line, const char* file);

#endif
