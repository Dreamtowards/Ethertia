

#include "ChunkSystem.h"

#include <ethertia/Ethertia.h>


static void _UpdateChunkLoadAndUnload(glm::vec3 viewpos, glm::vec2 viewDistance)
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
                if (GetChunk(chunkpos) || m_ChunksLoading.exists())
                    continue;

                std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>(m_World, chunkpos);

                auto& threadpool = Ethertia::GetThreadPool();

                threadpool.submit([]()
                    {
                        // GenerateChunk(chunk);
                    });
            }
        }
    }
}


void ChunkSystem::OnTick()
{
	_UpdateChunkLoadAndUnload({0,0,0});


}




std::shared_ptr<Chunk> GetChunk(glm::vec3 chunkpos)
{

}