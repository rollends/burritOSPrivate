#include "common/types.h"

#include "hardware/memory.h"
#include "hardware/timer.h"
#include "hardware/ts7200/ts7200.h"

void timerInit(const U32 timer)
{
    RWRegister control = (RWRegister)(timer + CRTL_OFFSET);
    __str(control, 0xc8);
}

void timerClear(const U32 timer)
{
    RWRegister value = (RWRegister)(timer + CLR_OFFSET);
    __str(value, 1);
}

U32 timerGetValue(const U32 timer)
{
    RORegister rvalue = (RWRegister)(timer + VAL_OFFSET);
    return *rvalue;
}

void timerSetValue(const U32 timer, const U32 value)
{
    RWRegister load = (RWRegister)(timer + LDR_OFFSET);
    __str(load, value);
}

U32 timerStart(const U32 timer, TimerState* state)
{
    RORegister rvalue = (RWRegister)(timer + VAL_OFFSET);
    state->start = *rvalue;

    return state->start;
}

U32 timerEnd(const U32 timer, TimerState const * state)
{
    RORegister rvalue = (RWRegister)(timer + VAL_OFFSET);
    U32 value = *rvalue;
    U32 result;

    if (value > state->start)
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

    return result;
}
