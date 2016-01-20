#include "uart.h"

int uartSpeed(const int uart, const int speed)
{
    volatile int* high = (volatile int*)(uart + UART_LCRM_OFFSET);
    volatile int* low = (volatile int*)(uart + UART_LCRL_OFFSET);

    *high = 0x0;
    *low = speed;

    return 0;
}

int uartConfig(const int uart, const int fifo, const int stp, const int pen)
{
    volatile int* config = (volatile int*)(uart + UART_LCRH_OFFSET);
    *config = (fifo ? *config | FEN_MASK : *config & ~FEN_MASK);
    *config = (stp ? *config | STP2_MASK : *config & ~STP2_MASK);
    *config = (pen ? *config | PEN_MASK : *config & ~PEN_MASK);
    *config = *config | WLEN_MASK;
    
    return 0;
}

int uartWriteChar(const int uart, const char c)
{
    volatile int* flags = (volatile int*)(uart + UART_FLAG_OFFSET);
    volatile int* data = (volatile int*)(uart + UART_DATA_OFFSET);

    while (*flags & TXFF_MASK)
    {
    }

    *data = c;

    return 0;
}

int uartReadChar(const int uart, char* c)
{
    volatile int* flags = (volatile int*)(uart + UART_FLAG_OFFSET);
    volatile int* data = (volatile int*)(uart + UART_DATA_OFFSET);

    while (!(*flags & RXFF_MASK))
    {
    }

    *c = *data;

    return 0;
}
