#include "common/types.h"
#include "kernel/timer.h"
#include "kernel/ts7200.h"

#define         Timer_RWRegister(offset)        (RWRegister)(TIMER3_BASE + offset )
#define         Timer_RORegister(offset)        (RORegister)(TIMER3_BASE + offset )
#define         TicksPerTenthSecond             50800

void timerInit()
{
    RWRegister control = Timer_RWRegister( CRTL_OFFSET );
    RWRegister value = Timer_RWRegister( LDR_OFFSET );
    *control = 0xc8;
    *value = 508000 * 2;

    RWRegister vic = 0x800C0010;
    *vic |= 0x80000;
}

void timerStart(TimerState* state)
{
    RORegister rvalue = Timer_RORegister( VAL_OFFSET );
    state->start = *rvalue;
}

void timerClear()
{
    RWRegister value = Timer_RWRegister( CLR_OFFSET );
    *value = 1;
}

U32 timerValue()
{
    RORegister rvalue = Timer_RORegister( VAL_OFFSET );
    return *rvalue;
}

U32 timerEnd(TimerState const * state)
{
    RORegister rvalue = Timer_RORegister( VAL_OFFSET );
    U32 value = *rvalue;
    U32 result;

    if( value > state->start )
        result = state->start + (0xFFFFFFFF - value);
    else
        result = state->start - value;

    return result;
}
