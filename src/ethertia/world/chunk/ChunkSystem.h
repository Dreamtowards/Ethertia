
#pragma once

#include <memory>
#include <unordered_map>
#include <glm/vec3.hpp>
#include <glm/gtx/hash.hpp>

#include <ethertia/world/Chunk.h>

#include <ethertia/world/chunk/SVO.h>



class ChunkSystem
{
public:

	// todo: use SVO to get a chunk in O(logN) time.

	// get loaded chunk.
	// return nullptr if chunk at the chunkpos is not loaded
	std::shared_ptr<Chunk> GetChunk(glm::vec3 chunkpos);

	const std::unordered_map<glm::vec3, std::shared_ptr<Chunk>>& GetChunks();



	void OnTick();



	// GetLoadedChunk or LoadChunk_FromDisk or GenerateChunk
	std::shared_ptr<Chunk> LoadChunk(glm::vec3 chunkpos);
	// ProvideChunk()?

	void SaveChunk();

	void UnloadChunk(std::shared_ptr<Chunk> chunk);

	void QueueUnload(std::shared_ptr<Chunk> chunk);

	void QueueUnloadAll(std::shared_ptr<Chunk> chunk);


private:

	World* m_World = nullptr;  // ref to world.


	// Loaded Chunks. basic linear struct.
	std::unordered_map<glm::vec3, std::shared_ptr<Chunk>> m_Chunks;

	// Chunk SVO Root.
	SVO m_SVO;
};