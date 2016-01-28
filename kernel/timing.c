#include "kernel/ts7200.h"
#include "common/types.h"
#include "kernel/timing.h"

#define         Timer_RWRegister(offset)        (RWRegister)(TIMER3_BASE + offset )
#define         Timer_RORegister(offset)        (RORegister)(TIMER3_BASE + offset )
#define         TicksPerTenthSecond             50800

void timerInit( )
{
    RWRegister control = Timer_RWRegister( CRTL_OFFSET );
    *control = 0x88; // Enable the timer in free-running mode.
}

void timerStart( TimerState * state )
{
    RORegister rvalue = Timer_RORegister( VAL_OFFSET );
    state->start = *rvalue;
}

U32 timerEnd( TimerState const * t )
{
    RORegister rvalue = Timer_RORegister( VAL_OFFSET );
    U32 value = *rvalue;
    U32 result;

    if( value > t->start )
        result = t->start + (0xFFFFFFFF - value);
    else
        result = t->start - value;

    return result;
}