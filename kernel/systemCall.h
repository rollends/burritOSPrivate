#ifndef SYS_CALL_H
#define SYS_CALL_H

#include "common/common.h"
#include "kernel/kernelUtils.h"

#define SYS_CALL_EXIT_ID            0
#define SYS_CALL_PASS_ID            1
#define SYS_CALL_RUNNING_ID         2
#define SYS_CALL_SHUTDOWN_ID        3
#define SYS_CALL_CREATE_ID          4
#define SYS_CALL_TID_ID             5
#define SYS_CALL_PID_ID             6
#define SYS_CALL_SEND_ID            7
#define SYS_CALL_RECEIVE_ID         8
#define SYS_CALL_REPLY_ID           9
#define SYS_CALL_PERF_RESET_ID      10
#define SYS_CALL_PERF_QUERYP_ID     11
#define SYS_CALL_PERF_QUERYT_ID     12
#define SYS_CALL_AWAIT_ID           13
#define SYS_CALL_READ_ID            14
#define SYS_CALL_WRITE_ID           15

#define sysExit() \
            __sysCall0((U32)SYS_CALL_EXIT_ID)
#define sysPass() \
            __sysCall0((U32)SYS_CALL_PASS_ID)
#define sysRunning() \
            __sysCall0((U32)SYS_CALL_RUNNING_ID)
#define sysShutdown() \
            __sysCall0((U32)SYS_CALL_SHUTDOWN_ID)
#define sysCreate(pri, entry, size) \
            __sysCall3((U32)SYS_CALL_CREATE_ID, (U32)pri, (U32)entry, (U32)size)
#define sysTid() \
            __sysCall0((U32)SYS_CALL_TID_ID)
#define sysPid() \
            __sysCall0((U32)SYS_CALL_PID_ID)
#define sysSend(tid, pmsg, preply) \
            __sysCall3((U32)SYS_CALL_SEND_ID, (U32)tid, (U32)pmsg, (U32)preply)
#define sysReceive(ptid, pmsg) \
            __sysCall2((U32)SYS_CALL_RECEIVE_ID, (U32)ptid, (U32)pmsg)
#define sysReply(tid, preply) \
            __sysCall2((U32)SYS_CALL_REPLY_ID, (U32)tid, (U32)preply)
#define sysPerfReset() \
            __sysCall0((U32)SYS_CALL_PERF_RESET_ID)
#define sysPerfQueryP(tid, mode) \
            __sysCall2((U32)SYS_CALL_PERF_QUERYP_ID, (U32)tid, (U32)mode)
#define sysPerfQueryT(tid, mode) \
            __sysCall2((U32)SYS_CALL_PERF_QUERYT_ID, (U32)tid, (U32)mode)
#define sysAwaitEvent(event) \
            __sysCall1((U32)SYS_CALL_AWAIT_ID, (U32)event)
#define sysRead() \
            __sysCall0((U32)SYS_CALL_READ_ID)
#define sysWrite(byte) \
            __sysCall1((U32)SYS_CALL_WRITE_ID, (U32)byte)

#endif
