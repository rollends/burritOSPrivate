#ifndef TIMER_H
#define TIMER_H

#include "common/common.h"

/**
 * Initializes a hardware timer
 *
 * @param   timer   The timer to initialize
 */
inline RETURN timerEnable(const U32 timer,
                          const U32 enable,
                          const U32 mode,
                          const U32 clk);

/**
 * Clears any active interrupts from the given timer
 *
 * @param   timer   The timer to clear
 */
inline RETURN timerClear(const U32 timer);

/**
 * Returns the value of a specific timer
 *
 * @param   timer   The timer to get the value from
 * @param   value   The value pointer to read into
 *
 * @return  0 on success, else an error code
 */
inline RETURN timerGetValue(const U32 timer, U32* value);

/**
 * Sets the value of a specific timer
 *
 * @param   timer   The timer value to set
 * @param   value   A 16 bit value for TIMER_1 and TIMER_2 else a 32 bit value
 */
inline RETURN timerSetValue(const U32 timer, const U32 value);

/**
 * Starts the timer and records the current value to a TimerState
 *
 * @param   state   The TimerState instance in which the state should be stored
 *
 * @return  The 32 bit timer value if using TIMER_3, else a 16 bit timer value
 */
RETURN timerStart(const U32 timer, TimerState* state);

/**
 * Returns the total duration since the last start or sample. Updates the 
 * timer state struct.
 * 
 * @param   state   The TimerState instance that has the starting state
 *
 * @return  The time delta between start and the current time
 */
RETURN timerSample(const U32 timer, TimerState* state);

#endif
