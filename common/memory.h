#ifndef MEMORY_H
#define MEMORY_H

#include "common/types.h"

static inline void __str(RWRegister dst, const U32 src)
{
    asm volatile("str %0, [%1]" : : "r" (src), "r" (dst) : "memory");
}

static inline void __strb(RWRegister dst, const U8 src)
{
    asm volatile("strb %0, [%1]" : : "r" (src), "r" (dst) : "memory");
}

static inline U32 __ldr(U32 src)
{
    asm volatile("ldr %0, [%1]" : "=r" (src) :  "r" (src) : "memory");
    return src;
}

#endif
