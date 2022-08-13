//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_WINDOWDROPEVENT_H
#define ETHERTIA_WINDOWDROPEVENT_H

class WindowDropEvent
{
    u32 count;
    const char** paths;

public:
    WindowDropEvent(const u32& count, const char** paths) : count(count), paths(paths) {}

    u32 getCount() const { return count; }

    const char* getPath(u32 i) const { return paths[i]; }
};

#endif //ETHERTIA_WINDOWDROPEVENT_H
