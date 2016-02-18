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
    sysCreate(1, &ClockServer);
    sysCreate(3, &TerminalInputDriver);
    sysCreate(2, &TerminalOutputDriver);
    sysCreate(1, &TrainDriver);
    sysCreate(2, &TrainSwitchServer);
    sysCreate(2, &TrainSensorServer);
}
