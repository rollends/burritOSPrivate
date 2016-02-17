#include "kernel/kernel.h"

#include "user/messageTypes.h"
#include "user/services/services.h"
#include "user/servers/TrainDriver.h"

static U8 getTrainDriverMessageType(U16 inMsgType)
{
    if( (inMsgType & 0x00FF) <= 0x0F ) return DRIVER_MESSAGE_TX_TRAIN_MULTI_BYTE;

    switch(inMsgType & 0x00FF)
    {
    case MESSAGE_TRAIN_SWITCH_STRAIGHT:
    case MESSAGE_TRAIN_SWITCH_CURVED:
    case MESSAGE_TRAIN_SET_SPEED:
    case MESSAGE_TRAIN_REVERSE:
    case MESSAGE_TRAIN_GET_SENSOR:
        return DRIVER_MESSAGE_TX_TRAIN_MULTI_BYTE;
    
    case MESSAGE_TRAIN_GET_SENSOR_A:
    case MESSAGE_TRAIN_GET_SENSOR_B:
    case MESSAGE_TRAIN_GET_SENSOR_C:
    case MESSAGE_TRAIN_GET_SENSOR_D:
    case MESSAGE_TRAIN_GET_SENSOR_E:
    case MESSAGE_TRAIN_GET_SENSOR_F:
    case MESSAGE_TRAIN_GO:
    case MESSAGE_TRAIN_STOP:
    case MESSAGE_TRAIN_SOLENOID_OFF:
        return DRIVER_MESSAGE_TX_TRAIN;
    
    default:
        break;
    }
    assert( 0 );
    return 0;
}

void TrainInputNotifier(void);
void TrainOutputNotifier(void);

