

#include "World.h"
#include "Entity.h"

#include <ethertia/world/chunk/ChunkSystem.h>

#include <ethertia/world/Physics.h>


using namespace physx;

World::World()
{

	m_ChunkSystem = std::make_unique<ChunkSystem>(this);

	PxPhysics* _Phys = Physics::Phys();


	PxSceneDesc sceneDesc(_Phys->getTolerancesScale());
	sceneDesc.gravity = { 0, -9.81, 0 };
	sceneDesc.cpuDispatcher = Physics::CpuDispatcher();
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	m_PxScene = _Phys->createScene(sceneDesc);

	if (Physics::Pvd())
	{
		PxPvdSceneClient* pvdClient = m_PxScene->getScenePvdClient();
		assert(pvdClient);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	m_PxScene->lockRead();

	PxMaterial* g_PxDefaultMaterial = _Phys->createMaterial(0.5, 0.5, 0.6);

	PxRigidStatic* aGroundStatic = PxCreatePlane(*_Phys, PxPlane(0, 1, 0, 0), *g_PxDefaultMaterial);
	m_PxScene->addActor(*aGroundStatic);

	for (int i = 0; i < 30; ++i) {
		for (int j = 0; j < 30; ++j) {
			PxTransform localTm();

		}
	}

	while (true)
	{
		m_PxScene->simulate(1.0 / 60.0f);
		m_PxScene->fetchResults(true);
	}
}

World::~World()
{

	PX_RELEASE(m_PxScene);
}



void World::OnTick(float dt)
{
	if (m_Paused && --m_PausedStepFrames < 0)
		return;

	WorldInfo& wi = GetWorldInfo();
	wi.InhabitedTime += dt;
	wi.DayTime += dt / wi.DayTimeLength;
	wi.DayTime -= (int)wi.DayTime;  // keep [0-1]


	m_ChunkSystem->OnTick();
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



Cell& World::GetCell(glm::ivec3 p)
{
	auto chunk = GetChunkSystem().GetChunk(Chunk::ChunkPos(p));

	if (!chunk)
		return Cell::Nil();
	
	return chunk->LocalCell(Chunk::LocalPos(p));
}