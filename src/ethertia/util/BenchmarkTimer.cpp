
#include "BenchmarkTimer.h"

#include <chrono>
#include <iostream>
#include <format>



BenchmarkTimer::BenchmarkTimer(double* _ptm, const char* _msg) :
	m_TimeBegin(std::chrono::high_resolution_clock::now()),
	m_pTimeAppend(_ptm),
	m_EndMsg(_msg)
{
}

BenchmarkTimer::~BenchmarkTimer()
{
	if (!m_Stopped)
	{
		Stop();
	}
}

void BenchmarkTimer::Stop()
{
	m_Stopped = true;
	std::chrono::duration<double, std::milli> dura = std::chrono::high_resolution_clock::now() - m_TimeBegin;
	double dura_ms = dura.count();

	if (m_pTimeAppend)
	{
		*m_pTimeAppend += dura_ms * 0.001;
	}

	if (m_EndMsg)
	{
		std::string time_str;

		if (dura_ms > 1000)
		{
			time_str = std::to_string(dura_ms * 0.001) + "s";
		}
		else
		{
			time_str = std::to_string(dura_ms) + "ms";
		}

		std::cout << std::vformat(m_EndMsg, std::make_format_args(time_str));
		std::cout.flush();  // for what?
	}

}
