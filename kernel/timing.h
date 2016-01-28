#ifndef TIMING_H
#define TIMING_H

#include "common/types.h"

typedef struct
{
    U32 start;
} TimerState;

void timerInit();
void timerStart( TimerState * );
U32 timerEnd( TimerState const * );
#endif