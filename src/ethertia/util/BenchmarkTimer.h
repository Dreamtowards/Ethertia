//
// Created by Dreamtowards on 2022/4/30.
//

#pragma once

#include <chrono>
#include <iostream>
#include <format>

// Time Error: x64-Release: ~= 0.0001ms; x64-Debug ~= 0.003ms; (2133MHz RAM, E5V4 CPU)

// An execution time measurer, starting when the object is constructed, and ends when the done() function or destructor is called
class BenchmarkTimer
{
public:
	// @_tmr: a pointer to a number, If not nullptr, this number will be += accumulate the entire measurement time (in seconds) at the end of the measurement.
	//        this is useful when you want sum-up or get the measurement time.
	// @_msg: a message that will be print at the end of the measurement unless it's a nullptr. "{}" is the time e.g. "85.616320ms"
	BenchmarkTimer(double* _ptm = nullptr, const char* _msg = " in {}.\n") : m_pTimeAppend(_ptm), m_EndMsg(_msg) 
	{
		m_TimeBegin = std::chrono::high_resolution_clock::now();  // latest to set.
	}

	// there will auto call Stop();
	~BenchmarkTimer()
	{
		if (!m_Stopped)
		{
			Stop();
		}
	}

	// End of Measurement
	void Stop()
	{
		std::chrono::duration<double, std::milli> dura = std::chrono::high_resolution_clock::now() - m_TimeBegin;
		m_Stopped = true;
		double dura_ms = dura.count();

		if (m_pTimeAppend) {
			*m_pTimeAppend += dura_ms * 0.001;
		}

		if (m_EndMsg)
		{
			std::string time_str;
			if (dura_ms > 1000) {
				time_str = std::to_string(dura_ms * 0.001) + "s";
			} else {
				time_str = std::to_string(dura_ms) + "ms";
			}
			std::cout << std::vformat(m_EndMsg, std::make_format_args(time_str));
			// std::cout.flush();  // for what?
		}
	}

private:

	std::chrono::time_point<std::chrono::high_resolution_clock> m_TimeBegin;

	const char* m_EndMsg;
	double* m_pTimeAppend;  // f32 or f64?

	bool m_Stopped = false;
};

// simply with default measurement message.
#define BENCHMARK_TIMER BenchmarkTimer _tm

// with Custom Message
#define BENCHMARK_TIMER_MSG(msg) BenchmarkTimer _tm(nullptr, msg)

// pure Time Accumulator.
#define BENCHMARK_TIMER_TMR(ptm) BenchmarkTimer _tm(ptm, nullptr)

