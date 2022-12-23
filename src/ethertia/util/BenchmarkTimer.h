//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_BENCHMARKTIMER_H
#define ETHERTIA_BENCHMARKTIMER_H

#include <chrono>
#include <iostream>

#include <ethertia/util/Timer.h>
#include <ethertia/util/Strings.h>

class BenchmarkTimer
{
    double begin = Timer::nanoseconds();

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

    void done()
    {
        double d_ms = (Timer::nanoseconds() - begin) * 0.000001;

        if (timerAppend) {
            *timerAppend += d_ms * 0.001f;
        }
        if (tailmsg) {
            std::string t_str;
            if (d_ms > 1000) {
                t_str = std::to_string(d_ms * 0.001) + "s";
            } else {
                t_str = std::to_string(d_ms) + "ms";
            }
            std::cout << Strings::fmt(tailmsg, t_str);
            std::cout.flush();
        }

        m_Done = true;
    }
};

#endif //ETHERTIA_BENCHMARKTIMER_H
