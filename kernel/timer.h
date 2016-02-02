#ifndef TIMER_H
#define TIMER_H

#include "common/types.h"
#include "kernel/ts7200.h"

#define TIMER_1 TIMER1_BASE
#define TIMER_2 TIMER2_BASE
#define TIMER_3 TIMER3_BASE

/**
 * Utility struct for storing timer state
 */
typedef struct
{
    /// The timer start time
    U32 start;
} TimerState;

/**
 * Initializes a hardware timer
 *
 * @param   timer   The timer to initialize
 *
 */
void timerInit(const U32 timer);

/**
 * Clears any active interrupts from the given timer
 *
 * @param   timer   The timer to clear
 *
 */
void timerClear(const U32 timer);

/**
 * Returns the value of a specific timer
 *
 * @param   timer   The timer to get the value from
 *
 * @return  A 32 bit value if using TIMER_3, else a 16 bit value
 */
U32 timerValue(const U32 timer);

/**
 * Starts the timer and records the current value to a TimerState
 *
 * @param   state   The TimerState instance in which the state should be stored
 *
 * @return  The 32 bit timer value if using TIMER_3, else a 16 bit timer value
 */
U32 timerStart(const U32 timer, TimerState* state);

/**
 * Ends the timer and returns the total duration
 * 
 * @param   state   The TimerState instance that has the starting state
 *
 * @return  The time delta between start and end
 */
U32 timerEnd(const U32 timer, TimerState const * state);

#endif
