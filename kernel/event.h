#ifndef EVENT_H
#define EVENT_H

/// Unknown or null event type
#define EVENT_NULL              0

/// 10 millisecond timer event
#define EVENT_10MS_TICK         1

/// 100 millisecond timer event
#define EVENT_100MS_TICK        2

/// UART 2 input event
#define EVENT_TERMINAL_READ     3

/// UART 2 output event
#define EVENT_TERMINAL_WRITE    4

/// UART 1 input event
#define EVENT_TRAIN_READ        5

/// UART 1 output event
#define EVENT_TRAIN_WRITE       6

/// Event table size 
#define EVENT_TABLE_SIZE        7


/// The terminal IO port 
#define PORT_TERMINAL           0

/// The train IO port
#define PORT_TRAIN              1

#endif
