#ifndef SYS_CALL_H
#define SYS_CALL_H

#include "common/common.h"
#include "kernel/kernelUtils.h"

#define SYS_CALL_EXIT_ID            0
#define SYS_CALL_PASS_ID            1
#define SYS_CALL_RUNNING_ID         2
#define SYS_CALL_SHUTDOWN_ID        3
#define SYS_CALL_KILL_ID            4

#define SYS_CALL_CREATE_ID          5
#define SYS_CALL_NAME_ID            6
#define SYS_CALL_TID_ID             7
#define SYS_CALL_PID_ID             8
#define SYS_CALL_PRIORITY_ID        9

#define SYS_CALL_SEND_ID            10
#define SYS_CALL_RECEIVE_ID         11
#define SYS_CALL_REPLY_ID           12

#ifdef KERNEL_PERF
      #define SYS_CALL_PERF_RESET_ID      13
      #define SYS_CALL_PERF_QUERYP_ID     14
      #define SYS_CALL_PERF_QUERYT_ID     15
      #define SYS_CALL_PERF_COUNT_ID      16
#endif

#define SYS_CALL_AWAIT_ID           17
#define SYS_CALL_READ_ID            18
#define SYS_CALL_WRITE_ID           19

#define SYS_CALL_ALLOC_ID           20
#define SYS_CALL_FREE_ID            21

#define sysExit() \
            __sysCall0((U32)SYS_CALL_EXIT_ID)
#define sysPass() \
            __sysCall0((U32)SYS_CALL_PASS_ID)
#define sysRunning() \
            __sysCall0((U32)SYS_CALL_RUNNING_ID)
#define sysShutdown() \
            __sysCall0((U32)SYS_CALL_SHUTDOWN_ID)
#define sysKill() \
            __sysCall0((U32)SYS_CALL_KILL_ID)

#define sysCreate(pri, entry) \
            __sysCall3((U32)SYS_CALL_CREATE_ID, (U32)(pri), (U32)(entry), (U32)(STRINGIFY(entry)))
#define sysName(tid) \
            __sysCall1((U32)SYS_CALL_NAME_ID, (U32)(tid))
#define sysTid() \
            __sysCall0((U32)SYS_CALL_TID_ID)
#define sysPid() \
            __sysCall0((U32)SYS_CALL_PID_ID)
#define sysPriority() \
            __sysCall0((U32)SYS_CALL_PRIORITY_ID)
#define sysSend(tid, pmsg, preply) \
            __sysCall3((U32)SYS_CALL_SEND_ID, (U32)(tid), (U32)(pmsg), (U32)(preply))
#define sysReceive(ptid, pmsg) \
            __sysCall2((U32)SYS_CALL_RECEIVE_ID, (U32)(ptid), (U32)(pmsg))
#define sysReply(tid, preply) \
            __sysCall2((U32)SYS_CALL_REPLY_ID, (U32)(tid), (U32)(preply))

#ifdef KERNEL_PERF
      #define sysPerfReset() \
                  __sysCall0((U32)SYS_CALL_PERF_RESET_ID)
      #define sysPerfQueryP(tid, mode) \
                  __sysCall2((U32)SYS_CALL_PERF_QUERYP_ID, (U32)(tid), (U32)(mode))
      #define sysPerfQueryT(tid, mode) \
                  __sysCall2((U32)SYS_CALL_PERF_QUERYT_ID, (U32)(tid), (U32)(mode))
      #define sysPerfCount() \
                  __sysCall0((U32)SYS_CALL_PERF_COUNT_ID)
#endif
                  
#define sysAwait(event) \
            __sysCall1((U32)SYS_CALL_AWAIT_ID, (U32)(event))
#define sysRead(port) \
            __sysCall1((U32)SYS_CALL_READ_ID, (U32)(port))
#define sysWrite(port, byte) \
            __sysCall2((U32)SYS_CALL_WRITE_ID, (U32)(port), (U32)(byte))
#define sysAlloc() \
            __sysCall0((U32)SYS_CALL_ALLOC_ID)
#define sysFree(block) \
            __sysCall1((U32)SYS_CALL_ALLOC_ID, (U32)(block))

#endif
