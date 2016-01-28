#include "kernel/message.h"
#include "kernel/print.h"
#include "kernel/sysCall.h"
#include "user/TestTask.h"

void TestTask()
{
    printString("Tid: %b\tPid: %b\r\n", sysTid(), sysPid());
    sysPass();
    printString("Tid: %b\tPid: %b\r\n", sysTid(), sysPid());

    MessageEnvelope env;
    env.message.NameserverRequest.name = sysTid();
    env.type = MESSAGE_NAMESERVER_REGISTER;

    sysSend(sysPid(), &env, &env);
    printString("Reply: %x\r\n", env.message.NameserverRequest.name);
    sysExit();
}
