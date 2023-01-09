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

#define LOCK_GUARD(x) std::lock_guard<std::mutex> _guard(x);

class Scheduler
{
    using TaskFunc = std::function<void()>;

    std::deque<TaskFunc> m_Tasks;
    std::mutex m_LockTasks;

    bool m_Stopped = false;

public:
    std::thread::id m_ThreadId;

    Scheduler() {}

    ~Scheduler() {
        m_Stopped = true;
    }

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
                TaskFunc task;
                {
                    LOCK_GUARD(m_LockTasks);  // Lock too long.
                    task = m_Tasks.front();
                    m_Tasks.pop_front();
                }

                task();

                // m_Tasks.pop_front();  // pop after executed. or might destroy lambda captured variables.
                numTasks++;
            }

            if (duration > timeout) {  // Log::info("Break. done {}, over {}s.", numTasks, duration-timeout);
                break;
            }
        }

        return numTasks;
    }

    const std::deque<std::function<void()>>& getTasks() {
        LOCK_GUARD(m_LockTasks);
        return m_Tasks;
    }

    void addTask(const std::function<void()>& task) {
        LOCK_GUARD(m_LockTasks);
        m_Tasks.push_back(task);
    }

    bool inThread() {
        return std::this_thread::get_id() == m_ThreadId;
    }

    void initThread()
    {
        new std::thread([this]()
        {
            m_ThreadId = std::this_thread::get_id();
            // Log::info("Async Tasks thread is ready.", workerName);

            while (!m_Stopped)
            {
                int n = processTasks(1);

                if (n == 0) {
                    Timer::sleep_for(1);  // prevents high frequency queue query while no task.
                }
            }
        });
    }


};

#endif //ETHERTIA_SCHEDULER_H
