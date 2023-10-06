

#include "Chunk.h"



Chunk::Chunk(World* world, glm::ivec3 cp) : m_World(world), chunkpos(cp)
{
	++Chunk::dbg_ChunkAlive;
}

Chunk::~Chunk()
{
	--Chunk::dbg_ChunkAlive;
}
