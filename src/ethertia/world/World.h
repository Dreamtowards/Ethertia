
#pragma once

#include <entt/entt.hpp>

#include "Entity.h"

#include <unordered_map>



class World
{
public:

	explicit World();
	~World();


	Entity CreateEntity(Entity::Id uuid);

	void DestroyEntity(Entity entity);

	Entity FindEntity(Entity::Id uuid);

	void DuplicateEntity(Entity entity);



	void OnTick(float dt);


private:

	entt::registry m_EntityRegistry;

	std::unordered_map<Entity::Id, entt::entity> m_EntityMap;

};