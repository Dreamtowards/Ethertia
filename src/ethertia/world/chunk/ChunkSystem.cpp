

#include "ChunkSystem.h"

#include <ethertia/Ethertia.h>  // thread_pool, viewpos

#include <ethertia/util/Assert.h>
#include <ethertia/util/Math.h>


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


void ChunkSystem::QueueLoad(glm::vec3 chunkpos)
{
    ET_ASSERT(Chunk::IsChunkPos(chunkpos));
    ET_ASSERT(GetChunk(chunkpos) == nullptr);
    ET_ASSERT(!m_ChunksLoading.contains(chunkpos));

    auto& threadpool = Ethertia::GetThreadPool();

    auto task = threadpool.submit([this, chunkpos]()
        {
            return _ProvideChunk(chunkpos);
        });

    auto [it, succ] = m_ChunksLoading.try_emplace(chunkpos, task);

    ET_ASSERT(succ);  // make sure no override
}

void ChunkSystem::_UpdateChunkLoadAndUnload(glm::vec3 viewpos, glm::vec2 viewDistance)
{
    static int m_ChunksLoadingMaxConcurrent = 40;

    // todo: Recursive Octree Load.
	// Check which chunks Need to be Load

    glm::ivec3 viewer_chunkpos = Chunk::ChunkPos(viewpos);

    glm::ivec3 vd{ viewDistance.x, viewDistance.y, viewDistance.x };

    ET_BEGIN_ITER_REGION(vd, rp)
    {
        glm::ivec3 chunkpos = rp * 16 + viewer_chunkpos;

        if (m_ChunksLoading.size() > m_ChunksLoadingMaxConcurrent) 
            break;
        if (GetChunk(chunkpos) || m_ChunksLoading.contains(chunkpos))
            continue;

        QueueLoad(chunkpos);
    }
    ET_END_ITER_REGION

    



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




void ChunkSystem::OnTick()
{
	_UpdateChunkLoadAndUnload({0,0,0}, {4, 2});


}



