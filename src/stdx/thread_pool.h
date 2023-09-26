
#pragma once

#include <thread>

#include <functional>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace stdx
{
	class thread_pool
	{
	public:
		using func_t = std::function<void()>;
		
		explicit thread_pool(uint32_t _NumWorkers = std::thread::hardware_concurrency());

		~thread_pool();

		void submit(const func_t& task);

	private:

		std::vector<std::thread> m_WorkerThreads;

		std::queue<func_t> m_Tasks;
		std::mutex m_TasksLock;
		std::condition_variable m_TasksNotification;

		bool m_Stop = false;

		void _WorkerProc();

	};
}
