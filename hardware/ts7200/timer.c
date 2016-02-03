#include "common/types.h"

#include "hardware/memory.h"
#include "hardware/timer.h"
#include "hardware/ts7200/ts7200.h"

void timerInit(const U32 timer)
{
    RWRegister control = (RWRegister)(timer + CRTL_OFFSET);
    if (timer != TIMER_4)
    {
        __str(control, 0xc8);
    }
    else
    {
        U32 controlValue = __ldr(control);
        controlValue |= DEBUG_MASK;
        __str(control, controlValue);
    }
}

void timerClear(const U32 timer)
{
    if (timer != TIMER_4)
    {
        RWRegister value = (RWRegister)(timer + CLR_OFFSET);
        __str(value, 1);
    }
}

U32 timerGetValue(const U32 timer)
{
    RORegister rvalue = (RWRegister)(timer + VAL_OFFSET);
    return *rvalue;
}

void timerSetValue(const U32 timer, const U32 value)
{
    if (timer != TIMER_4)
    {
        RWRegister load = (RWRegister)(timer + LDR_OFFSET);
        __str(load, value);
    }
}

U32 timerStart(const U32 timer, TimerState* state)
{
    state->start = timerGetValue(timer);
    return state->start;
}

U32 timerSample(const U32 timer, TimerState * state)
{
    U32 value = timerGetValue(timer);
    U32 result;

    if (timer == TIMER_4)
    {
        if (value < state->start)
        {
            result = value + (0xFFFFFFFF - state->start);
        }   
        else
        {
            result = value - state->start;
        }
    }
    else if (value > state->start)
    {
        if (timer == TIMER_3)
        {
            result = state->start + (0xFFFFFFFF - value);
        }
        else
        {
            result = state->start + (0x0000FFFF - value);
        }
    }   
    else
    {
        result = state->start - value;
    }

    state->start = value;

    return result;
}
