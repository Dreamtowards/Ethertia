//
// Created by Dreamtowards on 2023/5/23.
//

#include "BenchmarkTimer.h"

#include <chrono>
#include <iostream>

#include <ethertia/util/Timer.h>
#include <ethertia/util/Strings.h>


BenchmarkTimer::BenchmarkTimer(float *_tmr, const char *_msg) : m_Begin(Timer::nanoseconds()), m_AppendTime(_tmr), m_TailMsg(_msg)
{
}

BenchmarkTimer::~BenchmarkTimer()
{
    if (!m_Done)
    {
        done();
    }
}

void BenchmarkTimer::done()
{
    double d_ms = (Timer::nanoseconds() - m_Begin) * 0.000001;

    if (m_AppendTime) {
        *m_AppendTime += d_ms * 0.001f;
    }
    if (m_TailMsg) {
        std::string t_str;
        if (d_ms > 1000) {
            t_str = std::to_string(d_ms * 0.001) + "s";
        } else {
            t_str = std::to_string(d_ms) + "ms";
        }
        std::cout << Strings::fmt(m_TailMsg, t_str);
        std::cout.flush();
    }

    m_Done = true;
}