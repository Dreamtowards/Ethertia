//
// Created by Dreamtowards on 2022/4/27.
//

#ifndef ETHERTIA_EXECUTOR_H
#define ETHERTIA_EXECUTOR_H

#include <vector>
#include <functional>
#include <deque>

class Executor
{
    std::deque<std::function<void()>> tasks;
    std::thread::id _thread;

    // std::mutex lock;  // needs?

public:
    explicit Executor(std::thread::id th) {
        _thread = th;
    }

    void processTasks()
    {
        while (!tasks.empty())
        {
            auto& task = tasks.front();

            task();

            tasks.pop_front();
        }
    }

    void exec(const std::function<void()>& task) {
        if (_thread == std::this_thread::get_id()) {  // execute immediately.
            task();
        } else {  // delay.
            tasks.push_back(task);
        }
    }

};

#endif //ETHERTIA_EXECUTOR_H
