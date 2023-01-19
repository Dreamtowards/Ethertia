//
// Created by Dreamtowards on 2022/4/27.
//

#ifndef ETHERTIA_SCHEDULER_H
#define ETHERTIA_SCHEDULER_H

#include <vector>
#include <functional>
#include <deque>
#include <queue>
#include <thread>

#include <ethertia/util/BenchmarkTimer.h>
#include <ethertia/util/Log.h>

#define LOCK_GUARD(x) std::lock_guard<std::mutex> _guard(x);

class Scheduler
{
    using TaskFunc = std::function<void()>;
    struct Task {
        TaskFunc taskFunc;
        //TaskFunc cancelFunc;  // exec if force terminated the scheduler
        int priority = 0;
        std::function<bool()> condition;  // ture -> process. use for e.g. delay countdown task

        bool operator <(const Task& rhs) const {
            return priority < rhs.priority;
        }
    };

    std::priority_queue<Task> m_Tasks;
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
        int numTasksProcessed = 0;

        while (numTasks())  // numTasks() is locked, thread-safe.
        {
            {
                BenchmarkTimer _tm(&duration, nullptr);
                TaskFunc taskfunc;
                {
                    LOCK_GUARD(m_LockTasks);  // Lock too long.
                    const Task& task = m_Tasks.top();
                    if (!task.condition()) {
                        continue;
                    }
                    taskfunc = task.taskFunc;
                    m_Tasks.pop();
                }

                taskfunc();

                // m_Tasks.pop_front();  // pop after executed. or might destroy lambda captured variables.
                numTasksProcessed++;
            }

            if (duration > timeout) {  // Log::info("Break. done {}, over {}s.", numTasks, duration-timeout);
                break;
            }
        }

        return numTasksProcessed;
    }

    size_t numTasks() {
        LOCK_GUARD(m_LockTasks);
        return m_Tasks.size();
    }

    void addTask(const TaskFunc& taskfunc, int priority = 0, const std::function<bool()>& cond = [](){return true;}) {
        LOCK_GUARD(m_LockTasks);
        m_Tasks.push({
            taskfunc,
            priority,
            cond
        });
    }
    void addDelayTask(const TaskFunc& taskfunc, float delay);

//    void clearTasks() {
//        LOCK_GUARD(m_LockTasks);
//        while (!m_Tasks.empty()) {
//            const TaskFunc& cancelfunc = m_Tasks.top().cancelFunc;
//
//            cancelfunc();
//
//            m_Tasks.pop();
//        }
//    }

    [[nodiscard]] bool inThread() const {
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
                int n = processTasks(1.0f);

                if (n == 0) {
                    Timer::sleep_for(1);  // prevents high frequency queue query while no task.
                }
            }
        });
    }


};

#endif //ETHERTIA_SCHEDULER_H
