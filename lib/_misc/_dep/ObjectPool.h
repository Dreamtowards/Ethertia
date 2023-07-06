//
// Created by Dreamtowards on 2023/2/22.
//

#ifndef ETHERTIA_OBJECTPOOL_H
#define ETHERTIA_OBJECTPOOL_H

#include <stack>

template<typename T>
class ObjectPool
{
public:

    std::mutex m_LockPool;

    std::stack<T*> m_IdlePool;

    int m_NumUsing = 0;

    int m_Cap = 5;

    T* acquire() {
        ++m_NumUsing;
        if (m_IdlePool.empty()) {
            if (m_NumUsing > m_Cap) {
                Timer::wait_for([&](){return m_NumUsing <= m_Cap;});
            }
            return new T();
        }
        LOCK_GUARD(m_LockPool);
        T* p = m_IdlePool.top();
        m_IdlePool.pop();
        return p;
    }

    void restore(T* obj) {
        --m_NumUsing;
        LOCK_GUARD(m_LockPool);
        m_IdlePool.push(obj);
    }

};

#endif //ETHERTIA_OBJECTPOOL_H
