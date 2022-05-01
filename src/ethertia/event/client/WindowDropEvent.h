//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_WINDOWDROPEVENT_H
#define ETHERTIA_WINDOWDROPEVENT_H

class WindowDropEvent
{
    uint count;
    const char** paths;

public:
    WindowDropEvent(const uint& count, const char** paths) : count(count), paths(paths) {}

    uint getCount() const { return count; }

    const char* getPath(uint i) const { return paths[i]; }
};

#endif //ETHERTIA_WINDOWDROPEVENT_H
