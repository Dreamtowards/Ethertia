

#include "World.h"

#include "Entity.h"


World::World()
{
	//auto view = m_EntityRegistry.view<ScriptComponent>();
	//for (auto e : view)
	//{
	//	Entity entity = { e, this };
	//	ScriptEngine::OnCreateEntity(entity);
	//}
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


Entity World::CreateEntity(uint64_t uuid)
{
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