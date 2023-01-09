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

    inline static bool g_Processing = false;

    static void initThread()
    {
        new std::thread([]()
        {
            Log::info("ChunkMeshProc thread is ready.");

            while (Ethertia::isRunning())
            {
                World* world = Ethertia::getWorld();
                Timer::sleep_for(1);
                if (!world) {
                    continue;
                }
                g_Processing = true;

                Chunk* chunk = findNearestMeshInvalidChunk(world, Ethertia::getCamera()->position, RenderEngine::viewDistance);

                if (chunk)
                {
                    meshChunk_Upload(chunk);
                }

                g_Processing = false;
            }
        });
    }


    static void meshChunk_Upload(Chunk* chunk) {
        BENCHMARK_TIMER(&ChunkProcStat::MESH.time, nullptr);  ChunkProcStat::MESH.num++;

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

            BlockyMeshGen::gen(chunk, vbufVegetable);

            BlockyMeshGen::genGrasses(vbufVegetable, grass_fp);


        }


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


        Ethertia::getScheduler()->addTask([chunk, vbufTerrain, vbufVegetable, meshTerrain, meshVegetable]() {
            BenchmarkTimer _tm(&ChunkProcStat::EMIT.time, nullptr);  ++ChunkProcStat::EMIT.num;

//            if (Ethertia::getWorld())  // task may defer to world unloaded. todo BUG: different world
//            {
//
//            }
            chunk->m_MeshTerrain->setMesh(meshTerrain);
            chunk->m_MeshTerrain->updateModel(Loader::loadModel(vbufTerrain));

            chunk->m_MeshVegetable->setMesh(meshVegetable);
            chunk->m_MeshVegetable->updateModel(Loader::loadModel(vbufVegetable));

            delete vbufTerrain;
            delete vbufVegetable;
        }, [=](){
            delete vbufTerrain;
            delete vbufVegetable;
            delete meshTerrain;
            delete meshVegetable;
        });
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
