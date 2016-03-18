#ifndef RANDOM_H
#define RANDOM_H

#include "common/types.h"

/**
 * Generates a pseudo random byte
 * Note that the the 'previous value' passed in is modified and will be equal
 * to the return value.
 *
 * @param   value   A pointer to the previous byte / seed for the next byte
 *
 * @return  a pseudo random byte
 */
U32 nextRandU32(U32* value);

#endif
