

#include "Chunk.h"

#include <ethertia/util/Math.h>


Chunk::Chunk(World* world, glm::vec3 cp) : m_World(world), chunkpos(cp)
{

}


glm::ivec3 Chunk::ChunkPos(glm::ivec3 p)
{
	return {
		Math::Floor16(p.x),
		Math::Floor16(p.y),
		Math::Floor16(p.z)
	};
}

glm::ivec3 Chunk::LocalPos(glm::ivec3 p)
{
	return {
		p.x & 15,
		p.y & 15,
		p.z & 15
	};
}

bool Chunk::IsChunkPos(glm::ivec3 chunkpos)
{
	return	chunkpos.x % 16 == 0 && 
			chunkpos.y % 16 == 0 && 
			chunkpos.z % 16 == 0;
}
bool Chunk::IsLocalPos(glm::ivec3 localpos)
{
	return	localpos.x >= 0 && localpos.x < 16 &&
			localpos.y >= 0 && localpos.y < 16 &&
			localpos.z >= 0 && localpos.z < 16;
}
