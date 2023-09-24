

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

	m_EntityRegistry.emplace<TagComponent>(entity, "Entity");
	m_EntityRegistry.emplace<TransformComponent>(entity, glm::mat4(1.0f));

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