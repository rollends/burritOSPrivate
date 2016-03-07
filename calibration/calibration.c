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

    U8 speed = 13;

    while (speed > 2)
    {
        U8 incspeed = 13;
        U32 start, end;

        while (incspeed > 2)
        {
            U8 samples = 0;
            printBlocking("Vi:\t%d\r\n", speed);
            printBlocking("Vf:\t%d\r\n", incspeed);
            while (samples < 10)
            {
                trainByte(speed);
                trainByte(62);

                U32 delay;
                volatile U32 vol = 0;
                for (delay = 0; delay < 50000; delay++)
                {
                    vol++;
                }
            
                while (1)
                {
                    trainByte(0xC5);
                    U8 byte1, byte2;
                    uartReadByteBlock(UART_1, &byte1);
                    uartReadByteBlock(UART_1, &byte2);
                    if (byte2 == 0x10)
                    {
                        break;
                    }
                }

                timerGetValue(TIMER_4, &start);

                trainByte(incspeed);
                trainByte(62);

                for (delay = 0; delay < 50000; delay++)
                {
                    vol++;
                }

                while (1)
                {
                    trainByte(0xC3);
                    U8 byte1, byte2;
                    uartReadByteBlock(UART_1, &byte1);
                    uartReadByteBlock(UART_1, &byte2);

                    if (byte2 == 0x01)
                    {
                        break;
                    }
                }

                timerGetValue(TIMER_4, &end);
                printBlocking("%d\r\n", end-start);

                trainByte(speed);
                trainByte(62);

                for (delay = 0; delay < 50000; delay++)
                {
                    vol++;
                }

                samples++;
            }

            incspeed--;
        }

        speed--;

        printBlocking("\r\n");
    }

    return 0;
}

#endif
