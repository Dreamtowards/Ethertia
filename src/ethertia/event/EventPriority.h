//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_EVENTPRIORITY_H
#define ETHERTIA_EVENTPRIORITY_H

class EventPriority
{
public:
    static constexpr int LOWEST = 3;
    static constexpr int LOW = 4;
    static constexpr int NORMAL = 5;
    static constexpr int HIGH = 6;
    static constexpr int HIGHEST = 7;
    static constexpr int MONITOR = 8;
};

#endif //ETHERTIA_EVENTPRIORITY_H
