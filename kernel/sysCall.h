#ifndef SYS_CALL_H
#define SYS_CALL_H

#include "common/types.h"

#define SYS_CALL_EXIT_ID    0
#define SYS_CALL_PASS_ID    1
#define SYS_CALL_CREATE_ID  2
#define SYS_CALL_TID_ID     3
#define SYS_CALL_PID_ID     4

extern U32 __sysCall3(U32, U32, U32, U32);
extern U32 __sysCall2(U32, U32, U32);
extern U32 __sysCall1(U32, U32);
extern U32 __sysCall0(U32);

#define sysExit()           __sysCall0((U32)SYS_CALL_EXIT_ID)
#define sysPass()           __sysCall0((U32)SYS_CALL_PASS_ID)
#define sysCreate(a, b)     __sysCall2((U32)SYS_CALL_CREATE_ID, (U32)a, (U32)b)
#define sysTid()            __sysCall0((U32)SYS_CALL_TID_ID)
#define sysPid()            __sysCall0((U32)SYS_CALL_PID_ID)

#endif
