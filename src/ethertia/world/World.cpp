

#include "World.h"
#include "Entity.h"

#include <ethertia/world/chunk/ChunkSystem.h>
#include <ethertia/world/Physics.h>

#include <ethertia/Ethertia.h>
#include <ethertia/util/Log.h>
#include <ethertia/util/Assert.h>



//static PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0))
//{
//	ETPX_CTX;
//	PxRigidDynamic* dynamic = PxCreateDynamic(PhysX, t, geometry, *Physics::dbg_DefaultMaterial, 10.0f);
//	dynamic->setAngularDamping(0.5f);
//	dynamic->setLinearVelocity(velocity);
//	dbg_Scene->addActor(*dynamic);
//
//	return dynamic;
//}
//
//static void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent)
//{
//	PxShape* shape = dbg_Phys->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *Physics::dbg_DefaultMaterial);
//	for (PxU32 i = 0; i < size; i++)
//	{
//		for (PxU32 j = 0; j < size - i; j++)
//		{
//			PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(size - i), PxReal(i * 2 + 1), 0) * halfExtent);
//			PxRigidDynamic* body = dbg_Phys->createRigidDynamic(t.transform(localTm));
//			body->attachShape(*shape);
//			PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
//			dbg_Scene->addActor(*body);
//		}
//	}
//	shape->release();
//}

static void _OnAddedRigidStaticComponent(entt::registry& reg, entt::entity eid)
{
	ETPX_CTX;
	RigidStaticComponent& comp = reg.get<RigidStaticComponent>(eid);
	Entity entity{ eid, Ethertia::GetWorld() };
	
	if (!comp.RigidStatic)
	{
		auto& trans = entity.GetTransform();
	
		comp.RigidStatic = PhysX.createRigidStatic(trans.PxTransform());
	
		ET_ASSERT(comp.RigidStatic);
	}
	
	entity.world().PhysScene().addActor(*comp.RigidStatic);
}

//void OnComponentRemove<RigidStaticComponent>(entt::entity entity, RigidStaticComponent& comp)
//{
//	Log::info("Remove RigidStatic");
//
//	Physics::ClearShapes(*comp.RigidStatic);
//
//	m_PxScene->removeActor(*comp.RigidStatic);
//}

World::World()
{

	m_ChunkSystem = std::make_unique<ChunkSystem>(this);


	// Init ECS

	//registry().on_construct<RigidStaticComponent>().connect<&_OnAddedRigidStaticComponent>();

	ListenComponent<RigidStaticComponent>(
		[](Entity entity, RigidStaticComponent& comp) 
		{
			Log::info("Comp RigidStaticComponent 11111 Added {}", (size_t)comp.RigidStatic);
		},
		{}
	);
	

	// InitPhys

	ETPX_CTX;

	PxSceneDesc sceneDesc(PhysX.getTolerancesScale());
	sceneDesc.gravity = { 0, -9.81, 0 };
	sceneDesc.cpuDispatcher = Physics::CpuDispatcher();
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	m_PxScene = PhysX.createScene(sceneDesc);

	if (PxPvdSceneClient* pvdClient = m_PxScene->getScenePvdClient())
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	PxRigidStatic* aGroundStatic = PxCreatePlane(PhysX, PxPlane(0, 1, 0, 0), *Physics::dbg_DefaultMaterial);
	m_PxScene->addActor(*aGroundStatic);
	
	//PxReal stackZ = 10.0f;
	//for (PxU32 i = 0; i < 5; i++)
	//	createStack(PxTransform(PxVec3(0, 0, stackZ -= 10.0f)), 10, 2.0f);
	//
	//createDynamic(PxTransform(PxVec3(0, 40, 100)), PxSphereGeometry(10), PxVec3(0, -50, -100));
}

World::~World()
{
	Log::info("Destroy Entities");
	registry().clear();

	//registry().each([this](entt::entity entity) {
	//	registry().destroy(entity);
	//});

	Log::info("Delete PxScene");
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

void World::DestroyEntity(entt::entity entityId)
{
	Entity entity{ entityId, this };

	m_EntityRegistry.destroy(entityId);
}








#pragma endregion

