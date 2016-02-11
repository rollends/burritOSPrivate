#include "hardware/memory.h"
#include "hardware/timer.h"
#include "hardware/ts7200/ts7200.h"

S32 timerEnable(const U32 timer,
                const U32 enable,
                const U32 mode,
                const U32 clk)
{
    RWRegister control = (RWRegister)(timer + CRTL_OFFSET);
    U32 value = __ldr(control);

    if (timer != TIMER_4)
    {
        value = (enable ? value | ENABLE_MASK : value & ~ENABLE_MASK);
        value = (mode ? value | MODE_MASK : value & ~MODE_MASK);
        value = (clk ? value | CLKSEL_MASK : value & ~CLKSEL_MASK);
    }
    else
    {
        value = (enable ? enable | DEBUG_MASK : value & ~DEBUG_MASK);
    }

    __str(control, value);

    return 0;
}

S32 timerClear(const U32 timer)
{
    if (timer != TIMER_4)
    {
        RWRegister value = (RWRegister)(timer + CLR_OFFSET);
        __str(value, 1);
    }

    return 0;
}

U32 timerGetValue(const U32 timer)
{
    RORegister rvalue = (RWRegister)(timer + VAL_OFFSET);
    return *rvalue;
}

S32 timerSetValue(const U32 timer, const U32 value)
{
    if (timer != TIMER_4)
    {
        RWRegister load = (RWRegister)(timer + LDR_OFFSET);
        __str(load, value);
    }

    return 0;
}

U32 timerStart(const U32 timer, TimerState* state)
{
    state->start = timerGetValue(timer);
    state->last = state->start;
    state->total = 0;
    
    return state->last;
}

U32 timerSample(const U32 timer, TimerState * state)
{
    U32 value = timerGetValue(timer);
    U32 result;

    if (timer == TIMER_4)
    {
        if (value < state->last)
        {
            result = value + (0xFFFFFFFF - state->last);
        }   
        else
        {
            result = value - state->last;
        }
    }
    else if (value > state->last)
    {
        if (timer == TIMER_3)
        {
            result = state->last + (0xFFFFFFFF - value);
        }
        else
        {
            result = state->last + (0x0000FFFF - value);
        }
    }   
    else
    {
        result = state->last - value;
    }

    state->last = value;
    state->total += result;

    return result;
}
