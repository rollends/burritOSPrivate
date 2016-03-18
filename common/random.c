#include "common/random.h"

static const U32 a = 1103515245;
static const U32 c = 12345;
static const U32 m = 0x80000000;

U32 nextRandU32(U32* value)
{
    return *value = (a * (*value) + c) % m;
}
