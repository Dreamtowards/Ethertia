//
// Created by Dreamtowards on 2022/4/27.
//

#ifndef ETHERTIA_SCHEDULER_H
#define ETHERTIA_SCHEDULER_H

#include <vector>
#include <functional>
#include <deque>
#include <thread>

class Scheduler
{
    std::deque<std::function<void()>> m_Tasks;
    std::thread::id m_ThreadId{};

    // std::mutex lock;  // needs?

public:
    explicit Scheduler(std::thread::id th = std::this_thread::get_id()) : m_ThreadId(th) {}

    void processTasks()
    {
        while (!m_Tasks.empty())
        {
            auto& task = m_Tasks.front();

            task();

            m_Tasks.pop_front();
        }
    }

    void exec(const std::function<void()>& task) {
        if (m_ThreadId == std::this_thread::get_id()) {  // execute immediately.
            task();
        } else {  // delay.
            m_Tasks.push_back(task);
        }
    }

};

#endif //ETHERTIA_SCHEDULER_H
