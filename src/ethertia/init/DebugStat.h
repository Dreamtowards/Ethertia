//
// Created by Dreamtowards on 2023/3/12.
//

#ifndef ETHERTIA_DEBUGSTAT_H
#define ETHERTIA_DEBUGSTAT_H

struct DebugStat
{
public:

    inline static int dbg_ChunksSaving = 0;
    inline static const char* dbg_ChunkProvideState = nullptr;  // "gen" or "load"
    inline static int dbg_NumChunksMeshInvalid = 0;

    inline static bool dbg_PauseThread_ChunkLoadGenSave = false;
    inline static bool dbg_PauseThread_ChunkMeshing = false;


    inline static bool dbg_EntityGeo = false;
    inline static bool dbg_HitPointEntityGeo = false;
    inline static bool dbg_RenderedEntityAABB = false;
    inline static bool dbg_NoVegetable = false;



    inline static int dbg_FPS = 0;

    // call every frame. return true: once per second. update fps value.
    static bool _fps_frame(double curr_sec) {
        static int fps_sec = 0;
        ++fps_sec;

        static double last_fps_time = 0;
        if (last_fps_time == 0) {
            last_fps_time = curr_sec;
        } else if (curr_sec - last_fps_time >= 1.0f) {
            dbg_FPS = fps_sec;
            fps_sec = 0;
            last_fps_time = curr_sec;
            return true;
        }
        return false;
    }
};

using Dbg = DebugStat;

#endif //ETHERTIA_DEBUGSTAT_H
