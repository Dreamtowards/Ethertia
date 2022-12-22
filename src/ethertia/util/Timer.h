//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_TIMER_H
#define ETHERTIA_TIMER_H

#include <chrono>

class Timer
{
public:
    static constexpr int TPS = 20;
    static constexpr float T_DELTA = 1.0f / TPS;

    float delta;
    float lastUpdateTime = 0;

    float elapsedTicks;

public:

    void update(float currTime) {
        if (lastUpdateTime == 0)
            lastUpdateTime = currTime;

        delta = currTime - lastUpdateTime;

        elapsedTicks += delta * TPS;
        lastUpdateTime = currTime;
    }

    float getDelta() {
        return delta;
    }

    float getPartialTick() {
        return elapsedTicks - (int)elapsedTicks;
    }

    bool polltick() {
        if (elapsedTicks < 1.0)
            return false;
        elapsedTicks -= 1.0;
        return true;
    }

    static double getHighResolutionTime() {  // 1ns = 1,000,000,000
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() * 0.000000001;
    }
};

#endif //ETHERTIA_TIMER_H
