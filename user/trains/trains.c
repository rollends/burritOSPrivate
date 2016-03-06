#include "user/trains/trains.h"
#include "user/trains/SwitchOffice.h"
#include "user/trains/TrainYard.h"
#include "user/trains/TrainCommander.h"

void setupTrainServices(void)
{
    sysCreate(6, &SwitchExecutive);
    sysCreate(2, &TrainYardServer);
    sysCreate(5, &TrainCommanderServer);
}
