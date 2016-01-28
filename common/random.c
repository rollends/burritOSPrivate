#include "common/random.h"

U8 nextRandU8(U8* value)
{
    return *value = (133 * (*value) + 67) & (0xFF);
}