#ifndef TIMER_STATE_H
#define TIMER_STATE_H

#include "common/types.h"

/**
 * Utility struct for storing timer state, used for computing time deltas
 */
typedef struct
{
    /// The timer start time
    U32 start;
} TimerState;

#endif
