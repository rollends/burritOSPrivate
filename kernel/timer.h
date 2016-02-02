#ifndef TIMER_H
#define TIMER_H

#include "common/types.h"

/**
 * Utility struct for storing timer state
 */
typedef struct
{
    /// The timer start time
    U32 start;
} TimerState;

/**
 * Initializes the 32 bit hardware timer
 */
void timerInit();

/**
 * Starts the timer and records the current value
 *
 * @param   state   The TimerState instance in which the state should be stored
 */
void timerStart(TimerState* state);

void timerClear();
U32 timerValue();

/**
 * Ends the timer and returns the total duration
 * 
 * @param   state   The TimerState instance that has the starting state
 *
 * @return  The time delta between start and end
 */
U32 timerEnd(TimerState const * state);

#endif
