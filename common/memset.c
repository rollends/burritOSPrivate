#include "common/memset.h"

void* memset(void* memory, S32 value, U32 bytes)
{
    volatile U8 *c = (U8*)memory;
    while(bytes--)
        *(c++) = value;
    return memory;
}

