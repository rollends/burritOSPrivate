#ifndef SWITCH_OFFICE_H
#define SWITCH_OFFICE_H

typedef struct
{
    U32             startTime;
    U32             endTime;
    U16             indBranchNode;
    SwitchState     direction;
} SwitchRequest;

#endif
