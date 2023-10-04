
#pragma once

#include <memory>
#include <unordered_map>
#include <glm/vec3.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <stdx/thread_pool.h>

#include <ethertia/world/Chunk.h>
#include <ethertia/world/chunk/ChunkLoader.h>
#include <ethertia/world/chunk/gen/ChunkGenerator.h>

//#include <ethertia/world/chunk/SVO.h>

#define ET_LOCK_GUARD(mtx) std::lock_guard<std::mutex> _lock(mtx);

class World;


class ChunkSystem
{
public:

	ChunkSystem(World* world);
	~ChunkSystem();

	// todo: use SVO to get a chunk in O(logN) time.
	// get loaded chunk.
	// return nullptr if chunk at the chunkpos is not loaded
	std::shared_ptr<Chunk> GetChunk(glm::ivec3 chunkpos)
	{
		//ET_LOCK_GUARD(m_ChunksLock);
		auto it = m_Chunks.find(chunkpos);
		if (it == m_Chunks.end())
			return nullptr;
		return it->second;
	}

	const std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>>& GetChunks()
	{
		return m_Chunks;
	}



	void OnTick();


	//void QueueLoad(glm::vec3 chunkpos);

	void SaveChunk();

	void UnloadChunk(std::shared_ptr<Chunk> chunk);

	void QueueUnload(std::shared_ptr<Chunk> chunk);

	void QueueUnloadAll(std::shared_ptr<Chunk> chunk);


private:

	World* m_World = nullptr;  // ref to world.


	// Loaded Chunks. basic linear struct.
	std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> m_Chunks;

	std::mutex m_ChunksLock;

public:
	inline static int cfg_ChunkLoadingMaxConcurrent = 4;
	std::unordered_map<glm::ivec3, std::shared_ptr<stdx::thread_pool::task<std::shared_ptr<Chunk>>>> m_ChunksLoading;

	glm::ivec2 m_TmpLoadDistance{1, 0};

	inline static int cfg_ChunkMeshingMaxConcurrent = 4;
	std::unordered_map<glm::ivec3, std::shared_ptr<stdx::thread_pool::task<std::shared_ptr<Chunk>>>> m_ChunksMeshing;
private:


	// Chunk SVO Root.
	//SVO m_SVO;



	// Load / Generate Chunk, but not load to world
	std::shared_ptr<Chunk> _ProvideChunk(glm::vec3 chunkpos);

	
	void _UpdateChunkLoadAndUnload(glm::vec3 viewpos, glm::ivec3 viewDistance);
	
	//void _AddChunk(std::shared_ptr<Chunk> chunk);
	//
	//void _RemoveChunk(std::shared_ptr<Chunk> chunk);


};