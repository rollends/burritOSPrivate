#include "common/memory.h"
#include "common/types.h"

#include "kernel/uart.h"

S32 uartSpeed(const U32 uart, const U32 speed)
{
    RWRegister high = (RWRegister)(uart + UART_LCRM_OFFSET);
    RWRegister low = (RWRegister)(uart + UART_LCRL_OFFSET);

    __str(high, 0x0);
    __str(low, speed);

    return 0;
}

S32 uartConfig(const U32 uart, const U32 fifo, const U32 stp, const U32 pen)
{
    RWRegister config = (RWRegister)(uart + UART_LCRH_OFFSET);
    
    *config = (fifo ? *config | FEN_MASK : *config & ~FEN_MASK);
    *config = (stp ? *config | STP2_MASK : *config & ~STP2_MASK);
    *config = (pen ? *config | PEN_MASK : *config & ~PEN_MASK);
    *config = *config | WLEN_MASK;
    
    return 0;
}

S32 uartWriteByte(const U32 uart, const U8 c)
{
    RWRegister flags = (RWRegister)(uart + UART_FLAG_OFFSET);
    RWRegister data = (RWRegister)(uart + UART_DATA_OFFSET);

    while (*flags & TXFF_MASK)
    {
    }

    __strb(data, c);

    return 0;
}

S32 uartReadByte(const U32 uart, U8* c)
{
    RWRegister flags = (RWRegister)(uart + UART_FLAG_OFFSET);
    RWRegister data = (RWRegister)(uart + UART_DATA_OFFSET);

    while (!(*flags & RXFF_MASK))
    {
    }

    *c = *data;

    return 0;
}
