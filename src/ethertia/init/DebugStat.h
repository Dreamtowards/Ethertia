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

    inline static bool  dbg_PauseWorldRender = false,
            dbg_NoChunkSave = true,
            dbg_NoChunkLoad = true;


    inline static int dbg_NumEntityRendered = 0;

    inline static glm::vec3 dbg_WorldSunColor = {1,1,1};
    inline static float dbg_WorldSunColorBrightnessMul = 5;
    inline static glm::vec3 dbg_ShaderDbgColor = {0.000f, 0.397f, 0.684f};


    inline static bool dbg_EntityGeo = false;
    inline static bool dbg_HitPointEntityGeo = false;
    inline static bool dbg_RenderedEntityAABB = false;
    inline static bool dbg_NoVegetable = false;


    // GUI Render
    inline static bool dbg_TextInfo = false,
                       dbg_WorldBasis = false,
                       dbg_ViewBasis = false,
                       dbg_ViewGizmo = false,
                       dbg_MeshingChunksAABB = false,   // draw AABB for Chunks that is meshing. RED=Meshing, DARK_RED=WaitTobeMesh/InvalidMesh.
                       dbg_ChunkMeshedCounter = false;  // draw a number in every chunk shows that chunk had been Meshed how many times.

    inline static int dbg_WorldHintGrids = 10;

    inline static Profiler dbgProf_ChunkGen;
    inline static Profiler dbgProf_ChunkMeshGen;

    // Terrain Breaking.
    inline static float dbg_CurrCellBreakingFullTime = 0;


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
