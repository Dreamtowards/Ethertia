
#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include <PxPhysicsAPI.h>

#include <ethertia/util/Assert.h>

#include <ethertia/world/chunk/ChunkSystem.h>

class ChunkSystem;

class WorldInfo
{
public:

	uint64_t Seed = 0;

	std::string Name;

	// =t*24hr
	float DayTime = 0;
	float DayTimeLength = 60*6;  // in seconds

	float TimeInhabited = 0;
	uint64_t TimeCreated = 0;
	uint64_t TimeModified = 0;

};

class Cell;
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

	const Cell& GetCell(glm::ivec3 p) { return GetChunkSystem().GetCell(p); }

	ChunkSystem& GetChunkSystem() { return *m_ChunkSystem; }

	physx::PxScene& PhysScene() { return *m_PxScene; }


	//template<typename T>
	//void OnComponentAdded(entt::entity entity, T& component);
	//
	//template<typename T>
	//void OnComponentRemove(entt::entity entity, T& component);

	template<typename ComponentType>
	using ComponentListener = std::function<void(Entity, ComponentType&)>;

	// Only Listen to the Components that need be Listen
	template<typename ComponentType>
	void ListenComponent(ComponentListener<ComponentType> on_construct, ComponentListener<ComponentType> on_destroy)
	{
		struct CompLsrAdapter
		{
			World* world = nullptr;
			ComponentListener<ComponentType> fnCreate;
			ComponentListener<ComponentType> fnDestroy;

			void OnConstruct(entt::registry& reg, entt::entity eid) {
				fnCreate(Entity{ eid, world }, reg.get<ComponentType>(eid));
			}
			void OnDestroy(entt::registry& reg, entt::entity eid) {
				fnDestroy(Entity{ eid, world }, reg.get<ComponentType>(eid));
			}
		};
		CompLsrAdapter* lsr = new CompLsrAdapter();  // std::shared_ptr not working here.
		lsr->world = this;
		// m_DeleteOnWorldDestroy->push_back();
		// todo: Delete on World Destroy

		if (on_construct)
		{
			lsr->fnCreate = on_construct;
			registry().on_construct<ComponentType>().connect<&CompLsrAdapter::OnConstruct>(lsr);
		}
		if (on_destroy)
		{
			lsr->fnDestroy = on_destroy;
			registry().on_destroy<ComponentType>().connect<&CompLsrAdapter::OnDestroy>(lsr);
		}
	}

	#pragma region WorldInfo
	
	WorldInfo& GetWorldInfo() { return m_WorldInfo; }

	uint64_t GetSeed() const { return m_WorldInfo.Seed; }

	const std::string& GetWorldName() const { return m_WorldInfo.Name;  }

	#pragma endregion

private:

	entt::registry m_EntityRegistry;

	WorldInfo m_WorldInfo;

	ChunkSystem* m_ChunkSystem;


	bool m_Paused = false;
	int  m_PausedStepFrames = 0;

	physx::PxScene* m_PxScene = nullptr;
};