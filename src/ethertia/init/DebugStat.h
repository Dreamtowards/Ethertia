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

};

using Dbg = DebugStat;

#endif //ETHERTIA_DEBUGSTAT_H
