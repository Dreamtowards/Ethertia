//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_BENCHMARKTIMER_H
#define ETHERTIA_BENCHMARKTIMER_H


#define BENCHMARK_TIMER_MSG(msg) BenchmarkTimer _tm(nullptr, msg);
#define BENCHMARK_TIMER_VAL(ptr) BenchmarkTimer _tm(ptr, nullptr);

#define BENCHMARK_TIMER BenchmarkTimer _tm;


class BenchmarkTimer
{
public:
    BenchmarkTimer(float* _tmr = nullptr, const char* _msg = " in {}.\n");
    ~BenchmarkTimer();

    void done();

private:

    double m_Begin;

    const char* m_TailMsg = nullptr;
    float* m_AppendTime = nullptr;

    bool m_Done = false;

};

#endif //ETHERTIA_BENCHMARKTIMER_H