void TrainDriver(void)
{
    nsRegister(Train);
    
    TaskID output,input;

    input.value = sysCreate(0, &TrainInputNotifier);
    output.value = sysCreate(0, &TrainOutputNotifier);

    MessageEnvelope rcvMessage, commandMsg;
    TaskID rcvID;
  
    U16 blockedTaskCommandsBacking[16];
    U16 blockedTasksBacking[16];
    U8 blockedTaskOriginalTypeBacking[16];
    QueueU16 blockedTaskCommands;
    QueueU16 blockedTasks;
    QueueU8 blockedTaskOriginalType;
    queueU16Init(&blockedTaskCommands, blockedTaskCommandsBacking, 16);
    queueU16Init(&blockedTasks, blockedTasksBacking, 16);
    queueU8Init(&blockedTaskOriginalType, blockedTaskOriginalTypeBacking, 16);

    U8 isOutputWaiting = 0;

    U16 activeSensorRequestId = 0;
    while( sysRunning() != 0 )
    {
        sysReceive(&rcvID.value, &rcvMessage);

        U8 oldType = rcvMessage.type;
        
        // Some commands need priming. That is, the multi-byte commands and the 
        // sensor request need some extra information.
        switch( rcvMessage.type )
        {
        case MESSAGE_TRAIN_GET_SENSOR_A:
        case MESSAGE_TRAIN_GET_SENSOR_B:
        case MESSAGE_TRAIN_GET_SENSOR_C:
        case MESSAGE_TRAIN_GET_SENSOR_D:
        case MESSAGE_TRAIN_GET_SENSOR_E:
        case MESSAGE_TRAIN_GET_SENSOR_F:
        {
            assert(activeSensorRequestId == 0);
            activeSensorRequestId = rcvID.value;
            break;
        }
        case MESSAGE_TRAIN_REVERSE:
        case MESSAGE_TRAIN_SET_SPEED:
        {
            rcvMessage.type = rcvMessage.message.MessageU16.body & 0xFF;
            rcvMessage.message.MessageU8.body = rcvMessage.message.MessageU16.body >> 8;
            break;
        }
        }

        // Actual Message Processing.
        switch(oldType)
        {
        case MESSAGE_TRAIN_GET_SENSOR_A:
        case MESSAGE_TRAIN_GET_SENSOR_B:
        case MESSAGE_TRAIN_GET_SENSOR_C:
        case MESSAGE_TRAIN_GET_SENSOR_D:
        case MESSAGE_TRAIN_GET_SENSOR_E:
        case MESSAGE_TRAIN_GET_SENSOR_F:
        case MESSAGE_TRAIN_REVERSE:
        case MESSAGE_TRAIN_SET_SPEED: 
        case MESSAGE_TRAIN_SWITCH_STRAIGHT:
        case MESSAGE_TRAIN_SWITCH_CURVED:
        case MESSAGE_TRAIN_GO:
        case MESSAGE_TRAIN_STOP:
        case MESSAGE_TRAIN_SOLENOID_OFF:
        {
            U16 fullCmd = (rcvMessage.message.MessageU8.body << 8) | rcvMessage.type;
            if( isOutputWaiting )
            {
                commandMsg.type = getTrainDriverMessageType(fullCmd);
                commandMsg.message.MessageU16.body = fullCmd;
                
                sysReply(output.value, &commandMsg);
                if( oldType < MESSAGE_TRAIN_GET_SENSOR )
                {
                    sysReply(rcvID.value, &rcvMessage);
                }
                isOutputWaiting = 0;
            } 
            else
            {
                queueU16Push(&blockedTaskCommands, fullCmd);
                queueU16Push(&blockedTasks, rcvID.value);
                queueU8Push(&blockedTaskOriginalType, oldType);
            }
            break;
        } 

        case DRIVER_MESSAGE_TX_TRAIN:
        case DRIVER_MESSAGE_TX_TRAIN_MULTI_BYTE:
        {
            if( queueU16Pop(&blockedTaskCommands, &commandMsg.message.MessageU16.body) >= 0 )
            {
                commandMsg.type = getTrainDriverMessageType(commandMsg.message.MessageU16.body);
                sysReply(output.value, &commandMsg);

                U16 id;
                U8 oldType;
                assertOk(queueU16Pop(&blockedTasks, &id));
                assertOk(queueU8Pop(&blockedTaskOriginalType, &oldType));

                rcvMessage.type = oldType;
                rcvMessage.message.MessageU32.body = 0;
                
                if( oldType < MESSAGE_TRAIN_GET_SENSOR )
                {
                    sysReply(id, &rcvMessage);
                }
                else
                {
                    assert(id == activeSensorRequestId);
                }
            }
            else
            {
                isOutputWaiting = 1;
            }
            break;
        }

        case DRIVER_MESSAGE_RCV_SENSORS:
        {
            sysReply(rcvID.value, &rcvMessage);
            sysReply(activeSensorRequestId, &rcvMessage);
            activeSensorRequestId = 0;
            break;
        }

        default:
            assert(0);
            break;
        }
    }
}

void TrainOutputNotifier(void)
{
    TaskID server;
    MessageEnvelope notif;

    server.value = sysPid();

    notif.type = DRIVER_MESSAGE_TX_TRAIN;
    
    while( sysRunning() != 0 )
    {
        sysSend(server.value, &notif, &notif);
        
        sysWrite(EVENT_TRAIN_WRITE, notif.message.MessageU16.body & 0x00FF);
        if(notif.type == DRIVER_MESSAGE_TX_TRAIN_MULTI_BYTE)
            sysWrite(EVENT_TRAIN_WRITE, notif.message.MessageU16.body >> 8);
    }
}

void TrainInputNotifier(void)
{
    TaskID server;
    MessageEnvelope notif;
    notif.type = DRIVER_MESSAGE_RCV_SENSORS;

    server.value = sysPid();
    while( sysRunning() != 0 )
    {
        notif.message.MessageU16.body = sysRead(EVENT_TRAIN_READ) << 8;
        notif.message.MessageU16.body |= sysRead(EVENT_TRAIN_READ);
        sysSend( server.value, &notif, &notif ); 
    }
}
