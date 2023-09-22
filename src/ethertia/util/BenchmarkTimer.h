//
// Created by Dreamtowards on 2022/4/30.
//

#pragma once

#include <chrono>


// An execution time measurer, starting when the object is constructed, and ends when the done() function or destructor is called
class BenchmarkTimer
{
public:
	// @_tmr: a pointer to a number, If not nullptr, this number will be += accumulate the entire measurement time (in seconds) at the end of the measurement.
	//        this is useful when you want sum-up or get the measurement time.
	// @_msg: a message that will be print at the end of the measurement unless it's a nullptr. "{}" is the time e.g. "85.616320ms"
	BenchmarkTimer(double* _ptm = nullptr, const char* _msg = " in {}.\n");

	// there will auto call Stop();
	~BenchmarkTimer();

	// End of Measurement
	void Stop();

private:

	std::chrono::time_point<std::chrono::high_resolution_clock> m_TimeBegin;

	const char* m_EndMsg = nullptr;
	double* m_pTimeAppend = nullptr;  // f32 or f64?

	bool m_Stopped = false;
};

// simply with default measurement message.
#define BENCHMARK_TIMER BenchmarkTimer _tm

// with Custom Message
#define BENCHMARK_TIMER_MSG(msg) BenchmarkTimer _tm(nullptr, msg)

// pure Time Accumulator.
#define BENCHMARK_TIMER_TMR(ptm) BenchmarkTimer _tm(ptm, nullptr)

