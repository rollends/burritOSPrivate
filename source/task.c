#include "task.h"

S32 taskTableInit(TaskTable* table)
{
    U8 i = 0;

    for(i = 0; i < TASK_COUNT; ++i)
    {
        table->table[i].tid.fields.id = i + 1;
        table->table[i].tid.fields.generation = 0;
        table->table[i].pid.value = 0;
        table->table[i].state = eZombie;
    }

    for (i = 0; i < TASK_BIT_COUNT; ++i)
    {
        table->tableFlag[i] = 0;
    }

    return 0;
}

S32 taskTableAlloc(TaskTable* table, TaskDescriptor** desc)
{
    U32* pflag = table->tableFlag;
    
    for( pflag = table->tableFlag;
         !(~(*pflag)) && ((pflag - table->tableFlag) < TASK_BIT_COUNT);
         pflag++ ); 
    
    if( !(~*pflag) || ((pflag - table->tableFlag) >= TASK_BIT_COUNT ) )
    {
        return -1;
    }

    U32 flag = *pflag;
    U32 index = 0;
    while( !(flag & 1) ) 
    {
        flag >>= 1;
        ++index;
    }

    *pflag |= 1 << index;

    *desc = &table->table[index + (pflag - table->tableFlag)];
    (*desc)->tid.fields.generation++;

    return 0;
}

S32 taskTableFree(TaskTable* table, TaskDescriptor* desc)
{
    desc->pid.value = 0;

    U8 index = desc->tid.fields.id;
    U32* pflag = table->tableFlag + (index + 31) / 32 - 1;
    *pflag ^= (1 << ((index - 1) % 32));

    return 0;
}