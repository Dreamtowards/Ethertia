//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_TIMER_H
#define ETHERTIA_TIMER_H

#include <chrono>
#include <thread>
#include <functional>

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


    static const long NANOSECOND_UNIT = 1000000000;

    static double nanoseconds() {  // 1ns = 1,000,000,000,  * 0.000000001
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }

    static double milliseconds() {
        return Timer::nanoseconds() * 0.000001;
    }

    static void sleep_for(int ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    template<typename T>
    static void wait_for(T* state, const T& until) {
        while (*state != until) {
            sleep_for(1);
        }
    }

    static void wait_for(const std::function<bool()>& until) {
        while (!until()) {
            sleep_for(1);
        }
    }

    static uint64_t timestampMillis() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
};

#endif //ETHERTIA_TIMER_H
