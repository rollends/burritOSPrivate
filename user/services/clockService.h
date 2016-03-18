#ifndef CLOCK_SERVICE_H
#define CLOCK_SERVICE_H

#include "kernel/kernel.h"

void clockLongDelayBy( TaskID clockId, S32 mult150ms );
void clockDelayBy( TaskID clock, S32 mult10ms );
U32 clockTime( TaskID clock );
void clockDelayUntil( TaskID clock, U32 );

#endif
