#ifndef TRAIN_SERVICES_H
#define TRAIN_SERVICES_H

#include "common/common.h"
#include "kernel/kernel.h"
#include "user/services/trainCommand.h"

typedef struct
{
    U32             startTime;
    U32             endTime;
    U16             indBranchNode;
    U16             branchId;
    SwitchState     direction;
} SwitchRequest;

S32 pushTrainCommand(String string);
S32 pollTrainCommand(U8 trainId, MessageEnvelope* env);
void trainRegister(U8 trainId);
S32 trainWhoIs(U8 trainId, TaskID* tid);

#endif
