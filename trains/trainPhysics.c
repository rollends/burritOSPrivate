#include "trains/trainMath.h"
#include "trains/trainPhysics.h"

void trainPhysicsInit(TrainPhysics* physics)
{
    physics->velocity = 0;
    physics->acceleration = 0;
    physics->targetVelocity = 0;
    physics->speed = 0;
    physics->targetSpeed = 0;
    physics->distance = 0;
    physics->initialVelocity = 0;
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

    physics->calibrationCount[0] = 0;

    U8 i;
    for (i = 1; i < 14; i++)
    {
        physics->calibrationCount[i] = 10;
    }
}

void trainPhysicsAccelMap(TrainPhysics* physics,
                          const S32 speed,
                          const S32 v0, const S32 v5, const S32 v6,
                          const S32 v7, const S32 v8, const S32 v9,
                          const S32 v10, const S32 v11, const S32 v12,
                          const S32 v13)
{
    physics->accelMap[speed][ 0] = v0;
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

U32 trainPhysicsStep(TrainPhysics* physics, const U32 delta)
{
    U32 ktick = (delta/100);
    ktick += 5;
    ktick /= 10;

    if (physics->acceleration != 0)
    {
        S32 a = physics->acceleration / 100;
        S32 aDist = ktick*ktick/200;
        aDist *= a;

        physics->distance += (aDist/1000000);
        physics->velocity += ((a + 5)/10 * ktick);

        if (((a > 0) && (physics->velocity >= physics->targetVelocity)) ||
            ((a < 0) && (physics->velocity <= physics->targetVelocity)))
        {
            physics->acceleration = 0;
            physics->velocity = physics->targetVelocity;
            physics->speed = physics->targetSpeed;
        }
    }

    S32 steadyDist = (physics->velocity/100);
    steadyDist *= ktick;
    steadyDist = ((steadyDist/1000) + 5)/10;

    physics->distance += steadyDist;

    return steadyDist;
}

void trainPhysicsSetSpeed(TrainPhysics* physics, const U8 speed)
{
    if (speed != physics->speed)
    {
        physics->targetVelocity = physics->speedMap[speed];
        physics->targetSpeed = speed;
        physics->acceleration = physics->accelMap[physics->speed][speed];
        physics->initialVelocity = physics->velocity;
        physics->initialSpeed = physics->speed;
    }
}

S32 trainPhysicsReport(TrainPhysics* physics,
                        const S32 dist,
                        const U32 ticks,
                        const S32 tickError)
{
    U32 ktick = (ticks / 100);
    ktick += 5;
    ktick /= 10;

    physics->distance = 0;
    
    if (physics->acceleration == 0 && physics->initialVelocity == 0)
    {
        if (physics->calibrationCount[physics->speed] > 0 &&
            tickError < 200 && tickError > -200)
        {
            // V = v*0.9 + d/t*0.1
            S32 velocity = physics->velocity;
            velocity = (velocity * 95)/100;
            velocity += (dist*1000*50)/ktick;

            physics->velocity = velocity;
            physics->calibrationCount[physics->speed] -=1;
            physics->speedMap[physics->speed] = velocity;

            return 1;
        }
    }
    else
    {
        // a = (f - i)*(i - f)/(2*(x - f*t))
        S32 a = (physics->targetVelocity - physics->initialVelocity)/10;
        a *= (physics->initialVelocity - physics->targetVelocity)/20;
        S32 x = dist*1000 - ((physics->targetVelocity*ktick)/1000);
        a /= x;
        physics->initialVelocity = 0;

        S32 oldA = (physics->accelMap[physics->initialSpeed][physics->targetSpeed] * 95)/100;
        oldA += a * 5;
        physics->accelMap[physics->initialSpeed][physics->targetSpeed] = oldA;

        return a;
    }

    return 0;
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
        accelTime *= 100;
        accelTime /= physics->acceleration;
        accelTime *= 10;

        // 0.5at^2 + v*t = x
        S32 vDist = physics->velocity * accelTime;
        vDist /= 1000000;

        S32 aDist = (accelTime*accelTime);
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

            S32 disc = intSqrt(vsqr + ac);
           
            S32 ticks1 = (1000 * (-physics->velocity + disc*1000)) / physics->acceleration;
            S32 ticks2 = (1000 * (-physics->velocity - disc*1000)) / physics->acceleration;

            if ((ticks1 > 0 && ticks1 < ticks2) ||
                (ticks2 < 0))
            {
                ticks = ticks1;
            }
            else
            {
                ticks = ticks2;
            }
        }
    }

    return ticks * 1000;
}

S32 trainPhysicsStopDist(TrainPhysics* physics)
{
    S32 v = physics->velocity/100;
    S32 a = physics->accelMap[physics->targetSpeed][0] / -10;
    S32 dist = (v * v)/(2*a);
    dist += (v*550)/10000;

    return dist;
}
