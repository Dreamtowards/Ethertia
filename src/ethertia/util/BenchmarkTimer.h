//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_BENCHMARKTIMER_H
#define ETHERTIA_BENCHMARKTIMER_H

#include <chrono>
#include <iostream>

#include <ethertia/util/Strings.h>

class BenchmarkTimer
{
    std::chrono::time_point<std::chrono::high_resolution_clock> begin = std::chrono::high_resolution_clock::now();

    const char* tailmsg = nullptr;
    float* timerAppend = nullptr;

    bool m_Done = false;

public:
    BenchmarkTimer(float* _tmr = nullptr, const char* _msg = " in {}.\n") : timerAppend(_tmr), tailmsg(_msg) {}

    ~BenchmarkTimer()
    {
        if (!m_Done)
        {
            done();
        }
    }

    double done()
    {
        m_Done = true;

        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::time_point_cast<std::chrono::nanoseconds>(end).time_since_epoch().count() -
                        std::chrono::time_point_cast<std::chrono::nanoseconds>(begin).time_since_epoch().count();

        double d = duration * 0.001 * 0.001;
        double d_sec = d * 0.001;

        if (timerAppend) {
            *timerAppend += d_sec;
        }
        if (tailmsg) {
            std::string t_str;
            if (d > 1000) {
                t_str = std::to_string(d / 1000.0f) + "s";
            } else {
                t_str = std::to_string(d) + "ms";
            }
            std::cout << Strings::fmt(tailmsg, t_str);
            std::cout.flush();
        }
        return d_sec;
    }
};

#endif //ETHERTIA_BENCHMARKTIMER_H
