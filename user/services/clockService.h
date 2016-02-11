#ifndef CLOCK_SERVICE_H
#define CLOCK_SERVICE_H

#include "kernel/kernel.h"

void clockDelayBy( TaskID clock, U32 );
U32 clockTime( TaskID clock );
void clockDelayUntil( TaskID clock, U32 );

#endif
