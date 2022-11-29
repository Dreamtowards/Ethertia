//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_BENCHMARKTIMER_H
#define ETHERTIA_BENCHMARKTIMER_H

#include <chrono>
#include <iostream>

class BenchmarkTimer
{
    std::chrono::time_point<std::chrono::high_resolution_clock> begin = std::chrono::high_resolution_clock::now();

public:
    BenchmarkTimer(const char* _msg) {
        std::cout << _msg;
        std::cout.flush();
    }
    BenchmarkTimer() {}

    ~BenchmarkTimer()
    {
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::time_point_cast<std::chrono::nanoseconds>(end).time_since_epoch().count() -
                        std::chrono::time_point_cast<std::chrono::nanoseconds>(begin).time_since_epoch().count();

        double d = duration * 0.001 * 0.001;

        if (d > 1000) {
            std::cout << " used " << d/1000 << " s."  << std::endl;
        } else {
            std::cout << " used " << d      << " ms." << std::endl;
        }
        std::cout.flush();
    }
};

#endif //ETHERTIA_BENCHMARKTIMER_H
