#ifndef RANDOM_H
#define RANDOM_H

#include "common/types.h"

/**
 * Generates a pseudo andom byte
 *
 * @param   value   A pointer to the previous byte / seed for the next byte
 *
 * @return  a pseudo random byte
 */
U8 nextRandU8(U8* value);

#endif