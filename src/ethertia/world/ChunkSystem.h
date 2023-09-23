
#pragma once


// ChunkProvider
class ChunkSystem
{
public:


	// get loaded chunk.
	// return nullptr if chunk at the chunkpos is not loaded
	std::shared_ptr<Chunk> GetChunk(glm::vec3 chunkpos);

	const std::unordered_map<glm::vec3, std::shared_ptr<Chunk>>& GetChunks();



	void OnTick();



	// GetLoadedChunk or LoadChunk_FromDisk or GenerateChunk
	Chunk* LoadChunk(glm::vec3 chunkpos);
	// ProvideChunk()?

	void SaveChunk();

	void UnloadChunk(std::shared_ptr<Chunk> chunk);

	void QueueUnload(Chunk* chunk);

	void QueueUnloadAll(Chunk* chunk);


private:

	std::unordered_map<glm::vec3, std::shared_ptr<Chunk>> m_Chunks;

};