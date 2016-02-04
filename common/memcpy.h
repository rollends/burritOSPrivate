#ifndef MEMCPY_H
#define MEMCPY_H

#include "common/types.h"

/**
 * Copies words of memory from src to dst
 *
 * @param   dst     The memory address to copy to
 * @param   src     The memory to copy from
 * @param   len     The number of words to copy
 */
extern void memcpy(U32* dst, U32* src, U32 len);

#endif
