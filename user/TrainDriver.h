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
void trainSwitch(TaskID server, U8 address, SwitchState state);
void trainSolenoidOff(TaskID train);
void trainReverseDirection(TaskID server, U8 train, U8 newSpeed);
void trainSetSpeed(TaskID server, U8 train, U8 speed);

void TrainDriver(void);

#endif
