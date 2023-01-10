//
// Created by Dreamtowards on 2022/12/10.
//

#ifndef ETHERTIA_CHUNKMESHPROC_H
#define ETHERTIA_CHUNKMESHPROC_H

#include <ethertia/render/chunk/SurfaceNetsMeshGen.h>
#include <ethertia/render/chunk/BlockyMeshGen.h>
#include <ethertia/render/chunk/proc/ChunkProcStat.h>

class ChunkMeshProc
{
public:
    using vec3 = glm::vec3;

    inline static bool dbg_ChunkUnload = true;

    inline static Chunk* g_Processing = nullptr;

    static void initThread()
    {
        new std::thread([]()
        {
            Log::info("ChunkMeshProc thread is ready.");

            while (Ethertia::isRunning())
            {
                World* world = Ethertia::getWorld();
                if (!world) {
                    Timer::sleep_for(1);
                    continue;
                }

                Chunk* chunk = findNearestMeshInvalidChunk(world, Ethertia::getCamera()->position, RenderEngine::viewDistance);
                g_Processing = chunk;

                if (chunk)
                {
                    meshChunk_Upload(chunk);
                }

                g_Processing = nullptr;
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
        BENCHMARK_TIMER(&ChunkProcStat::MESH.time, nullptr);  ChunkProcStat::MESH.num++;
        chunk->m_Meshing = true;

        chunk->m_MeshInvalid = false;

        VertexBuffer* vbufTerrain = new VertexBuffer();

        VertexBuffer* vbufVegetable = new VertexBuffer();


//        VertexBuffer* vbufWater = new VertexBuffer();

        {
//        BenchmarkTimer _tm;
//        Log::info("Chunk MeshGen. \1");

//        vbuf = MarchingCubesMeshGen::genMesh(chunk);

            std::vector<glm::vec3> grass_fp;

            SurfaceNetsMeshGen::contouring(chunk, vbufTerrain, grass_fp);

//            BlockyMeshGen::gen(chunk, vbufVegetable);
//
//            BlockyMeshGen::genGrasses(vbufVegetable, grass_fp);

// CNS BUG: 在区块Unloaded后，可能其Mesh刚刚开始。结果采集到被删除的区块 造成错误
// 如何修复？1. try-catch 但不够纯粹。2. delete/unload 的时候wait 但又繁杂了
        }

        checkNonNaN(vbufTerrain->positions.data(), vbufTerrain->vertexCount()*3);

        vbufTerrain->normals.reserve(vbufTerrain->vertexCount() * 3);
        VertexProcess::gen_avgnorm(vbufTerrain->vertexCount(), vbufTerrain->positions.data(), vbufTerrain->vertexCount(), vbufTerrain->normals.data());

        vbufVegetable->normals.reserve(vbufVegetable->vertexCount() * 3);
        float* vegnorms = vbufVegetable->normals.data();
        for (int n_i = 0; n_i < vbufVegetable->vertexCount(); ++n_i) {
            Mth::vec3out(glm::vec3(0, 1, 0), &vegnorms[n_i*3]);
        }
//        VertexProcess::othonorm(vbufVegetable->vertexCount(), vbufVegetable->positions.data(), vbufVegetable->normals.data(), false);


//        Loader::saveOBJ("test_chunk.obj", vbufTerrain->vertexCount(), vbufTerrain->positions.data());


        btBvhTriangleMeshShape* meshTerrain =
            vbufTerrain->vertexCount()   == 0 ? nullptr :
            EntityMesh::createMeshShape(vbufTerrain->vertexCount(), vbufTerrain->positions.data());

        btBvhTriangleMeshShape* meshVegetable =
            vbufVegetable->vertexCount() == 0 ? nullptr :
            EntityMesh::createMeshShape(vbufVegetable->vertexCount(), vbufVegetable->positions.data());

        // Dont upload/to be render if Current and Previous Mesh is Empty.
        Ethertia::getScheduler()->addTask([chunk, vbufTerrain, vbufVegetable, meshTerrain, meshVegetable]() {
            BenchmarkTimer _tm(&ChunkProcStat::EMIT.time, nullptr);  ++ChunkProcStat::EMIT.num;

            // 区块可能已经被删除了

            // 移除 CancelTask。重复代码，而且，不解决问题
            // 当初添加Cancel的时候，就是因为 卸载世界后 不需要再执行 不想影响到其他世界
            // 但是并不解决问题：当某区块被卸载，其实也应该调用对应的Cancel 防止执行。但很明显不支持这种灵活度。
            // 而且有重复代码

//            if (chunk->m_World) {
                chunk->m_MeshTerrain->setMesh(meshTerrain);
                chunk->m_MeshTerrain->updateModel(Loader::loadModel(vbufTerrain));

                chunk->m_MeshVegetable->setMesh(meshVegetable);
                chunk->m_MeshVegetable->updateModel(Loader::loadModel(vbufVegetable));
//            } else {
//                delete meshTerrain;
//                delete meshVegetable;
//            }
            chunk->m_Meshing = false;

            delete vbufTerrain;
            delete vbufVegetable;
        }, [=](){
            chunk->m_Meshing = false;
            delete vbufTerrain;
            delete vbufVegetable;
            delete meshTerrain;
            delete meshVegetable;
        }, -1 - (int)glm::length2(Ethertia::getCamera()->position - chunk->position));
        // priority -1: after addEntity() to the world.
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
        float  minDist = FLT_MAX;

        walkViewDistanceChunks(viewDistance, [&](vec3 rel_chunkpos)
        {
            float dist = glm::length2(rel_chunkpos);
            Chunk* chunk = world->getLoadedChunk(origin_chunkpos + rel_chunkpos);
            if (dist < minDist && chunk && chunk->m_MeshInvalid) {
                minDist = dist;
                nearest = chunk;
            }
        });

        return nearest;
    }


};

#endif //ETHERTIA_CHUNKMESHPROC_H
