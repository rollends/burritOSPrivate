#ifndef NAME_SERVICE_H
#define NAME_SERVICE_H

#include "common/types.h"
#include "kernel/kernel.h"

/** Task Name Enumerated List
 *
 *  This list enumerates the task names with
 *  an appropriate index that is used internally
 *  by the Nameserver in order to accelerate name
 *  lookup/registration. 
 */
typedef enum
{
    Clock             = 0,
    Terminal          = 1,
    Train             = 2,
    TrainSwitches     = 3,
    TrainSensors      = 4,
    TrainSwitchOffice = 5,
    TaskNameCount     = 6,
} TaskName;

/** Nameserver Registration Method
 *
 *  This method is called by user tasks to register themselves to
 *  a particular name. Currently registration of all names must be performed before any
 *  WhoIs requests are served. This may lead to race conditions if the
 *  initial tasks are not structured correctly. Use with caution.
 *
 *  @param name The Name the calling task should be registered to.
 *  @return Zero on success.
 */
S32 nsRegister( TaskName name );

/** Nameserver Lookup Method
 *
 *  This method is called by user tasks in order to find a particular
 *  task. See nsRegister for warnings.
 *
 *  @param name Task Name to lookup.
 *  @param sid TaskID (by ref) to store the result in.
 *  @see nsRegister()
 */
TaskID nsWhoIs( TaskName name );

#endif
