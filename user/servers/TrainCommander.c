#include "user/messageTypes.h"
#include "user/servers/TrainCommander.h"
#include "user/services/nameService.h"

void TrainCommanderServer(void)
{

    nsRegister(TrainCommander);

    for(;;)
    {
        TaskID from;
        MessageEnvelope env;
        sysReceive(
    }
}

S32 dispatchTrainCommand(String string)
{
    U32 len = strlen(string);

  if(len <= 1) { return -1; }
    
    strtolower(string);
    ConstString cstring = (ConstString)string;

    TaskID  sTrain      = nsWhoIs(Train),
            sSwitch     = nsWhoIs(TrainSwitches),
            sSwitchOffice = nsWhoIs(TrainSwitchOffice),
            clock       = nsWhoIs(Clock);

    if( cstring[0] == 't' && cstring[1] == 'r' )
    {
        cstring += 2;
        strskipws(&cstring);
        U8 train = stratoui(&cstring);
        strskipws(&cstring);
        U8 speed = stratoui(&cstring);
        
        trainSetSpeed(sTrain, train, speed);
    }
    else if( cstring[0] == 'r' && cstring[1] == 'v' )
    {
        cstring += 2;
        strskipws(&cstring);
        U8 train = stratoui(&cstring);
        trainReverseDirection(sTrain, train);
    }
    else if( cstring[0] == 's' && cstring[1] == 'w' )
    {
        cstring += 2;
        strskipws(&cstring);
        U8 switchId = stratoui(&cstring);
        strskipws(&cstring);
        SwitchState sw = eStraight;
        switch(*cstring)
        {
        case 'c':
            sw = eCurved;
        case 's':
            break;

        default:
            return -1;
        }
        trainSwitch(sSwitch, switchId, sw);
    }
    else if( cstring[0] == 's' && cstring[1] == 'd' )
    {
        cstring += 2;
        strskipws(&cstring);
        U8 switchId = stratoui(&cstring);
        strskipws(&cstring);
        SwitchState sw = eStraight;
        switch(*cstring++)
        {
        case 'c':
            sw = eCurved;
        case 's':
            break;

        default:
            return -1;
        }
        strskipws(&cstring);
        U32 timeSeconds = stratoui(&cstring);
 
        SwitchRequest request;
        request.startTime = clockTime(clock) + timeSeconds * 100;
        request.endTime = request.startTime + 300;
        request.direction = sw;
        request.branchId = switchId;
        request.indBranchNode = 79 + (2 * switchId - 1); 
        MessageEnvelope env;
        env.type = MESSAGE_SWITCH_ALLOCATE;
        env.message.MessageArbitrary.body = (U32*)&request;
        sysSend(sSwitchOffice.value, &env, &env);
    }
    else
    {
        return -1;
    }
    return 0;
}
