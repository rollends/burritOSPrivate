#include "common/common.h"
#include "hardware/hardware.h"
#include "kernel/kernelData.h"
#include "kernel/print.h"
#include "kernel/kernelUtils.h"
#include "user/IdleTask.h"

U32* bootstrap(U32 pc)
{
    uartEnable(UART_2, 1, 0, 1, 1, 0);
    uartSpeed(UART_2, UART_SPEED_HI);
    uartConfig(UART_2, 0, 0, 0);
    
    timerInit(TIMER_1);
    timerSetValue(TIMER_1, 5080);
    timerClear(TIMER_1);

    timerInit(TIMER_2);
    timerSetValue(TIMER_2, 50800);
    timerClear(TIMER_2);

    timerInit(TIMER_4);

    interruptEnable(INT_1, 0x02000030);

    printString("%c[2J\r", 27);

    kernelDataInit(pc);

    U16 taskID = taskTableAlloc(&kernel.tasks,
                                PRIORITY_COUNT - 1,
                                (U32)(&IdleTask) + pc,
                                (U32)(&__taskExit) + pc,
                                STACK_SIZE_SMALL,
                                VAL_TO_ID(0));

    kernel.activeTask = taskGetDescriptor(&kernel.tasks, VAL_TO_ID(taskID));
    return kernel.activeTask->stack;
}

void cleanup()
{
    interruptClear(INT_1, 0xFFFFFFFF);
    interruptClear(INT_2, 0xFFFFFFFF);
}
