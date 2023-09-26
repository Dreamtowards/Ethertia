
#include "thread_pool.h"



void stdx::thread_pool::_WorkerProc()
{
	while (true)
	{
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> _lock(m_TasksLock);

			while (m_Tasks.empty())
			{
				if (m_Stop) 
					return;  // only return when queue is empty and should_stop
				
                // wait until notified
				m_TasksNotification.wait(_lock);
			}

			task = std::move(m_Tasks.front());
			m_Tasks.pop();
			++m_NumThreadsWorking;
		}

		task();

		{
			std::unique_lock<std::mutex> _lock(m_TasksLock);  // performance issue?
			--m_NumThreadsWorking;  // del:should after task(), but put here for less-lock efficiency
		}
	}
}

stdx::thread_pool::thread_pool(uint32_t _NumWorkers)
{
	m_WorkerThreads.reserve(_NumWorkers);

	for (uint32_t i = 0; i < _NumWorkers; ++i)
	{
		m_WorkerThreads.emplace_back(&stdx::thread_pool::_WorkerProc, this);
	}
	// std::cout << "ThreadPool Init " << _NumWorkers << "\n";
}


stdx::thread_pool::~thread_pool()
{
	m_Stop = true;

	m_TasksNotification.notify_all();

	for (std::thread& worker : m_WorkerThreads) 
	{
		if (worker.joinable()) 
		{
			worker.join();
		}
	}
}