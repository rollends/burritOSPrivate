#ifndef TRAIN_YARD_H
#define TRAIN_YARD_H

#include "kernel/kernel.h"
#include "common/common.h"

void trainRegister(U8 trainId);
S32 trainWhoIs(U8 trainId, TaskID* tid);

void TrainYardServer(void);

#endif
