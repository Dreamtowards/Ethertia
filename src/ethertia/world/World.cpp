

#include "World.h"

#include "Entity.h"


World::World()
{

}

World::~World()
{

}


// OnUpdate()
void World::OnTick(float dt)
{
	//if (m_IsPaused && --m_PausedStepFrames < 0)
	//	return;
}

static int g_LastEntityUUID = 0;

Entity World::CreateEntity(uint64_t uuid)
{
	if (uuid == 0)
	{
		uuid = ++g_LastEntityUUID;
	}

	Entity entity{ m_EntityRegistry.create(), this, uuid };

	// assign: uuid, transform, name.

	m_EntityMap[uuid] = entity;

	return entity;
}

void World::DestroyEntity(Entity entity)
{
	m_EntityMap.erase(entity.uuid());
	m_EntityRegistry.destroy(entity);
}

Entity World::FindEntity(uint64_t uuid)
{
	auto it = m_EntityMap.find(uuid);
	if (it == m_EntityMap.end())
		return {};

	return Entity{ it->second, this, uuid };
}