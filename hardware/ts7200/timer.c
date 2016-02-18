#include "hardware/memory.h"
#include "hardware/timer.h"
#include "hardware/ts7200/ts7200.h"

#ifdef DEVICE_CHECK
    #define IS_DEVICE(d) \
        do { if (d!=TIMER_1 && d!=TIMER_2 && d!=TIMER_3 && d!=TIMER_4) \
                return ERROR_DEVICE; } while(0)
#else
    #define IS_DEVICE(d)
#endif

RETURN timerEnable(const U32 timer,
                   const U32 enable,
                   const U32 mode,
                   const U32 clk)
{
    IS_DEVICE(timer);

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

    IS_OK();
}

RETURN timerClear(const U32 timer)
{
    IS_DEVICE(timer);

    if (timer != TIMER_4)
    {
        RWRegister value = (RWRegister)(timer + CLR_OFFSET);
        __str(value, 1);
    }

    IS_OK();
}

RETURN timerGetValue(const U32 timer, U32* value)
{
    IS_DEVICE(timer);
    IS_NOT_NULL(value);
    IS_IN_RANGE(value);
    
    RORegister rvalue = (RWRegister)(timer + VAL_OFFSET);
    *value = __ldr(rvalue);

    IS_OK();
}

RETURN timerSetValue(const U32 timer, const U32 value)
{
    IS_DEVICE(timer);

    if (timer != TIMER_4)
    {
        RWRegister load = (RWRegister)(timer + LDR_OFFSET);
        __str(load, value);
    }

    IS_OK();
}

RETURN timerStart(const U32 timer, TimerState* state)
{
    IS_DEVICE(timer);
    IS_NOT_NULL(state);
    IS_IN_RANGE(state);
    
    RORegister rvalue = (RWRegister)(timer + VAL_OFFSET);
    state->start = __ldr(rvalue);
    state->last = state->start;
    state->total = 0;
    
    IS_OK();
}

RETURN timerSample(const U32 timer, TimerState* state)
{
    IS_DEVICE(timer);
    IS_NOT_NULL(state);
    IS_IN_RANGE(state);

    RORegister rvalue = (RWRegister)(timer + VAL_OFFSET);
    U32 value = __ldr(rvalue);

    U32 delta;

    if (timer == TIMER_4)
    {
        if (value < state->last)
        {
            delta = value + (0xFFFFFFFF - state->last);
        }   
        else
        {
            delta = value - state->last;
        }
    }
    else if (value > state->last)
    {
        if (timer == TIMER_3)
        {
            delta = state->last + (0xFFFFFFFF - value);
        }
        else
        {
            delta = state->last + (0x0000FFFF - value);
        }
    }   
    else
    {
        delta = state->last - value;
    }

    state->last = value;
    state->delta = delta;
    state->total += delta;

    IS_OK();
}

#undef IS_DEVICE
