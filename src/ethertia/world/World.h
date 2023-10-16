
#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <ethertia/world/Entity.h>
#include <ethertia/world/chunk/ChunkSystem.h>

#include <ethertia/world/Physics.h>
#include <ethertia/util/Assert.h>


class ChunkSystem;
class Cell;
class Entity;  // fwd

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


class HitResult
{
public:
	bool enabled = true;
	bool hit = false;

	glm::vec3 position;
	glm::vec3 normal;
	float distance = 0;  // length(origin - hit.position)

	Entity entity;

	bool hitVoxel = false;  // if the hit.entity is a chunk.

	//Cell* cell = nullptr;  // may invalid even if .hit/.hitTerrain is true.  but the cell.mtl always is valid.
	//float cell_breaking_time = 0;  // sec breaking. >= 0. if not 0 means is breaking.


	operator bool() const { return hit; }

};



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

	PxScene& PhysScene() { return *m_PxScene; }

	bool Raycast(glm::vec3 origin, glm::vec3 dir, float maxDistance,
		glm::vec3& out_Position, glm::vec3& out_Normal, PxShape** out_Shape = nullptr, PxRigidActor** out_Actor = nullptr) const;


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

	inline static PxController* dbg_CCT;

	bool IsPaused() const { return m_Paused; }
	void SetPause(bool paused) { m_Paused = paused; }
	void SetPauseSteps(int steps) { m_PausedStepFrames = steps; }

private:

	entt::registry m_EntityRegistry;

	WorldInfo m_WorldInfo;

	ChunkSystem* m_ChunkSystem;


	bool m_Paused = false;
	int  m_PausedStepFrames = 0;

	PxScene* m_PxScene = nullptr;
	PxControllerManager* m_PxControllerManager = nullptr;

};