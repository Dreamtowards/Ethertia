
#pragma once

#include <memory>
#include <unordered_map>
#include <glm/vec3.hpp>
#include <glm/gtx/hash.hpp>

#include <stdx/thread_pool.h>

#include <ethertia/world/Chunk.h>
#include <ethertia/world/chunk/ChunkLoader.h>
#include <ethertia/world/chunk/ChunkGenerator.h>

//#include <ethertia/world/chunk/SVO.h>



class ChunkSystem
{
public:

	// get loaded chunk.
	// return nullptr if chunk at the chunkpos is not loaded
	std::shared_ptr<Chunk> GetChunk(glm::vec3 chunkpos);

	const std::unordered_map<glm::vec3, std::shared_ptr<Chunk>>& GetChunks();



	void OnTick();


	void QueueLoad(glm::vec3 chunkpos);

	void SaveChunk();

	void UnloadChunk(std::shared_ptr<Chunk> chunk);

	void QueueUnload(std::shared_ptr<Chunk> chunk);

	void QueueUnloadAll(std::shared_ptr<Chunk> chunk);


private:

	World* m_World = nullptr;  // ref to world.


	// Loaded Chunks. basic linear struct.
	std::unordered_map<glm::vec3, std::shared_ptr<Chunk>> m_Chunks;

	std::unordered_map<glm::vec3, std::shared_ptr<stdx::thread_pool::task<std::shared_ptr<Chunk>>>> m_ChunksLoading;


	// Chunk SVO Root.
	//SVO m_SVO;



	// Load / Generate Chunk, but not load to world
	std::shared_ptr<Chunk> _ProvideChunk(glm::vec3 chunkpos);


	void _UpdateChunkLoadAndUnload(glm::vec3 viewpos, glm::vec2 viewDistance);
	
	//void _AddChunk(std::shared_ptr<Chunk> chunk);
	//
	//void _RemoveChunk(std::shared_ptr<Chunk> chunk);


};