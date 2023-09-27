

#include "ChunkSystem.h"

#include <ethertia/Ethertia.h>  // thread_pool, viewpos

#include <ethertia/util/Assert.h>


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
    // todo: Recursive Octree Load.
    
	// Check which chunks Need to be Load

    glm::vec3 viewer_chunkpos = Chunk::ChunkPos(viewpos);

    int nH = (int)viewDistance.x;
    int nV = (int)viewDistance.y;
    for (int dx = -nH; dx <= nH; ++dx)
    {
        for (int dy = -nV; dy <= nV; ++dy)
        {
            for (int dz = -nH; dz <= nH; ++dz)
            {
                glm::vec3 chunkpos = glm::vec3(dx, dy, dz) * 16.0f + viewer_chunkpos;

                // if (m_LoadingChunks.size() > ChunksLoadingMaxConcurrent) break;
                if (GetChunk(chunkpos) || m_ChunksLoading.contains(chunkpos))
                    continue;


                

            }
        }
    }
}




void ChunkSystem::OnTick()
{
	_UpdateChunkLoadAndUnload({0,0,0}, {4, 2});


}



