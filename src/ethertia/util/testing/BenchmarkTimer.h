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
    const char* name;

public:
    BenchmarkTimer(const char* _name) : name(_name) {}
    BenchmarkTimer() {}

    ~BenchmarkTimer()
    {
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::time_point_cast<std::chrono::nanoseconds>(end).time_since_epoch().count() -
                        std::chrono::time_point_cast<std::chrono::nanoseconds>(begin).time_since_epoch().count();

        double d = duration * 0.001 * 0.001;

        std::cout << "\"" << name << "\" done. used " << d << " ms." << std::endl;
    }
};

#endif //ETHERTIA_BENCHMARKTIMER_H
