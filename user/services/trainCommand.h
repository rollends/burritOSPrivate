#ifndef TRAIN_COMMAND_H
#define TRAIN_COMMAND_H

#include "common/types.h"
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
void trainReverseDirection(TaskID server, U8 train);
void trainSetSpeed(TaskID server, U8 train, U8 speed);
U8 trainReadSensorGroup(TaskID sensorServer, U8 sensorGroup);

S32 dispatchTrainCommand(String string);

#endif
