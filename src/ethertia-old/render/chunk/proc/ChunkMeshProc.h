//
// Created by Dreamtowards on 2022/12/10.
//

#ifndef ETHERTIA_CHUNKMESHPROC_H
#define ETHERTIA_CHUNKMESHPROC_H

#include <ethertia/render/chunk/SurfaceNetsMeshGen.h>
#include <ethertia/render/chunk/BlockyMeshGen.h>

#include <ethertia/init/DebugStat.h>
#include <ethertia/init/Settings.h>

class ChunkMeshProc
{
public:
    using vec3 = glm::vec3;

    // -1: To Stop Run (not stopped), 0: Stopped. 1: Running.
    // why not direct set 0 when want-stop? because sometimes we need wait-stopped.
    inline static int g_Running = 0;

    static void initThread()
    {
        new std::thread([]()
        {
            Log::info("ChunkMeshProc thread is ready.");

            while (true)
            {
                if (g_Running == -1 || g_Running == 0) {
                    g_Running = 0;
                    if (!Ethertia::isRunning()) { break; }
                    Timer::sleep_for(1);
                    continue;
                }
                if (Dbg::dbg_PauseThread_ChunkMeshing) {
                    Timer::sleep_for(1);
                    continue;
                }
                World* world = Ethertia::getWorld();
                PROFILE_X(Dbg::dbgProf_ChunkMeshGen, "MeshGen");

                Chunk* chunk = nullptr;
                {
                    PROFILE_X(Dbg::dbgProf_ChunkMeshGen, "Seek");
                    chunk = findNearestMeshInvalidChunk(world, Ethertia::getCamera().position, Settings::s_ViewDistance);
                }

                if (!chunk) {
                    Timer::sleep_for(1);
                } else
                {
                    meshChunk_Upload(chunk);
                }

            }
        });
    }

    static void checkNonNaN(float* begin, size_t n) {
        for (int i = 0; i < n; ++i) {
            float f = begin[i];
            assert(!std::isnan(f));
        }
    }

