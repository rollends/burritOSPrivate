#ifndef UTILS_H
#define UTILS_H

/// Borrowed from Standford's Bit Twiddling hacks
static inline U32 __ctz(U32 v)
{
    U32 c = 32; // c will be the number of zero bits on the right
    v &= -((S32)(v));
    if (v) c--;
    if (v & 0x0000FFFF) c -= 16;
    if (v & 0x00FF00FF) c -= 8;
    if (v & 0x0F0F0F0F) c -= 4;
    if (v & 0x33333333) c -= 2;
    if (v & 0x55555555) c -= 1;
    return c;
}

#define STRINGIFY(s) #s

#endif
