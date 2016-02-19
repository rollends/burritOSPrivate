#include "kernel/kernel.h"
#include "user/servers/ClockServer.h"
#include "user/servers/NameServer.h"
#include "user/servers/TerminalInputDriver.h"
#include "user/servers/TerminalOutputDriver.h"
#include "user/servers/TrainDriver.h"
#include "user/services/services.h"

void setupUserServices(void)
{
    sysCreate(0, &NameServer);
    sysCreate(2, &ClockServer);
    sysCreate(1, &TerminalInputDriver);
    sysCreate(1, &TerminalOutputDriver);
    sysCreate(2, &TrainDriver);
    sysCreate(4, &TrainSwitchServer);
    sysCreate(4, &TrainSensorServer);
}
