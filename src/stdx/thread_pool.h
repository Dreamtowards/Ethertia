
#pragma once

#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <future>
#include <functional>
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

		
		template<typename F, typename... Args>
		auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
		{
			auto task = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
				std::bind(std::forward<F>(f), std::forward<Args>(args)...)
			);

			submit([]()
				{
					(*task)();
				});
			//{
			//	std::lock_guard<std::mutex> _lock(m_TasksLock);
			//	m_Tasks.emplace([]()
			//		{
			//			(*task)();
			//		});
			//}
			//m_TasksNotification.notify_one();

			return task->get_future();
		}

		size_t num_threads() const { return m_WorkerThreads.size(); }
		size_t num_tasks() const { return m_Tasks.size(); }

	private:

		std::vector<std::thread> m_WorkerThreads;

		std::queue<func_t> m_Tasks;
		std::mutex m_TasksLock;
		std::condition_variable m_TasksNotification;

		bool m_Stop = false;

		void _WorkerProc();

	};
}
