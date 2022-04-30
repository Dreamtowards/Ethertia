//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_TIMER_H
#define ETHERTIA_TIMER_H

class Timer
{
    static constexpr int TPS = 20;

    float delta;
    float lastUpdateTime = 0;

    float elapsedTicks;


public:

    void update(float currTime) {
        if (lastUpdateTime == 0)
            lastUpdateTime = currTime;

        delta = currTime - lastUpdateTime;

        elapsedTicks += delta / TPS;
        lastUpdateTime = currTime;
    }

    float getDelta() {
        return delta;
    }

    bool polltick() {
        if (elapsedTicks < 1.0)
            return false;
        elapsedTicks -= 1.0;
        return true;
    }
};

#endif //ETHERTIA_TIMER_H
