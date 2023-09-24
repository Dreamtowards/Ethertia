
#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include <unordered_map>


class WorldInfo
{
public:

	uint64_t Seed = 0;
	float InhabitedTime = 0;

	std::string Name;

};

class TransformComponent
{
	glm::mat4 transform;
};
class NameComponent
{
	std::string name;
};

class Entity;  // fwd

class World
{
public:

	explicit World();
	~World();

	Entity CreateEntity();

	void DestroyEntity(entt::entity uuid);

	Entity FindEntity(entt::entity uuid);

	void DuplicateEntity(entt::entity uuid);

	entt::registry& registry() { return m_EntityRegistry; }


	void OnTick(float dt);

	WorldInfo& GetWorldInfo() { return m_WorldInfo; }

private:

	entt::registry m_EntityRegistry;

	WorldInfo m_WorldInfo;

};