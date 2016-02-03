#ifndef HARDWARE_H
#define HARDWARE_H

#include "hardware/interrupt.h"
#include "hardware/memory.h"
#include "hardware/timer.h"
#include "hardware/uart.h"

#ifdef TARGET_TS7200
    #include "hardware/ts7200/ts7200.h"
#elif TARGET_PI
    #include "hardware/pi/pi.h"
#else
#endif

#endif
