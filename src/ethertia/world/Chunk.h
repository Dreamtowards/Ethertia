
#pragma once

#include "Cell.h"

#include <ethertia/util/AABB.h>
#include <ethertia/util/Assert.h>
#include <ethertia/util/Math.h>

#include <ethertia/world/Entity.h>



class World;

class Chunk
{
public:
	static const int SIZE = 16;

	glm::ivec3 chunkpos;  // world coordinate, but multiples of 16

	Entity entity;  // proxy entity.

	bool m_NeedRebuildMesh = true;  // MeshingState

	float m_TimeInhabited = 0;  // in sec

	explicit Chunk(World* world, glm::ivec3 chunkpos);

	~Chunk();

	// we may create Chunk Object in another thread (WorldGen), and later put it into world. so the Ctor just do simple work, while this do world-related init work.
	void _LoadToWorld();

	inline static int dbg_ChunkAlive = 0;


	Cell& LocalCell(glm::ivec3 localpos) { return m_Cells[Chunk::LocalIdx(localpos)]; }

	Cell GetCell(glm::ivec3 localpos, bool worldwide = false) 
	{ 
		if (worldwide && !Chunk::IsLocalPos(localpos))
		{
			return m_World->GetCell(chunkpos + localpos);
		}
		return LocalCell(localpos); 
	}

	void SetCell(glm::ivec3 localpos, const Cell& cell) { LocalCell(localpos) = cell; }



	AABB GetAABB() const { return { chunkpos, chunkpos + 16 }; }

	World& GetWorld() { return *m_World; }


	Chunk(const Chunk& c) = delete;  // auto-copy is disabled due to Heavy Cell MemoryCost
	Chunk& operator=(const Chunk& c) = delete;

	static glm::ivec3 ChunkPos(glm::ivec3 p) { return Math::Floor16(p); }
	static glm::ivec3 LocalPos(glm::ivec3 p) { return Math::Mod16(p); }

	static bool IsChunkPos(glm::ivec3 chunkpos)  // mod(p, 16) == 0
	{
		return	chunkpos.x % 16 == 0 &&
				chunkpos.y % 16 == 0 &&
				chunkpos.z % 16 == 0;
	}

	// [0, 16)
	static bool IsLocalPos(glm::ivec3 localpos)
	{
		return	localpos.x >= 0 && localpos.x < 16 &&
				localpos.y >= 0 && localpos.y < 16 &&
				localpos.z >= 0 && localpos.z < 16;
	}

	// x << 8 | y << 4 | z
	static int LocalIdx(glm::ivec3 localpos)
	{
		ET_ASSERT(Chunk::IsLocalPos(localpos));
		return localpos.x << 8 | localpos.y << 4 | localpos.z;
	}


private:
	Cell m_Cells[SIZE * SIZE * SIZE]{};

	World* m_World = nullptr;  // ptr to the world


	// m_MeshingState;

};




