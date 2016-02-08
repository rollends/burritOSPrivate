#ifndef UART_H
#define UART_H

#include "common/common.h"

/**
 * Sets UART enabled and interrupt state
 *
 * @param   uart    The UART port
 * @param   enable  The enable bit
 * @param   timeout The read timeout interrupt bit
 * @param   trans   The transmit interrupt bit
 * @param   rcv     The receive interrupt bit
 * @param   modem   The mode status bit
 *
 * @return  0 on success, else an error code
 */
S32 uartEnable(const U32 uart,
               const U32 enable,
               const U32 timeout,
               const U32 trans,
               const U32 rcv,
               const U32 modem);

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
S32 uartConfig(const U32 uart,
               const U32 fifo,
               const U32 stp,
               const U32 pen);

/**
 * Blocking write to a UART
 *
 * @param   uart    The UART port to write to
 * @param   byte    The value to write
 *
 * @return 0 on success, else an error code
*/
S32 uartWriteByteBlock(const U32 uart, const U8 byte);
S32 uartWriteByte(const U32 uart, const U8 byte);

/**
 * Blocking read from a UART
 *
 * @param   uart    The UART port to read from
 * @param   byte    The value to read into
 *
 * @return 0 on success, else an error code
*/
S32 uartReadByteBlock(const U32 uart, U8* byte);
S32 uartReadByte(const U32 uart, U8* byte);

/**
 * Read the interrupt status of the UART and clears it
 *
 * @param   uart    The UART port to read from
 *
 * @return  The interrupt status bits
 */
U8 uartInterruptStatus(const U32 uart);

#endif
