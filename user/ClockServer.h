#ifndef CLOCK_SERVER_H
#define CLOCK_SERVER_H

#include "common/types.h"
#include "kernel/taskDescriptor.h"

void DelayBy( TaskID clock, U8 );
void ClockServer();

#endif
