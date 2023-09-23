
#pragma once

#include <entt/entt.hpp>

#include <unordered_map>


class Entity;  // fwd

class World
{
public:

	explicit World();
	~World();


	Entity CreateEntity(uint64_t uuid);

	void DestroyEntity(Entity entity);

	Entity FindEntity(uint64_t uuid);

	void DuplicateEntity(Entity entity);



	void OnTick(float dt);


private:

	entt::registry m_EntityRegistry;

	std::unordered_map<uint64_t, entt::entity> m_EntityMap;

};