
#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include <ethertia/world/chunk/ChunkSystem.h>

#include "EntityComponents.h"


class WorldInfo
{
public:

	uint64_t Seed = 0;
	float InhabitedTime = 0;

	std::string Name;

	// =t*24hr
	float DayTime = 0;
	float DayTimeLength = 60*6;  // in seconds

};

class Entity;  // fwd

class World
{
public:

	explicit World();
	~World();

	Entity CreateEntity();

	void DestroyEntity(entt::entity id);


	entt::registry& registry() { return m_EntityRegistry; }



	void OnTick(float dt);



	#pragma region WorldInfo
	
	WorldInfo& GetWorldInfo() { return m_WorldInfo; }

	uint64_t GetSeed() const { return m_WorldInfo.Seed; }
	float& GetInhabitedTime() { return m_WorldInfo.InhabitedTime; }

	const std::string& GetWorldName() const { return m_WorldInfo.Name;  }

	#pragma endregion

private:

	entt::registry m_EntityRegistry;

	WorldInfo m_WorldInfo;

	ChunkSystem m_ChunkSystem {this};


	bool m_Paused = false;
	int  m_PausedStepFrames = 0;
};