

#include "World.h"

#include "Entity.h"


World::World()
{

	//m_ChunkSystem.m_World = this;
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


	m_ChunkSystem.OnTick();
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
