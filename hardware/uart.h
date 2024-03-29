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
RETURN uartEnable(const U32 uart,
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
RETURN uartSpeed(const U32 uart, const U32 speed);

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
RETURN uartConfig(const U32 uart,
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
RETURN uartWriteByteBlock(const U32 uart, const U8 byte);

/**
 * Non blocking write to the a UART - assumes CTS and TXFE
 *
 * @param   uart    The UART port to write to
 * @param   byte    the value to write
 *
 * @return  0 on success, else an error code
 */
RETURN uartWriteByte(const U32 uart, const U8 byte);

/**
 * Blocking read from a UART
 *
 * @param   uart    The UART port to read from
 * @param   byte    The value to read into
 *
 * @return 0 on success, else an error code
*/
RETURN uartReadByteBlock(const U32 uart, U8* byte);

/**
 * Non blocking read from a UART - assumes CTS and RXFE
 *
 * @param   uart    The UART port to write to
 * @param   byte    the value to read into
 *
 * @return  0 on success, else an error code
 */
RETURN uartReadByte(const U32 uart, U8* byte);

/**
 * Returns the CTS flag status for a given UART
 *
 * @param   uart    The uart to inspect
 * @param   cts     The status pointer to read into
 *
 * @return  The value of the CTS flag
 */
RETURN uartCTS(const U32 uart, U8* cts);

/**
 * Sets the TX interrupt enable flag for a given UART
 *
 * @param   uart    The uart to set the flag on
 * @param   enable  Whether or not the flag is enabled
 *
 * @return  0 on success, else an error code
 */
RETURN uartInterruptTX(const U32 uart, const U8 enable);

/**
 * Read the interrupt status of the UART and clears it
 *
 * @param   uart    The UART port to read from
 * @param   status  The status pointer to read into
 *
 * @return  0 on success, else an error code
 */
RETURN uartInterruptStatus(const U32 uart, U32* status);

#endif
