#ifndef EVENT_H
#define EVENT_H

#include "kernel/taskDescriptor.h"

/// Unknown or null event type
#define EVENT_NULL              0

/// 10 millisecond timer event
#define EVENT_10MS_TICK         1

/// 100 millisecond timer event
#define EVENT_100MS_TICK        2

/// UART 2 input event
#define EVENT_TERMINAL_READ     3

/// Event table size 
#define EVENT_TABLE_SIZE        4

#endif
