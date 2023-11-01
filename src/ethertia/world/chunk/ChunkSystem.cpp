

#include "ChunkSystem.h"
#include <ethertia/world/World.h>
#include <ethertia/world/Entity.h>
#include <ethertia/world/Physics.h>

#include <ethertia/Ethertia.h>  // thread_pool, viewpos

#include <ethertia/util/Assert.h>
#include <ethertia/util/Math.h>
#include <ethertia/util/Log.h>
#include <ethertia/util/Loader.h>
#include <ethertia/util/BenchmarkTimer.h>

#include <ethertia/world/chunk/meshgen/MeshGen.h>

#include <ethertia/init/MaterialMeshes.h>  // tmp test

#include <stdx/stdx.h>

#include <ethertia/world/chunk/gen/ChunkGenerator.h>



ChunkSystem::ChunkSystem(World* world) : m_World(world) 
{

}

ChunkSystem::~ChunkSystem()
{

}






const Cell& ChunkSystem::GetCell(glm::ivec3 p)
{
    auto chunk = GetChunk(Chunk::ChunkPos(p));

    if (!chunk)
        return Cell::Nil();

    return chunk->LocalCell(Chunk::LocalPos(p));
}



std::shared_ptr<Chunk> ChunkSystem::_ProvideChunk(glm::ivec3 chunkpos)
{
    ET_ASSERT(Chunk::IsChunkPos(chunkpos));
    ET_ASSERT(GetChunk(chunkpos) == nullptr);

    auto chunk = std::make_shared<Chunk>(m_World, chunkpos);

    bool load = false;// m_ChunkLoader->LoadChunk(chunk);

    if (!load)
    {
        ChunkGenerator::GenerateChunk(*chunk.get());  // m_ChunkGenerator->GenerateChunk(chunk);
    }

    return chunk;
}


//void ChunkSystem::QueueLoad(glm::vec3 chunkpos)
//{
//    ET_ASSERT(Chunk::IsChunkPos(chunkpos));
//    ET_ASSERT(GetChunk(chunkpos) == nullptr);
//    ET_ASSERT(!m_ChunksLoading.contains(chunkpos));
//
//    auto& threadpool = Ethertia::GetThreadPool();
//
//    auto task = threadpool.submit([this, chunkpos]()
//        {
//            return _ProvideChunk(chunkpos);
//        });
//
//    auto [it, succ] = m_ChunksLoading.try_emplace(chunkpos, task);
//
//    ET_ASSERT(succ);  // make sure no override
//}

static bool _IsChunkInLoadRange(glm::ivec3 chunkpos, glm::ivec3 viewer_chunkpos, glm::ivec3 viewer_loaddist)
{
    glm::ivec3 dist = glm::abs(chunkpos - viewer_chunkpos);
    return ET_VEC3_CMP(dist, <=, viewer_loaddist, &&);
}

