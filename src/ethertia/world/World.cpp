

#include "World.h"



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


Entity World::CreateEntity(Entity::Id uuid)
{
	Entity entity{ m_EntityRegistry.create(), this };

	// assign: uuid, transform, name.

	m_EntityMap[uuid] = entity;

	return entity;
}

void World::DestroyEntity(Entity entity)
{
	m_EntityMap.erase(entity.uuid());
	m_EntityRegistry.destroy(entity);
}

Entity World::FindEntity(Entity::Id uuid)
{
	auto it = m_EntityMap.find(uuid);
	if (it == m_EntityMap.end())
		return {};

	return Entity{ it->second, this };
}