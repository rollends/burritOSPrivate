#include "kernel/kernel.h"
#include "user/trainservers/trainservers.h"
#include "user/trainservers/SwitchOffice.h"
#include "user/trainservers/TrackManager.h"
#include "user/trainservers/TrainCommander.h"
#include "user/trainservers/TrainYard.h"

void setupTrainServices(void)
{
    sysCreate(6, &SwitchExecutive);
    sysCreate(2, &TrainYardServer);
    sysCreate(5, &TrainCommanderServer);
    sysCreate(5, &TrackManagerServer);
}