void ChunkSystem::_UpdateChunkLoadAndUnload(glm::vec3 viewpos, glm::ivec3 loaddist)
{
    auto& threadpool = Ethertia::GetThreadPool();

    // todo: Recursive Octree Load.
	// Check which chunks Need to be Load

    glm::ivec3 viewer_chunkpos = Chunk::ChunkPos(viewpos);


    // Async Generate/Load chunk
    {
        ET_PROFILE("DetLoad");
        auto _lock = LockRead();

        glm::ivec3 extent = loaddist;
        for (int dy = -extent.y; dy <= extent.y; ++dy)
        { 
            for (int dx = -extent.x; dx <= extent.x; ++dx)
            {
                for (int dz = -extent.z; dz <= extent.z; ++dz)
                {
                    if (m_ChunksLoading.size() >= cfg_ChunkLoadingMaxConcurrent)
                        break;

                    glm::ivec3 chunkpos = glm::ivec3(dx,dy,dz) * 16 + viewer_chunkpos;

                    if (m_Chunks.contains(chunkpos) || m_ChunksLoading.contains(chunkpos))
                        continue;

                    auto task = threadpool.submit([this, chunkpos]()
                        {
                            return _ProvideChunk(chunkpos);
                        });

                    auto [it, succ] = m_ChunksLoading.try_emplace(chunkpos, task);

                    ET_ASSERT(succ);  // make sure no override

                }
            }
        }
    }

    // Add AsyncGenerated Chunk to World
    static std::vector<glm::ivec3> _TmpChunksBatchErase;  // static: avoid dynamic heap alloc.

    _TmpChunksBatchErase.clear();
    {
        ET_PROFILE("Load");

        for (auto it : m_ChunksLoading)
        {
            glm::ivec3 chunkpos = it.first;
            auto task = it.second;

            if (!task->is_completed())
                continue;

            ET_ASSERT(GetChunk(chunkpos) == nullptr);
            auto chunk = task->get();
            {
                auto _lock = LockWrite();
                m_Chunks[chunkpos] = chunk;
            }

            chunk->_LoadToWorld();


            _TmpChunksBatchErase.push_back(chunkpos);
        }
        for (auto cp : _TmpChunksBatchErase)
        {
            m_ChunksLoading.erase(cp);
        }
        if (_TmpChunksBatchErase.size())
        {
            Log::info("{} Chunks Loaded", _TmpChunksBatchErase.size());
        }
    }



    // Detect Unload Chunks
    static int m_ChunksUnloadingMaxBatch = 10;

    _TmpChunksBatchErase.clear();
    {
        ET_PROFILE("Unload");
        ForChunks([&](auto chunkpos, auto& chunk) {

            if (_TmpChunksBatchErase.size() > m_ChunksUnloadingMaxBatch)
                return false;
            if (_IsChunkInLoadRange(chunkpos, viewer_chunkpos, loaddist * 16))
                return true;

            // UnloadChunk

            _TmpChunksBatchErase.push_back(chunkpos);
            return true;
        });

        auto _lock = LockWrite();
        for (auto cp : _TmpChunksBatchErase)
        {
            m_Chunks.erase(cp);
        }
    }
    if (_TmpChunksBatchErase.size())
    {
        Log::info("{} Chunks Unloaded", _TmpChunksBatchErase.size());
    }




    // Detect ChunkMeshing

    {
        ET_PROFILE("DetMesh");
        ForChunks([&](auto chunkpos, Chunk& chunk) {

            if (m_ChunksMeshing.size() >= cfg_ChunkMeshingMaxConcurrent)
                return false;
            if (!chunk.m_NeedRebuildMesh)  
                return true;
            if (chunk.m_TimeInhabited < 0.4f)  // later mesh. wait neibiough chunks loaded.
                return true;

            // Queue Chunk MeshGen

            chunk.m_NeedRebuildMesh = false;

            VertexData* vtx = chunk.entity.GetComponent<ChunkComponent>().VertexData;


            auto task = threadpool.submit([this, chunkpos, vtx]() {
                auto pChunk = GetChunk(chunkpos);

                //Log::info("MeshGen VBuf Pool Size: {} acquired, {} remained", g_VertexBufPool.num_aquired(), g_VertexBufPool.num_remained());

                VertexData* tmp = g_MeshGen_VertexBufPool.acquire();
                tmp->Clear();


                {
                    //BENCHMARK_TIMER;

                    // Bench x64-Debug: 0vtx~=3-4.5ms, 190vtx=14ms, 1000vtx=21ms, 2000vtx~=25-33ms, 3000vtx=50ms, 4000vtx~=80ms
                    MeshGen::GenerateMesh(*pChunk.get(), *tmp);

                    //Log::info("MeshGen {} vertices \1", tmp->VertexCount());
                }

                vtx->Clear();
                if (!tmp->empty())
                {
                    //BENCHMARK_TIMER;

                    // always ~80% vertices reduced, and ~70% size reduced (plus indices size).  
                    // Bench x64-Debug: 200vtx->50 = 1.1ms, 1200vtx->250 = 1.9ms, 2000vtx->400 = 2.5ms, | ex: 22000vtx->3500 30ms-120ms, 100,000vtx->7500 350ms
                    VertexData::MakeIndexed(tmp, vtx);  // for supports TriangleMesh Load for PhysX

                    //Log::info("MeshGenIndex vtx from {} to {}. compression ratio {} vtx, {} total_size\1", tmp->Vertices.size(), vtx->Vertices.size(), vtx->Vertices.size() / (float)tmp->Vertices.size(), (vtx->idx_size() + vtx->vtx_size()) / (float)(tmp->idx_size() + tmp->vtx_size()) );
                }

                g_MeshGen_VertexBufPool.release(tmp);
                //delete tmp;  // todo: use stdx::object_pool instead of new/delete

                return pChunk;
            });

            auto [it, succ] = m_ChunksMeshing.try_emplace(chunkpos, task);

            ET_ASSERT(succ);
            return true;
        });
    }


    // Load/Upload ChunkMesh

    _TmpChunksBatchErase.clear();
    {
        ET_PROFILE("LoadMesh");

        for (auto it : m_ChunksMeshing)
        {
            glm::ivec3 chunkpos = it.first;
            auto task = it.second;

            if (!task->is_completed())
                continue;

            auto chunk = task->get();
            Entity& entity = chunk->entity;

            // the entity may already been Removed from World (ChunkUnload)
            if (!m_World->registry().valid(entity))
            {
                // Release Resource: VertexBuffer
            }
            else
            {
                // Update MeshRender
                auto& comp = entity.GetComponent<ChunkComponent>();

                // Delete Old vkx::VertexBuffer
                if (comp.VertexBuffer)
                {
                    vkx::ctx().Device.waitIdle();
                    delete comp.VertexBuffer;
                    comp.VertexBuffer = nullptr;
                }

                VertexData& indexed = *comp.VertexData;
                if (!indexed.empty())
                {
                    // Upload VertexData to GPU.
                    comp.VertexBuffer = Loader::LoadVertexData(&indexed);

                    // Update Physics Shape TriangleMesh
                    PxRigidStatic& rigid = *comp.RigidStatic;

                    // Delete Old Shapes
                    Physics::ClearShapes(rigid);

                    // Attach New Shapes
                    ETPX_CTX;

                    // Bench CreateTriangleMesh x64-Debug: 270vtx=0.8ms, 1200vtx=1.3ms, 2100vtx=1.5ms, 3200vtx=2.2ms, 4000vtx=3.4ms
                    PxShape* shape = PhysX.createShape(PxTriangleMeshGeometry(Physics::CreateTriangleMesh(indexed)), *Physics::dbg_DefaultMaterial);
                    //Log::info("Create PhysX TriangleMesh of {} vertices \1", indexed.VertexCount());
                    
                    rigid.attachShape(*shape);
                    shape->release();
                }
                else
                {
                    // Set Empty GPU Mesh & Physics Shape

                }
                //delete compRenderMesh.VertexData;
                //compRenderMesh.VertexData = nullptr;
            }

        
            _TmpChunksBatchErase.push_back(chunkpos);
        }

        for (auto cp : _TmpChunksBatchErase)
        {
            m_ChunksMeshing.erase(cp);
        }
    }
    if (_TmpChunksBatchErase.size())
    {
        Log::info("{} Chunks Meshed", _TmpChunksBatchErase.size());
    }



    //for (int dx = -ex.x; dx <= ex.x; ++dx)
    //{
    //    for (int dy = -ex.y; dy <= ex.y; ++dy)
    //    {
    //        for (int dz = -ex.z; dz <= ex.z; ++dz)
    //        {
    //            glm::ivec3 chunkpos = glm::ivec3(dx, dy, dz) * 16 + viewer_chunkpos;
    //
    //            if (m_ChunksLoading.size() > m_ChunksLoadingMaxConcurrent)
    //                break;
    //            if (GetChunk(chunkpos) || m_ChunksLoading.contains(chunkpos))
    //                continue;
    //
    //            QueueLoad(chunkpos);
    //
    //        }
    //    }
    //}
}



static void _UpdateChunksMeshing()
{

}


void ChunkSystem::OnTick(float dt)
{
    ForChunks([&](auto cp, Chunk& chunk) {

        chunk.m_TimeInhabited += dt;

        return true;
    });

    // temp.
    m_ChunkLoadCenter = Chunk::ChunkPos(Ethertia::GetPlayer().GetTransform().position());

    _UpdateChunkLoadAndUnload(m_ChunkLoadCenter, { m_TmpLoadDistance.x, m_TmpLoadDistance.y, m_TmpLoadDistance.x });

}