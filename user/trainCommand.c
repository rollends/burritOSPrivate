#include "common/string.h"
#include "user/trainCommand.h"
#include "user/TrainDriver.h"

S32 dispatchTrainCommand(TaskID server, String string)
{
    U32 len = strlen(string);

  if(len <= 1) { return -1; }
    
    strtolower(string);
    ConstString cstring = (ConstString)string;


    if( cstring[0] == 't' && cstring[1] == 'r' )
    {
        cstring += 2;
        strskipws(&cstring);
        U8 train = stratoui(&cstring);
        strskipws(&cstring);
        U8 speed = stratoui(&cstring);
        
        trainSetSpeed(server, train, speed);
    }
    else if( cstring[0] == 'r' && cstring[1] == 'v' )
    {
        cstring += 2;
        strskipws(&cstring);
        U8 train = stratoui(&cstring);
        trainReverseDirection(server, train, 0);
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
        trainSwitch(server, switchId, sw);
    }
    else
    {
        return -1;
    }
    return 0;
}
