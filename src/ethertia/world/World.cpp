

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


Entity World::CreateEntity()
{
	Entity entity{ m_EntityRegistry.create(), this };

	// assign: uuid, transform, name.

	m_EntityRegistry.emplace<std::string>(entity, "Entity Name");
	m_EntityRegistry.emplace<uint16_t>(entity, 123);

	m_EntityRegistry.emplace<TransformComponent>(entity);
	m_EntityRegistry.emplace<NameComponent>(entity);

	return entity;
}

void World::DestroyEntity(entt::entity entity)
{
	m_EntityRegistry.destroy(entity);
}

Entity World::FindEntity(entt::entity id)
{
	return Entity{ id, this };
}