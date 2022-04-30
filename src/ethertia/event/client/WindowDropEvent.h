//
// Created by Dreamtowards on 2022/4/30.
//

#ifndef ETHERTIA_WINDOWDROPEVENT_H
#define ETHERTIA_WINDOWDROPEVENT_H

#include <ccomplex>

class WindowDropEvent
{
    uint _count;
    const char** _paths;

public:
    WindowDropEvent(const uint& _count, const char** _paths) : _count(_count), _paths(_paths) {}

    uint count() const { return _count; }

    const char* path(uint i) const { return _paths[i]; }
};

#endif //ETHERTIA_WINDOWDROPEVENT_H
