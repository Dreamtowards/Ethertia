
#pragma once

#include <memory>
#include <unordered_map>
#include <shared_mutex>
#include <glm/vec3.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <stdx/thread_pool.h>

#include <ethertia/world/Chunk.h>
#include <ethertia/world/chunk/ChunkLoader.h>
#include <ethertia/world/chunk/gen/ChunkGenerator.h>

//#include <ethertia/world/chunk/SVO.h>

//#define ET_LOCK_GUARD(mtx) std::lock_guard<std::mutex> _lock(mtx);

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
		auto _lock = LockRead();

		auto it = m_Chunks.find(chunkpos);
		if (it == m_Chunks.end())
			return nullptr;
		return it->second;
	}

	const std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>>& GetChunks()
	{
		return m_Chunks;
	}
	void ForChunks(std::function<bool(const glm::ivec3&, const Chunk&)>& fn)
	{
		auto _lock = LockRead();
		for (auto& it : m_Chunks)
		{
			if (!fn(it.first, *it.second.get()))
				return;
		}
	}
	void ForChunks(std::function<bool(const glm::ivec3&, Chunk&)>& fn)
	{
		auto _lock = LockWrite();
		for (auto& it : m_Chunks)
		{
			if (!fn(it.first, *it.second.get()))
				return;
		}
	}

	// NumChunksLoaded
	size_t ChunksCount()
	{
		auto _lock = LockRead();
		return m_Chunks.size();
	}



	void OnTick();


	[[nodiscard]]
	std::shared_lock<std::shared_mutex> LockRead()
	{
		return std::shared_lock<std::shared_mutex>{m_ChunksLock};
	}

	[[nodiscard]]
	std::lock_guard<std::shared_mutex> LockWrite()
	{
		return std::lock_guard<std::shared_mutex>{m_ChunksLock};
	}

	// Chunk Load and Unload are internal managed. so far no reason to expose interface out.
	// 
	//void QueueLoad(glm::vec3 chunkpos);
	//
	//void SaveChunk();
	//
	//void UnloadChunk(std::shared_ptr<Chunk> chunk);
	//
	//void QueueUnload(std::shared_ptr<Chunk> chunk);
	//
	//void QueueUnloadAll(std::shared_ptr<Chunk> chunk);


private:

	World* m_World = nullptr;  // ref to world.


	// Loaded Chunks. basic linear struct.
	std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> m_Chunks;  // MultiThread RW

	std::shared_mutex m_ChunksLock;

public:
	inline static int cfg_ChunkLoadingMaxConcurrent = 4;
	std::unordered_map<glm::ivec3, std::shared_ptr<stdx::thread_pool::task<std::shared_ptr<Chunk>>>> m_ChunksLoading;  // MainThread only

	glm::ivec2 m_TmpLoadDistance{1, 0};
	glm::vec3 m_ChunkLoadCenter = {};

	inline static int cfg_ChunkMeshingMaxConcurrent = 4;
	std::unordered_map<glm::ivec3, std::shared_ptr<stdx::thread_pool::task<std::shared_ptr<Chunk>>>> m_ChunksMeshing;  // MainThread only
private:


	// Chunk SVO Root.
	//SVO m_SVO;



	// Load / Generate Chunk, but not load to world
	std::shared_ptr<Chunk> _ProvideChunk(glm::ivec3 chunkpos);

	
	void _UpdateChunkLoadAndUnload(glm::vec3 viewpos, glm::ivec3 viewDistance);
	
	//void _AddChunk(std::shared_ptr<Chunk> chunk);
	//
	//void _RemoveChunk(std::shared_ptr<Chunk> chunk);


};