//
// Created by Dreamtowards on 2023/1/7.
//

#ifndef ETHERTIA_CHUNKPROCSTAT_H
#define ETHERTIA_CHUNKPROCSTAT_H

#include <ethertia/util/Strings.h>

// replaced by Profiler.
//class ChunkProcStat
//{
//public:
//    struct TimeAndNum {
//        float time;  // sum time
//        int   num;  // num call
//
//        TimeAndNum() : time(0), num(0) {}
//
//        std::string str() {
//            return Strings::fmt("({} {}ms, avg {}ms)",
//                                num,
//                                time * 1000,
//                                time / num * 1000);
//        }
//    };
//
//    inline static ChunkProcStat::TimeAndNum
//                             GEN,  // Chunk Gen
//                             GEN_POP,  // Chunk Populate
//                             MESH, // Chunk Mesh Gen
//                             EMIT, // Chunk Mesh Upload
//                             LOAD, // Load from disk
//                             SAVE; // Save to disk
//
//    static void reset() {
//        GEN = {};
//        MESH = {};
//        EMIT = {};
//        LOAD = {};
//        SAVE = {};
//    }
//
//};

#endif //ETHERTIA_CHUNKPROCSTAT_H
