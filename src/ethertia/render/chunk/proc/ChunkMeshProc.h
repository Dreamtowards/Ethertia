//
// Created by Dreamtowards on 2022/12/10.
//

#ifndef ETHERTIA_CHUNKMESHPROC_H
#define ETHERTIA_CHUNKMESHPROC_H

#include <ethertia/render/chunk/SurfaceNetsMeshGen.h>
#include <ethertia/render/chunk/BlockyMeshGen.h>

#include <ethertia/util/ObjectPool.h>
#include <ethertia/init/DebugStat.h>

class ChunkMeshProc
{
public:
    using vec3 = glm::vec3;

    inline static bool dbg_ChunkUnload = true;


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
                World* world = Ethertia::getWorld();
                PROFILE_X(gp_MeshGen, "MeshGen");

                Chunk* chunk = nullptr;
                {
                    PROFILE_X(gp_MeshGen, "Seek");
                    Timer::sleep_for(1);
                    chunk = findNearestMeshInvalidChunk(world, Ethertia::getCamera().position, RenderEngine::viewDistance);
                }

                if (chunk)
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

    inline static Profiler gp_MeshGen;

    inline static ObjectPool<VertexBuffer> g_VertBufPool;

    static void meshChunk_Upload(Chunk* chunk) {
        //BENCHMARK_TIMER_VAL(&ChunkProcStat::MESH.time);  ChunkProcStat::MESH.num++;

        //BENCHMARK_TIMER_MSG("Chunk MeshGen {}\n");

        chunk->m_MeshingState = Chunk::MESHING;  // May Already Been Deleted.

        // todo: ObjectPool Priority(reserved size, hot-ness) Stack
        g_VertBufPool.m_Cap = 10;  // keep memory hot.

        VertexBuffer* vbufTerrain = g_VertBufPool.acquire();

        VertexBuffer* vbufVegetable = g_VertBufPool.acquire();


        {
            PROFILE_X(gp_MeshGen, "Mesh");

//        vbuf = MarchingCubesMeshGen::genMesh(chunk);

            {
                PROFILE_X(gp_MeshGen, "Iso");
                SurfaceNetsMeshGen::contouring(chunk, vbufTerrain);
            }

            {
                PROFILE_X(gp_MeshGen, "Vegetable");
                BlockyMeshGen::gen(chunk, vbufVegetable, true);
            }

// CNS BUG: 在区块Unloaded后，可能其Mesh刚刚开始。结果采集到被删除的区块 造成错误
// 如何修复？1. try-catch 但不够纯粹。2. delete/unload 的时候wait 但又繁杂了
        }

        {
            PROFILE_X(gp_MeshGen, "Norm");

            checkNonNaN(vbufTerrain->positions.data(), vbufTerrain->vertexCount()*3);

            vbufTerrain->normals.resize(vbufTerrain->vertexCount() * 3);
            VertexProcess::gen_avgnorm(vbufTerrain->vertexCount(), vbufTerrain->positions.data(), vbufTerrain->vertexCount(), vbufTerrain->normals.data());

            vbufVegetable->normals.resize(vbufVegetable->vertexCount() * 3);
            VertexProcess::set_all_vec3(vbufVegetable->normals.data(), vbufVegetable->vertexCount(), {0,1,0});
        }

        {
            PROFILE_X(gp_MeshGen, "Blocky");

            // CNS. 在 Norm生成之后。因为这里会自带Norm 不需要别人处理。

            BlockyMeshGen::gen(chunk, vbufTerrain, false);
        }


        btBvhTriangleMeshShape* meshTerrain = nullptr;
        btBvhTriangleMeshShape* meshVegetable = nullptr;

        {
            PROFILE_X(gp_MeshGen, "Bvh");

            if (vbufTerrain->vertexCount()) {
                meshTerrain = EntityMesh::createMeshShape(vbufTerrain->vertexCount(), vbufTerrain->positions.data());
            }
            if (vbufVegetable->vertexCount()) {
                meshVegetable = EntityMesh::createMeshShape(vbufVegetable->vertexCount(), vbufVegetable->positions.data());
            }
        }

        // Dont upload/to be render if Current and Previous Mesh is Empty.
        Ethertia::getScheduler().addTask([chunk, vbufTerrain, vbufVegetable, meshTerrain, meshVegetable]() {

            if (Ethertia::getWorld() && chunk->m_World) {
                chunk->m_MeshTerrain->setMesh(meshTerrain);
                chunk->m_MeshTerrain->updateModel(Loader::loadModel(vbufTerrain));

                chunk->m_MeshVegetable->setMesh(meshVegetable);
                chunk->m_MeshVegetable->updateModel(Loader::loadModel(vbufVegetable));

            } else {
                delete meshTerrain;
                delete meshVegetable;
            }

            chunk->m_MeshingState = Chunk::MESH_VALID;  // must clear MESHING state for delete chunk (unloadChunk)
            vbufTerrain->clear();
            vbufVegetable->clear();
            g_VertBufPool.restore(vbufTerrain);
            g_VertBufPool.restore(vbufVegetable);
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
