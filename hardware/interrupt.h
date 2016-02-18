#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "common/common.h"

/**
 * Enables interrupts in the given VIC
 *
 * @param   interrupt   The VIC in which the bits should be enabled
 * @param   bits        The enable bit values
 */
inline RETURN interruptEnable(const U32 interrupt, const U32 bits);

/**
 * Clears interrupts in the given VIC
 *
 * @param   interrupt   The VIC in which bits should be cleared
 * @param   bits        The clear bit values
 */
inline RETURN interruptClear(const U32 interrupt, const U32 bits);

/**
 * Returns the interrupt status bits for a givne interrupt vector
 *
 * @param   interrupt   The VIC to check
 * @param   status      The status pointer
 */
inline RETURN interruptStatus(const U32 interrupt, U32* status);

#endif
