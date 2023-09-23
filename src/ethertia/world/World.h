
#pragma once

#include <entt/entt.hpp>

#include <unordered_map>


class WorldInfo
{
public:

	uint64_t Seed = 0;
	float InhabitedTime = 0;

	std::string Name;

};

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

	WorldInfo& GetWorldInfo() { return m_WorldInfo; }

private:

	entt::registry m_EntityRegistry;

	std::unordered_map<uint64_t, entt::entity> m_EntityMap;

	WorldInfo m_WorldInfo;

};