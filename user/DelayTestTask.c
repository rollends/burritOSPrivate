#include "common/random.h"

#include "kernel/message.h"
#include "kernel/print.h"
#include "kernel/sysCall.h"

#include "user/messageTypes.h"
#include "user/Nameserver.h"
#include "user/DelayTestTask.h"
#include "user/ClockServer.h"

void DelayTestTask()
{
	TaskID clock;
	U16 parentId = sysPid();
	U16 tid = sysTid();
	MessageEnvelope envelope;

	sysSend( parentId, &envelope, &envelope );
	
	U16 delayTime = (envelope.message.MessageU32.body & 0xFFFF0000) >> (8*2);
	U16 delayCount = (envelope.message.MessageU32.body & 0x0000FFFF);

	nsWhoIs( Clock, &clock );

	U16 delayFinished = 0;
	while( delayCount )
	{
		clockDelayBy( clock, delayTime );
		U32 time = clockTime( clock );
		printString( "  %d             %d                    %d          %d\r\n", 
					tid,
					++delayFinished, 
					delayTime,
					time );
		delayCount--;
	}
	sysShutdown();
	sysExit();
}
