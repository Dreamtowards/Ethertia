
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
		// TODO: Adaptive Increase Worker
		explicit thread_pool(uint32_t _NumWorkers = std::thread::hardware_concurrency());

		~thread_pool();

		#pragma region task

		// not use std::future, because need support immediate check state, no blocking. while std::future need blocking/wait to get state.

		enum task_state
		{
			created,
			running,
			completed,
			cancelled,
			faulted
		};

		template<typename _TResult>
		class base_task
		{
		public:
			_TResult& get_sync()
			{
				std::unique_lock<std::mutex> _lock(m_ResultLock);
				m_ResultNotification.wait(_lock, [this] { return is_completed(); });
				return m_Result;
			}

			_TResult& get()
			{
				assert(is_completed());
				return m_Result;
			}

			bool try_get(_TResult& val) const
			{
				if (!is_completed())
					return false;
				val = m_Result;
				return true;
			}

			task_state state() const { return m_State; }

			bool is_completed() const { return m_State == task_state::completed; }

			// [[nodiscard]] bool cancel();

		private:

			_TResult m_Result;

			task_state m_State = task_state::created;

			std::mutex m_ResultLock;
			std::condition_variable m_ResultNotification;

			friend class stdx::thread_pool;
		};

		template<typename _TResult>
		class task : public base_task<_TResult>
		{
		};

		template<>
		class task<void> : public base_task<int>
		{
		};

		#pragma endregion

		template<typename F, typename... Args>
		auto submit(F&& f, Args&&... args) -> std::shared_ptr<task<decltype(f(args...))>>
		{
			auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

			auto task_ = std::make_shared<thread_pool::task<decltype(f(args...))>>();

			{
				std::lock_guard<std::mutex> _lock(m_TasksLock);
				m_Tasks.emplace([task_, func]()
					{
						try
						{
							task_->m_State = task_state::running;
							if constexpr(std::is_void<decltype(f(args...))>::value)
							{
								func();
							}
							else
							{
								task_->m_Result = func();
							}
							task_->m_State = task_state::completed;
							task_->m_ResultNotification.notify_all();
						}
						catch (...)
						{
							task_->m_State = task_state::faulted;
						}
					});
			}
			m_TasksNotification.notify_one();

			return task_;
		}

		//template<typename T>
		//bool cancel(std::shared_ptr<task<T>> task)
		//{
		//	std::lock_guard<std::mutex> _lock(m_TasksLock);
		//}

		size_t num_threads() const { return m_WorkerThreads.size(); }
		size_t num_tasks() const { return m_Tasks.size(); }

		size_t num_working_threads() const { return m_NumThreadsWorking; }



	private:

		std::vector<std::thread> m_WorkerThreads;
		int m_NumThreadsWorking = 0;  // num threads that actual processing task.

		std::queue<std::function<void()>> m_Tasks;
		std::mutex m_TasksLock;
		std::condition_variable m_TasksNotification;

		bool m_Stop = false;

		void _WorkerProc();

	};


}
