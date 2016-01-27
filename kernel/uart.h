#ifndef UART_H
#define UART_H

#include "common/types.h"
#include "kernel/ts7200.h"

#define UART_PORT_1 UART1_BASE
#define UART_PORT_2 UART2_BASE

#define UART_SPEED_LO 0xBF
#define UART_SPEED_HI 0x3

/**
 * Sets the transfer speed for a given UART port
 *
 * @param   uart    The UART port
 * @param   speed   The speed to set
 *
 * @return 0 on success, else an error code
*/
S32 uartSpeed(const U32 uart, const U32 speed);

/**
 * Sets UART frame configuration
 *
 * @param   uart    The UART port
 * @param   fifo    Whether or not to use FIFO
 * @param   stp     The number of stop bits
 * @param   pen     Whether or not to use parity checking
 *
 * @return 0 on success, else an error code
*/
S32 uartConfig(const U32 uart, const U32 fifo, const U32 stp, const U32 pen);

/**
 * Blocking write to a UART
 *
 * @param   uart    The UART port to write to
 * @param   byte    The value to write
 *
 * @return 0 on success, else an error code
*/
S32 uartWriteByte(const U32 uart, const U8 byte);

/**
 * Blocking read from a UART
 *
 * @param   uart    The UART port to read from
 * @param   byte    The value to read into
 *
 * @return 0 on success, else an error code
*/
S32 uartReadByte(const U32 uart, U8* byte);

#endif
