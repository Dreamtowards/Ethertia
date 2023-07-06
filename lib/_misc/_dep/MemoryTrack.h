//
// Created by Dreamtowards on 2022/12/25.
//

#ifndef ETHERTIA_MEMORYTRACK_H
#define ETHERTIA_MEMORYTRACK_H

#include <cstdlib>

class MemoryTrack
{
public:

    inline static size_t g_MemoryAllocated = 0;
    inline static size_t g_MemoryFreed     = 0;

    static size_t g_MemoryPresent() {
        return g_MemoryAllocated - g_MemoryFreed;
    }
};

#endif //ETHERTIA_MEMORYTRACK_H
