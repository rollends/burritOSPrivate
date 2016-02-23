#include "kernel/kernel.h"
#include "user/servers/ClockServer.h"
#include "user/servers/NameServer.h"
#include "user/servers/TerminalDriver.h"
#include "user/servers/TrainDriver.h"
#include "user/services/services.h"

void setupUserServices(void)
{
    sysCreate(0, &NameServer);
    sysCreate(2, &ClockServer);
    sysCreate(1, &TerminalDriver);
    sysCreate(2, &TrainDriver);
    sysCreate(4, &TrainSwitchServer);
    sysCreate(4, &TrainSensorServer);
}
