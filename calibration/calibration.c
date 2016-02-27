#include "common/common.h"
#include "hardware/hardware.h"
#include "kernel/kernel.h"

#ifdef CALIBRATION_BUILD

void trainByte(const U8 byte)
{
    U8 cts = 0;
    do
    {
        uartCTS(UART_1, &cts);
    }
    while (cts == 0);

    assertOk(uartWriteByteBlock(UART_1, byte));

    U32 delay;
    volatile U32 vol = 0;
    for (delay = 0; delay < 5000; delay++)
    {
        vol++;
    }
}

U32* bootstrap()
{
    assertOk(uartEnable(UART_2, 1, 0, 0, 0, 0));
    assertOk(uartSpeed(UART_2, UART_SPEED_HI));
    assertOk(uartConfig(UART_2, 0, 0, 0));
    
    assertOk(uartEnable(UART_1, 1, 0, 0, 0, 0));
    assertOk(uartSpeed(UART_1, UART_SPEED_LO));
    assertOk(uartConfig(UART_1, 0, 1, 0));

    assertOk(timerEnable(TIMER_4, 1, 0, 0));

    trainByte(0x61);
    trainByte(0x60);

    while (1)
    {
        U8 speed = 14;
        while (speed > 2)
        {
            trainByte(speed);
            trainByte(68);

            U32 delay;
            volatile U32 vol = 0;
            for (delay = 0; delay < 50000; delay++)
            {
                vol++;
            }
         
            U32 start, end;
            while (1)
            {
                trainByte(0xC3);
                U8 byte1, byte2;
                uartReadByteBlock(UART_1, &byte1);
                uartReadByteBlock(UART_1, &byte2);
                if (byte2 == 0x02)
                {
                    break;
                }
            }

            timerGetValue(TIMER_4, &start);
            while (1)
            {
                trainByte(0xC5);
                U8 byte1, byte2;
                uartReadByteBlock(UART_1, &byte1);
                uartReadByteBlock(UART_1, &byte2);

                if (byte2 == 0x20)
                {
                    break;
                }
            }
            timerGetValue(TIMER_4, &end);
            printBlocking("%d\r\n", end-start);

            speed--;
        }

        printBlocking("\r\n");
    }

    return 0;
}

#endif
