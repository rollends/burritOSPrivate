#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "common/common.h"

/**
 * Enables interrupts in the given VIC
 *
 * @param   interrupt   The VIC in which the bits should be enabled
 * @param   bits        The enable bit values
 */
inline void interruptEnable(const U32 interrupt, const U32 bits);

/**
 * Clears interrupts in the given VIC
 *
 * @param   interrupt   The VIC in which bits should be cleared
 * @param   bits        The clear bit values
 */
inline void interruptClear(const U32 interrupt, const U32 bits);

/**
 * Returns the interrupt status bits for a givne interrupt vector
 *
 * @param   interrupt   The VIC to check
 *
 * @return  The status word for the given vector
 */
inline U32 interruptStatus(const U32 interrupt);

#endif
