#include "task.h"

S32 taskInit(Tasks* table)
{
	U8 i = 0;
	for(i = 0; i < TaskTableLength; ++i)
	{
		table->descriptors[i].tid.fields.id = i + 1;
		table->descriptors[i].tid.fields.generation = 0;
		table->descriptors[i].pid.value = 0;
		table->descriptors[i].state = eZombie;
	}

	for(i = 0; i < TaskTableFlagLength; ++i)
		table->tableFlag[i] = 0;
	return 0;
}

TaskDescriptor* taskGetDescriptor(Tasks* tasks, U16 tid)
{
	TaskID fid;
	fid.value = tid;
	return &tasks->descriptors[fid.fields.id - 1];
}

S32 taskFree(Tasks* table, U16 id)
{
	TaskID tid;
	tid.value = id;
	U32 index = tid.fields.id - 1;

	TaskDescriptor* desc = &table->descriptors[index];

	if( desc->state != eZombie )
		return -1;

	// Flag this task descriptor as free again
	U32 tableFlagIndex = index % 32;
	U32 tableFlagShift = index / 32;
	table->tableFlag[tableFlagIndex] ^= (1 << tableFlagShift);

	// Increment as a next generation Id
	desc->tid.fields.generation++;
	desc->pid.value = 0;
	desc->state = eZombie;
	return 0;
}

S32 taskAlloc(Tasks* table, U8 priority, U32 entry, U16 pid)
{
	U32* pflag = table->tableFlag;
	U32 i = 0;

	for( i = 0; i < TaskTableFlagLength; ++i )
	{
		if( ~(*pflag) )
		{
			break;
		}
		++pflag;
	}
	if( !(~*pflag) || i >= TaskTableFlagLength )
	{
		return -1;
	}

	U32 flag = *pflag;
	U32 index = 0;
	while( flag & 1 ) 
	{
		flag >>= 1;
		++index;
	}

	*pflag |= 1 << index;
	

	U32 id = index + 32 * (pflag - table->tableFlag);
	TaskDescriptor* desc = &table->descriptors[id];
	
	U32 tid = id + 1;
	U32* stack = table->stackMemory+(1024*tid - 1);
    *(stack) = entry;
    *(stack-15) = 0x10;
    
	desc->stack = stack - 15;
	desc->state = eReady;
    //desc->tid.fields.generation = 0;
    //desc->tid.fields.id = tid;
    desc->pid.fields.id = pid;
    desc->priority = priority;

    return desc->tid.value;
}
