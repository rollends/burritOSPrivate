#ifndef MEMORY_H
#define MEMORY_H

#include "common/common.h"

/**
 * Copies words of memory from src to dst
 *
 * @param   dst     The memory address to copy to
 * @param   src     The memory to copy from
 * @param   len     The number of words to copy
 */
extern void __memcpy(U32* dst, U32* src, U32 len);

/**
 * Stores a U32 value to a memory location
 *
 * @param   dst     The memory address to write to
 * @param   src     The value to write
 *
 * @return  no return value
 */
static inline void __str(RWRegister dst, const U32 src)
{
    asm volatile("str %0, [%1]" : : "r" (src), "r" (dst) : "memory");
}

/**
 * Stores a U8 value to a memory location
 *
 * @param   dst     The memory address to write to
 * @param   src     The value to write
 *
 * @return  no return value
 */
static inline void __strb(RWRegister dst, const U8 src)
{
    asm volatile("strb %0, [%1]" : : "r" (src), "r" (dst) : "memory");
}

/**
 * Loads a U32 value from a memory location
 *
 * @param   src     The memory address to read from
 *
 * @return  The value at the memory address
 */
static inline U32 __ldr(RORegister src)
{
    U32 result;
    asm volatile("ldr %0, [%1]" : "=r" (result) :  "r" (src) : "memory");
    return result;
}

/**
 * Loads a U8 value from a memory location
 *
 * @param   src     The memory address to read from
 *
 * @return  The value at the memory address
 */
static inline U8 __ldrb(RORegister src)
{
    U8 result;
    asm volatile("ldrb %0, [%1]" : "=r" (result) :  "r" (src) : "memory");
    return result;
}

#endif
