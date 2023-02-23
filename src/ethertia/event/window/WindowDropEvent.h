//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_WINDOWDROPEVENT_H
#define ETHERTIA_WINDOWDROPEVENT_H

class WindowDropEvent
{
    int count;
    const char** paths;

public:
    WindowDropEvent(int count, const char** paths) : count(count), paths(paths) {}

    int getCount() const { return count; }

    const char* getPath(int i) const { return paths[i]; }
};

#endif //ETHERTIA_WINDOWDROPEVENT_H
