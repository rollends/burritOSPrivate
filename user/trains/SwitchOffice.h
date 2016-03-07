#ifndef SWITCH_OFFICE_H
#define SWITCH_OFFICE_H

#include "common/common.h"
#include "user/services/trainCommand.h"

typedef struct
{
    U32             startTime;
    U32             endTime;
    U16             indBranchNode;
    U16             branchId;
    SwitchState     direction;
} SwitchRequest;

void SwitchExecutive(void);

#endif
