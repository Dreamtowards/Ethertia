

#include "World.h"
#include "Entity.h"

#include <ethertia/world/chunk/ChunkSystem.h>


World::World()
{

	m_ChunkSystem = std::make_unique<ChunkSystem>(this);
}

World::~World()
{

}



void World::OnTick(float dt)
{
	if (m_Paused && --m_PausedStepFrames < 0)
		return;

	WorldInfo& wi = GetWorldInfo();
	wi.InhabitedTime += dt;
	wi.DayTime += dt / wi.DayTimeLength;
	wi.DayTime -= (int)wi.DayTime;  // keep [0-1]


	m_ChunkSystem->OnTick();
}


Entity World::CreateEntity()
{
	Entity entity{ m_EntityRegistry.create(), this };

	// assign: uuid, transform, name.

	m_EntityRegistry.emplace<TagComponent>(entity, "Entity");
	m_EntityRegistry.emplace<TransformComponent>(entity, glm::mat4(1.0f));

	return entity;
}

void World::DestroyEntity(entt::entity entity)
{
	m_EntityRegistry.destroy(entity);
}



Cell& World::GetCell(glm::ivec3 p)
{
	auto chunk = GetChunkSystem().GetChunk(Chunk::ChunkPos(p));

	if (!chunk)
		return Cell::Nil();
	
	return chunk->LocalCell(Chunk::LocalPos(p));
}