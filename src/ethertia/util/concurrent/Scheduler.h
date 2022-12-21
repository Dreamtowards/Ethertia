//
// Created by Dreamtowards on 2022/4/27.
//

#ifndef ETHERTIA_SCHEDULER_H
#define ETHERTIA_SCHEDULER_H

#include <vector>
#include <functional>
#include <deque>
#include <thread>

#include <ethertia/util/BenchmarkTimer.h>
#include <ethertia/util/Log.h>

class Scheduler
{
    std::deque<std::function<void()>> m_Tasks;
//    std::thread::id m_ThreadId{}; std::thread::id th = std::this_thread::get_id()

    // std::mutex lock;  // needs?

public:
    Scheduler() {}

    /**
     * @param timeout process threshold in seconds. Inf is do all, 0 is do only one.
     */
    int processTasks(float timeout)
    {
        float duration = 0;
        int numTasks = 0;

        while (!m_Tasks.empty())
        {
            {
                BenchmarkTimer _tm(&duration, nullptr);
                auto& task = m_Tasks.front();

                task();

                m_Tasks.pop_front();  // pop after executed. or might destroy lambda captured variables.
                numTasks++;
            }

            if (duration > timeout) {  // Log::info("Break. done {}, over {}s.", numTasks, duration-timeout);
                break;
            }
        }

        return numTasks;
    }

    std::deque<std::function<void()>>& getTasks() {
        return m_Tasks;
    }

    void addTask(const std::function<void()>& task) {
        m_Tasks.push_back(task);
    }

//    void addTaskOrExec() {
//        if (m_ThreadId == std::this_thread::get_id()) {  // execute immediately.
//            task();
//        } else {  // delay.
//            addTask();
//        }
//    }

};

#endif //ETHERTIA_SCHEDULER_H
