
#pragma once

#include <queue>
#include <mutex>

#include <cassert>

namespace stdx
{

	template<typename T>
	class object_pool
	{
		std::queue<T*> m_Pool;
		std::mutex m_PoolLock;

		int m_NumAquired = 0;
		int m_NumRemained = 0;
	public:

		object_pool() = default;
		
		~object_pool()
		{
			assert(m_NumAquired == 0);

			std::lock_guard<std::mutex> _lock(m_PoolLock);

			while (!m_Pool.empty())
			{
				delete m_Pool.front();
				m_Pool.pop();
			}
		}

		T* acquire()
		{
			std::lock_guard<std::mutex> _lock(m_PoolLock);
			++m_NumAquired;
			if (m_Pool.empty())
			{
				T* obj = new T();
				return obj;
			}
			--m_NumRemained;
			T* obj = m_Pool.front();
			m_Pool.pop();
			return obj;
		}

		void release(T* obj)
		{
			std::lock_guard<std::mutex> _lock(m_PoolLock);
			--m_NumAquired;
			++m_NumRemained;
			m_Pool.push(obj);
		}

		int num_aquired() const { return m_NumAquired; }

		int num_remained() const { return m_NumRemained; }

	};

}