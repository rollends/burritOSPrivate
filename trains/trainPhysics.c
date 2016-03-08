#include "trains/trainMath.h"
#include "trains/trainPhysics.h"

void trainPhysicsInit(TrainPhysics* physics)
{
    physics->velocity = 0;
    physics->acceleration = 0;
    physics->targetVelocity = 0;
    physics->speed = 0;
    physics->targetSpeed = 0;
}

void trainPhysicsSpeedMap(TrainPhysics* physics, 
                          const S32 v5, const S32 v6, const S32 v7,
                          const S32 v8, const S32 v9, const S32 v10,
                          const S32 v11, const S32 v12, const S32 v13)
{
    physics->speedMap[ 0] = 0;
    physics->speedMap[ 1] = 0;
    physics->speedMap[ 2] = 0;
    physics->speedMap[ 3] = 0;
    physics->speedMap[ 4] = 0;

    physics->speedMap[ 5] = v5;
    physics->speedMap[ 6] = v6;
    physics->speedMap[ 7] = v7;
    physics->speedMap[ 8] = v8;
    physics->speedMap[ 9] = v9;
    physics->speedMap[10] = v10;
    physics->speedMap[11] = v11;
    physics->speedMap[12] = v12;
    physics->speedMap[13] = v13;
}

void trainPhysicsAccelMap(TrainPhysics* physics,
                          const S32 speed,
                          const S32 v5, const S32 v6, const S32 v7,
                          const S32 v8, const S32 v9, const S32 v10,
                          const S32 v11, const S32 v12, const S32 v13)
{
    physics->accelMap[speed][ 0] = 0;
    physics->accelMap[speed][ 1] = 0;
    physics->accelMap[speed][ 2] = 0;
    physics->accelMap[speed][ 3] = 0;
    physics->accelMap[speed][ 4] = 0;

    physics->accelMap[speed][ 5] = v5;
    physics->accelMap[speed][ 6] = v6;
    physics->accelMap[speed][ 7] = v7;
    physics->accelMap[speed][ 8] = v8;
    physics->accelMap[speed][ 9] = v9;
    physics->accelMap[speed][10] = v10;
    physics->accelMap[speed][11] = v11;
    physics->accelMap[speed][12] = v12;
    physics->accelMap[speed][13] = v13;
}

void trainPhysicsStep(TrainPhysics* physics, const U32 delta)
{
    if (physics->acceleration != 0)
    {
        U32 ktick = (delta / 1000);

        S32 a = physics->acceleration / 100;

        physics->velocity += (a * ktick);

        if (a > 0 && physics->velocity >= physics->targetVelocity ||
            a < 0 && physics->velocity <= physics->targetVelocity)
        {
            physics->acceleration = 0;
            physics->velocity = physics->targetVelocity;
            physics->speed = physics->targetSpeed;
        }
    }
}

void trainPhysicsSetSpeed(TrainPhysics* physics, const U8 speed)
{
    if (speed != physics->speed)
    {
        physics->targetVelocity = physics->speedMap[speed];
        physics->targetSpeed = speed;
        physics->acceleration = physics->accelMap[physics->speed][speed];
    }
}

void trainPhysicsReport(TrainPhysics* physics,
                        const S32 dist,
                        const U32 ticks)
{
    U32 ktick = (ticks / 1000);

    if (physics->acceleration == 0)
    {
        // V = v*0.9 + d/t*0.1
        S32 velocity = physics->velocity;
        velocity = (velocity * 9)/10;
        velocity += (dist*1000*100)/ktick;

        physics->velocity = velocity;
    }
    else
    {

    }
}

S32 trainPhysicsGetVelocity(TrainPhysics* physics)
{
    return physics->velocity;
}

U32 trainPhysicsGetTime(TrainPhysics* physics, const S32 dx)
{
    U32 ticks = 0;

    if (physics->acceleration == 0)
    {
        ticks = (dx*1000*1000)/physics->velocity;
    }
    else
    {
        // vf = vi + a*t
        // t = (vf-vi)/a
        S32 accelTime = physics->targetVelocity - physics->velocity;
        accelTime *= 1000;
        accelTime /= physics->acceleration;

        // 0.5at^2 + v*t = x
        S32 vDist = physics->velocity * accelTime;
        vDist /= 1000000;

        S32 aDist = accelTime * accelTime;
        aDist /= 2000;
        aDist *= physics->acceleration;
        aDist /= 1000000;

        S32 totalDist = aDist + vDist;

        // x <= dx
        if (totalDist <= dx)
        {
            ticks = accelTime;
            ticks += ((dx-totalDist)*1000*1000)/physics->targetVelocity;
        }
        // x > dx
        else
        {
            // 0.5at^2 + vt = dx
            // t = (-v + sqrt(v^2 - 2adx))/a
            S32 vsqr = (physics->velocity / 100);
            vsqr *= vsqr;
            vsqr /= 100;

            S32 ac = 2 * physics->acceleration * dx;
            ac /= 1000;

            U32 disc = intSqrt(vsqr + ac);
            
            ticks = (1000 * (-physics->velocity + disc*1000));
            ticks /= physics->acceleration;
        }
    }

    return ticks;
}
