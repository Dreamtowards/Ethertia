

#include "ChunkSystem.h"
#include <ethertia/world/World.h>
#include <ethertia/world/Entity.h>
#include <ethertia/world/Physics.h>

#include <ethertia/Ethertia.h>  // thread_pool, viewpos

#include <ethertia/util/Assert.h>
#include <ethertia/util/Math.h>
#include <ethertia/util/Loader.h>

#include <ethertia/world/chunk/meshgen/MeshGen.h>

#include <ethertia/init/MaterialMeshes.h>  // tmp test

#include <stdx/stdx.h>

#define ET_CAST(type, obj) *reinterpret_cast<type*>(&obj)

std::shared_ptr<Chunk> ChunkSystem::_ProvideChunk(glm::vec3 chunkpos)
{
    ET_ASSERT(Chunk::IsChunkPos(chunkpos));
    ET_ASSERT(GetChunk(chunkpos) == nullptr);

    std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>(m_World, chunkpos);

    bool load = false;// m_ChunkLoader->LoadChunk(chunk);

    if (!load)
    {
        // m_ChunkGenerator->GenerateChunk(chunk);

        ChunkGenerator::GenerateChunk(*chunk.get());
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
    AABB::each(loaddist, [&](glm::ivec3 rp) {

        glm::ivec3 chunkpos = rp * 16 + viewer_chunkpos;

        if (m_ChunksLoading.size() >= cfg_ChunkLoadingMaxConcurrent)
            return false;
        if (GetChunk(chunkpos) || m_ChunksLoading.contains(chunkpos))
            return true;

        auto task = threadpool.submit([this, chunkpos]()
            {
                return _ProvideChunk(chunkpos);
            });

        auto [it, succ] = m_ChunksLoading.try_emplace(chunkpos, task);

        ET_ASSERT(succ);  // make sure no override

        return true;
    });

    // Add AsyncGenerated Chunk to World
    static std::vector<glm::ivec3> _TmpChunksBatchErase;  // static: avoid dynamic heap alloc.

    _TmpChunksBatchErase.clear();
    for (auto it : m_ChunksLoading)
    {
        glm::ivec3 chunkpos = it.first;
        auto task = it.second;

        if (!task->is_completed())
            continue;

        ET_ASSERT(GetChunk(chunkpos) == nullptr);
        auto chunk = task->get();
        m_Chunks[chunkpos] = chunk;

        chunk->m_NeedRebuildMesh = true;  // RequestRemesh

        Entity entity = m_World->CreateEntity();
        chunk->entity = entity;

        auto& trans = entity.GetTransform();
        trans.position() = chunkpos;

        auto& compRenderMesh = entity.AddComponent<MeshRenderComponent>();
        compRenderMesh.VertexData = new VertexData();

        auto& compRigidStatic = entity.AddComponent<RigidStaticComponent>();


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



    // Detect Unload Chunks
    static int m_ChunksUnloadingMaxBatch = 10;

    _TmpChunksBatchErase.clear();
    for (auto it : m_Chunks)
    {
        glm::ivec3 cp = it.first;

        if (_TmpChunksBatchErase.size() > m_ChunksUnloadingMaxBatch)
            break;
        if (_IsChunkInLoadRange(cp, viewer_chunkpos, loaddist * 16))
            continue;
        
        // UnloadChunk
        auto chunk = it.second;

        m_World->DestroyEntity(chunk->entity);

        _TmpChunksBatchErase.push_back(cp);
    }
    for (auto cp : _TmpChunksBatchErase)
    {
        m_Chunks.erase(cp);
    }
    if (_TmpChunksBatchErase.size())
    {
        Log::info("{} Chunks Unloaded", _TmpChunksBatchErase.size());
    }




    // Detect ChunkMeshing

    for (auto it : m_Chunks)
    {
        glm::ivec3 cp = it.first;
        auto chunk = it.second;

        if (m_ChunksMeshing.size() >= cfg_ChunkMeshingMaxConcurrent)
            break;
        if (!chunk->m_NeedRebuildMesh)
            continue;
        chunk->m_NeedRebuildMesh = false;

        VertexData* vtx = chunk->entity.GetComponent<MeshRenderComponent>().VertexData;
        vtx->Clear();

        auto task = threadpool.submit([chunk, vtx]() {


            MeshGen::GenerateMesh(*chunk.get(), *vtx);

            return chunk;
        });

        auto [it, succ] = m_ChunksMeshing.try_emplace(cp, task);
    }


    // Load/Upload ChunkMesh

    _TmpChunksBatchErase.clear();
    for (auto it : m_ChunksMeshing)
    {
        glm::ivec3 chunkpos = it.first;
        auto task = it.second;

        if (!task->is_completed())
            continue;

        auto chunk = task->get();
        Entity& entity = chunk->entity;

        {
            // Update MeshRender
            auto& compRenderMesh = entity.GetComponent<MeshRenderComponent>();

            // Delete Old vkx::VertexBuffer
            if (compRenderMesh.VertexBuffer)
            {
                vkx::ctx().Device.waitIdle();
                delete compRenderMesh.VertexBuffer;
                compRenderMesh.VertexBuffer = nullptr;
            }

            VertexData& vtx = *compRenderMesh.VertexData;
            if (!vtx.empty())
            {
                std::unique_ptr<VertexData> indexed(VertexData::MakeIndexed(&vtx));

                // Upload VertexData to GPU.
                compRenderMesh.VertexBuffer = Loader::LoadVertexData(indexed.get());

                // Update Physics TriangleMesh
                auto& compRigidStatic = entity.GetComponent<RigidStaticComponent>();

                auto [pts, tri] = indexed->ExportPoints();

                compRigidStatic.RigidStatic->detachShape();

                ETPX_CTX;
                PxShape* shape = PhysX.createShape(PxTriangleMeshGeometry(Physics::CreateTriangleMesh(pts, tri)), *Physics::dbg_DefaultMaterial);
                compRigidStatic.RigidStatic->attachShape(*shape);
                shape->release();
            }
            else
            {

            }
        }

        
        _TmpChunksBatchErase.push_back(chunkpos);
    }

    for (auto cp : _TmpChunksBatchErase)
    {
        m_ChunksMeshing.erase(cp);
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

#include <ethertia/init/DebugStat.h>

void ChunkSystem::OnTick()
{

    Dbg::dbg_IsChunkModifying = true;
    _UpdateChunkLoadAndUnload({ 0,0,0 }, { m_TmpLoadDistance.x, m_TmpLoadDistance.y, m_TmpLoadDistance.x });
    Dbg::dbg_IsChunkModifying = false;

}