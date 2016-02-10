#ifndef TRAIN_DRIVER_H
#define TRAIN_DRIVER_H

#include "kernel/kernel.h"

typedef enum
{
    eStraight   = 33,
    eCurved     = 34, 
} SwitchState;

void trainStop(TaskID server);
void trainGo(TaskID server);
void trainSwitch(TaskID server, SwitchState, U8 address);
void trainSolenoidOff(TaskID train);

void TrainDriver(void);

#endif
