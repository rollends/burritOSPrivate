#include "hardware/memory.h"
#include "hardware/uart.h"
#include "hardware/ts7200/ts7200.h"

#ifdef DEVICE_CHECK
    #define IS_DEVICE(d) \
        do { if (d!=UART_1 && d!=UART_2) \
                return ERROR_DEVICE; } while(0)
#else
    #define IS_DEVICE(d)
#endif

RETURN uartEnable(const U32 uart,
                  const U32 enable,
                  const U32 timeout,
                  const U32 trans,
                  const U32 rcv,
                  const U32 modem)
{
    IS_DEVICE(uart);

    RWRegister control = (RWRegister)(uart + UART_CTLR_OFFSET);
    
    U32 value = __ldr(control);
    value = (enable ? value | UARTEN_MASK : value & ~UARTEN_MASK);
    value = (modem ? value | MSIEN_MASK : value & ~MSIEN_MASK);
    value = (rcv ? value | RIEN_MASK : value & ~RIEN_MASK);
    value = (trans ? value | TIEN_MASK : value & ~TIEN_MASK);
    value = (timeout ? value | RTIEN_MASK : value & ~RTIEN_MASK);
    __str(control, value);

    IS_OK();
}

RETURN uartSpeed(const U32 uart, const U32 speed)
{
    IS_DEVICE(uart);

    RWRegister mid = (RWRegister)(uart + UART_LCRM_OFFSET);
    RWRegister low = (RWRegister)(uart + UART_LCRL_OFFSET);

    __str(mid, 0x0);
    __str(low, speed);

    IS_OK();
}

RETURN uartConfig(const U32 uart,
                  const U32 fifo,
                  const U32 stp,
                  const U32 pen)
{
    IS_DEVICE(uart);

    RWRegister high = (RWRegister)(uart + UART_LCRH_OFFSET);

    U32 value = __ldr(high);
    value = (fifo ? value | FEN_MASK : value & ~FEN_MASK);
    value = (stp ? value | STP2_MASK : value & ~STP2_MASK);
    value = (pen ? value | PEN_MASK : value & ~PEN_MASK);
    value = value | WLEN_MASK;
    __str(high, value);
    
    IS_OK();
}

RETURN uartWriteByteBlock(const U32 uart, const U8 byte)
{
    IS_DEVICE(uart);

    RWRegister flags = (RWRegister)(uart + UART_FLAG_OFFSET);
    RWRegister data = (RWRegister)(uart + UART_DATA_OFFSET);

    while (*flags & TXFF_MASK)
    {
    }

    __strb(data, byte);

    IS_OK();
}

RETURN uartWriteByte(const U32 uart, const U8 byte)
{
    IS_DEVICE(uart);

    RWRegister data = (RWRegister)(uart + UART_DATA_OFFSET);
    __strb(data, byte);

    IS_OK();
}

RETURN uartReadByteBlock(const U32 uart, U8* byte)
{
    IS_DEVICE(uart);
    IS_NOT_NULL(byte);
    IS_IN_RANGE(byte);

    RWRegister flags = (RWRegister)(uart + UART_FLAG_OFFSET);
    RWRegister data = (RWRegister)(uart + UART_DATA_OFFSET);

    while (!(*flags & RXFF_MASK))
    {
    }

    *byte = __ldr(data);

    IS_OK();
}

RETURN uartReadByte(const U32 uart, U8* byte)
{
    IS_DEVICE(uart);
    IS_NOT_NULL(byte);
    IS_IN_RANGE(byte);

    RWRegister data = (RWRegister)(uart + UART_DATA_OFFSET);
    *byte = __ldr(data);

    IS_OK();
}

RETURN uartCTS(const U32 uart, U8* cts)
{
    IS_DEVICE(uart);
    IS_NOT_NULL(cts);
    IS_IN_RANGE(cts);

    RWRegister flags = (RWRegister)(uart + UART_FLAG_OFFSET);

    *cts = __ldr(flags) & CTS_MASK;

    IS_OK();
}

RETURN uartInterruptTX(const U32 uart, const U8 enable)
{
    IS_DEVICE(uart);

    RWRegister control = (RWRegister)(uart + UART_CTLR_OFFSET);
    
    U32 value = __ldr(control);
    value = (enable ? value | TIEN_MASK : value & ~TIEN_MASK);
    __str(control, value);

    IS_OK();
}

RETURN uartInterruptStatus(const U32 uart, U32* status)
{
    IS_DEVICE(uart);
    IS_NOT_NULL(status);
    IS_IN_RANGE(status);

    RWRegister stat = (RWRegister)(uart + UART_INTR_OFFSET);
    *status = __ldr(stat) & 0xF;
    __str(stat, 0);

    IS_OK();
}

#undef IS_DEVICE
