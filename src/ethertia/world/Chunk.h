
#pragma once

#include "Cell.h"

#include <ethertia/util/AABB.h>

class World;

class Chunk
{
public:
	static const int SIZE = 16;

	glm::vec3 chunkpos;  // world coordinate, but multiples of 16

	bool m_NeedRebuildMesh = true;  // MeshingState

	explicit Chunk(World* world, glm::vec3 chunkpos);



	Cell GetCell(glm::ivec3 lp);

	void SetCell(glm::ivec3 lp, const Cell& c);



	AABB BoundingBox() const { return AABB(chunkpos, chunkpos+16.0f); }


	Chunk(const Chunk& c) = delete;  // auto-copy is disabled due to Heavy Cell MemoryCost
	Chunk& operator=(const Chunk& c) = delete;


	static glm::vec3 ChunkPos(glm::vec3 p);  // floor(p, 16)
	static glm::vec3 LocalPos(glm::vec3 p);  // mod(p, 16)

	static bool IsChunkPos(glm::vec3 chunkpos);   // mod(p, 16) == 0
	static bool IsLocalPos(glm::ivec3 localpos);  // [0, 16)

private:
	Cell m_Cells[SIZE * SIZE * SIZE]{};

	World* m_World = nullptr;  // ptr to the world

	float m_TimeInhabited = 0;  // in sec

	// m_MeshingState;

	static int CellIdx(int x, int y, int z);
};