

#include "World.h"
#include "Entity.h"

#include <ethertia/world/chunk/ChunkSystem.h>

#include <ethertia/world/Physics.h>

#include <ethertia/util/Log.h>


using namespace physx;

static PxMaterial* dbg_PhysMtl = nullptr;
static PxPhysics* dbg_Phys = nullptr;
static PxScene* dbg_Scene = nullptr;

static PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0))
{
	PxRigidDynamic* dynamic = PxCreateDynamic(*dbg_Phys, t, geometry, *dbg_PhysMtl, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	dbg_Scene->addActor(*dynamic);

	return dynamic;
}

static void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent)
{
	PxShape* shape = dbg_Phys->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *dbg_PhysMtl);
	for (PxU32 i = 0; i < size; i++)
	{
		for (PxU32 j = 0; j < size - i; j++)
		{
			PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(size - i), PxReal(i * 2 + 1), 0) * halfExtent);
			PxRigidDynamic* body = dbg_Phys->createRigidDynamic(t.transform(localTm));
			body->attachShape(*shape);
			PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			dbg_Scene->addActor(*body);
		}
	}
	shape->release();
}


World::World()
{

	m_ChunkSystem = std::make_unique<ChunkSystem>(this);


	// InitPhys

	PxPhysics* _Phys = Physics::Phys();

	PxSceneDesc sceneDesc(_Phys->getTolerancesScale());
	sceneDesc.gravity = { 0, -9.81, 0 };
	sceneDesc.cpuDispatcher = Physics::CpuDispatcher();
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	m_PxScene = _Phys->createScene(sceneDesc);

	if (PxPvdSceneClient* pvdClient = m_PxScene->getScenePvdClient())
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	dbg_Phys = _Phys;
	dbg_Scene = m_PxScene;

	dbg_PhysMtl = _Phys->createMaterial(0.5, 0.5, 0.6);

	PxRigidStatic* aGroundStatic = PxCreatePlane(*_Phys, PxPlane(0, 1, 0, 0), *dbg_PhysMtl);
	m_PxScene->addActor(*aGroundStatic);
	
	//PxReal stackZ = 10.0f;
	//for (PxU32 i = 0; i < 5; i++)
	//	createStack(PxTransform(PxVec3(0, 0, stackZ -= 10.0f)), 10, 2.0f);
	//
	//createDynamic(PxTransform(PxVec3(0, 40, 100)), PxSphereGeometry(10), PxVec3(0, -50, -100));
}

World::~World()
{

	PX_RELEASE(m_PxScene);
}



void World::OnTick(float dt)
{
	if (m_Paused && --m_PausedStepFrames < 0)
		return;

	m_PxScene->simulate(1.0 / 60.0f);
	m_PxScene->fetchResults(true);

	WorldInfo& wi = GetWorldInfo();
	wi.InhabitedTime += dt;
	wi.DayTime += dt / wi.DayTimeLength;
	wi.DayTime -= (int)wi.DayTime;  // keep [0-1]


	m_ChunkSystem->OnTick();
}


Cell& World::GetCell(glm::ivec3 p)
{
	auto chunk = GetChunkSystem().GetChunk(Chunk::ChunkPos(p));

	if (!chunk)
		return Cell::Nil();

	return chunk->LocalCell(Chunk::LocalPos(p));
}




















#pragma region ECS


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


template<typename T>
void World::OnComponentAdded(entt::entity entity, T& comp)
{
	Log::info("Component Added {}", entt::type_name<T>());
}


template<>
void World::OnComponentAdded<MeshRenderComponent>(entt::entity entity, MeshRenderComponent& comp)
{

}

template<>
void World::OnComponentAdded<RendererComponent>(entt::entity entity, RendererComponent& comp)
{

}


template<>
void World::OnComponentAdded<RigidStaticComponent>(entt::entity entity, RigidStaticComponent& comp)
{
	ETPX_CTX;

	if (!comp.RigidStatic)
	{
		comp.RigidStatic = PhysX.createRigidStatic(PxTransform({0,0,0}));

		PxShape* shape = dbg_Phys->createShape(PxBoxGeometry(2.f, 2.f, 2.f), *dbg_PhysMtl);
		comp.RigidStatic->attachShape(*shape);
		shape->release();

		ET_ASSERT(comp.RigidStatic);
	}

	m_PxScene->addActor(*comp.RigidStatic);

}
template<>
void World::OnComponentRemove<RigidStaticComponent>(entt::entity entity, RigidStaticComponent& comp)
{
	m_PxScene->removeActor(*comp.RigidStatic);
}





#pragma endregion

