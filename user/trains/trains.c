#include "user/trains/trains.h"
#include "user/trains/SwitchOffice.h"
#include "user/trains/TrainYard.h"


void setupTrainServices(void)
{
    sysCreate(6, &SwitchExecutive);
    sysCreate(2, &TrainYardServer);
}
