
#include "thread_pool.h"



void stdx::thread_pool::_WorkerProc()
{
	while (true)
	{
		func_t task;
		{
			std::unique_lock<std::mutex> _lock(m_TasksLock);

			while (m_Tasks.empty())
			{
				if (m_Stop) 
					return;  // Only return when queue is empty and should_stop
				
                // Wait until notified
				m_TasksNotification.wait(_lock);
			}

			task = std::move(m_Tasks.front());
			m_Tasks.pop();
		}

		task();
	}
}

#include <iostream>
stdx::thread_pool::thread_pool(uint32_t _NumWorkers)
{
	m_WorkerThreads.reserve(_NumWorkers);

	for (uint32_t i = 0; i < _NumWorkers; ++i)
	{
		m_WorkerThreads.emplace_back(&stdx::thread_pool::_WorkerProc, this);
	}

	std::cout << "ThreadPool Init " << _NumWorkers << "\n";
}


stdx::thread_pool::~thread_pool()
{
	{
		std::lock_guard<std::mutex> lock(m_TasksLock);
		m_Stop = true;
	}

	m_TasksNotification.notify_all();

	for (std::thread& worker : m_WorkerThreads) 
	{
		if (worker.joinable()) 
		{
			worker.join();
		}
	}
}

void stdx::thread_pool::submit(const func_t& func)
{
	{
		std::unique_lock<std::mutex> _lock(m_TasksLock);

		m_Tasks.emplace(func);
	}
	m_TasksNotification.notify_one();
}

