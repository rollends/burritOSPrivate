#ifndef NAMESERVER_H
#define NAMESERVER_H

#include "common/types.h"
#include "kernel/taskDescriptor.h"

typedef enum
{
    God             = 0,
    RPS             = 1,
    TaskNameCount   = 2,
} TaskName;


S32 nsRegister( TaskName );
S32 nsWhoIs( TaskName, TaskID* sid );

void Nameserver();

#endif