    static void meshChunk_Upload(Chunk* chunk) {
        //BENCHMARK_TIMER_VAL(&ChunkProcStat::MESH.time);  ChunkProcStat::MESH.num++;

        //BENCHMARK_TIMER_MSG("Chunk MeshGen {}\n");

        chunk->m_MeshingState = Chunk::MESHING;  // May Already Been Deleted.

        // todo: ObjectPool Priority(reserved size, hot-ness) Stack
//        g_VertBufPool.m_Cap = 10;  // keep memory hot.

        VertexData* vtxTerrain = new VertexData();
        VertexData* vtxVegetable = new VertexData();


        {
            PROFILE_X(Dbg::dbgProf_ChunkMeshGen, "Mesh");

//        vbuf = MarchingCubesMeshGen::genMesh(chunk);

            {
                PROFILE_X(Dbg::dbgProf_ChunkMeshGen, "Iso");
                SurfaceNetsMeshGen::contouring(chunk, vtxTerrain);
            }

            {
                PROFILE_X(Dbg::dbgProf_ChunkMeshGen, "Vegetable");
                BlockyMeshGen::gen(chunk, vtxVegetable, true);
            }

// CNS BUG: 在区块Unloaded后，可能其Mesh刚刚开始。结果采集到被删除的区块 造成错误
// 如何修复？1. try-catch 但不够纯粹。2. delete/unload 的时候wait 但又繁杂了
        }

        {
            PROFILE_X(Dbg::dbgProf_ChunkMeshGen, "Norm");

//            checkNonNaN((float*)vtxTerrain->data(), vtxTerrain->vertexCount() * 8);

//            assert(vbufTerrain->normals.size() == vbufTerrain->vertexCount() * 3);
//            vbufTerrain->normals.resize(vbufTerrain->vertexCount() * 3);
//            VertexProcess::othonorm(vbufTerrain->vertexCount(), vbufTerrain->positions.data(), vbufTerrain->normals.data());
//            VertexProcess::gen_avgnorm(vbufTerrain->vertexCount(), vbufTerrain->positions.data(), vbufTerrain->vertexCount(), vbufTerrain->normals.data());

            for (auto& vert : vtxVegetable->Vertices) {
                vert.norm = {0,1,0};
            }
        }

        {
            PROFILE_X(Dbg::dbgProf_ChunkMeshGen, "Blocky");

            // CNS. 在 Norm生成之后。因为这里会自带Norm 不需要别人处理。

            BlockyMeshGen::gen(chunk, vtxTerrain, false);
        }


        btBvhTriangleMeshShape* meshTerrain = nullptr;
        btBvhTriangleMeshShape* meshVegetable = nullptr;

        {
            PROFILE_X(Dbg::dbgProf_ChunkMeshGen, "Bvh");

            if (vtxTerrain->vertexCount()) {
                meshTerrain = EntityMesh::CreateMeshShape(vtxTerrain);
            }
            if (vtxVegetable->vertexCount()) {
                meshVegetable = EntityMesh::CreateMeshShape(vtxVegetable);
            }
        }

        // Dont upload/to be render if Current and Previous Mesh is Empty.
        Ethertia::getScheduler().addTask([chunk, vtxTerrain, vtxVegetable, meshTerrain, meshVegetable]() {

            if (Ethertia::getWorld() && chunk->m_World) {
                chunk->m_MeshTerrain->setMesh(meshTerrain);
                chunk->m_MeshTerrain->updateModel(vtxTerrain->vertexCount() ? Loader::LoadVertexData(vtxTerrain) : nullptr);

                chunk->m_MeshVegetable->setMesh(meshVegetable);
                chunk->m_MeshVegetable->updateModel(vtxVegetable->vertexCount() ? Loader::LoadVertexData(vtxVegetable) : nullptr);

            } else {
                delete meshTerrain;
                delete meshVegetable;
            }
            ++chunk->dbg_MeshCounter;

            chunk->m_MeshingState = Chunk::MESH_VALID;  // must clear MESHING state for delete chunk (unloadChunk)
//            vbufTerrain->clear();
//            vbufVegetable->clear();
//            g_VertBufPool.restore(vbufTerrain);
//            g_VertBufPool.restore(vbufVegetable);
            delete vtxTerrain;
            delete vtxVegetable;
        }, -1 - (int)dist2ChunkCam(chunk));
        // priority <= -1: after addEntity() to the world.
    }

    inline static float dist2ChunkCam(Chunk* c) {
        return glm::length2(Ethertia::getCamera().position - c->position);
    }

    static void walkViewDistanceChunks(int viewDistance, const std::function<void(vec3 rcp)>& fn) {
        int n = viewDistance;
        for (int dx = -n;dx <= n;dx++) {
            for (int dz = -n; dz <= n; dz++) {
                for (int dy = -n; dy <= n; dy++)
                {
                    fn(vec3(dx,dy,dz) * 16.0f);
                }
            }
        }
    }


    static Chunk* findNearestMeshInvalidChunk(World* world, vec3 center, int viewDistance) {
        vec3 origin_chunkpos = Chunk::chunkpos(center);

        Chunk* nearest = nullptr;
        float  min_dist = FLT_MAX;

        DebugStat::dbg_NumChunksMeshInvalid = 0;

        // for ChunkList or getChunkAt
        LOCK_GUARD(world->m_LockChunks);
        walkViewDistanceChunks(viewDistance, [&](vec3 rel_chunkpos)
        {
            float dist = glm::length2(rel_chunkpos);
            Chunk* chunk = world->getLoadedChunk(origin_chunkpos + rel_chunkpos, true);
            if (chunk && chunk->m_MeshingState == Chunk::MESH_INVALID) {
                ++DebugStat::dbg_NumChunksMeshInvalid;
                if (dist < min_dist) {
                    min_dist = dist;
                    nearest = chunk;
                }
            }

        });

        return nearest;
    }


};

#endif //ETHERTIA_CHUNKMESHPROC_H
