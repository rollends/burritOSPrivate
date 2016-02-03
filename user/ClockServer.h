#ifndef CLOCK_SERVER_H
#define CLOCK_SERVER_H

#include "common/types.h"
#include "kernel/taskDescriptor.h"

void clockDelayBy( TaskID clock, U32 );
U32 clockTime( TaskID clock );
void clockDelayUntil( TaskID clock, U32 );
void ClockServer();

#endif
