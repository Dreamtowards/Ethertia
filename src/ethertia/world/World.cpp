

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

//static void _InitComponentListen(World& world)
//{
//	world.ListenComponent<RigidStaticComponent>(
//		[](Entity entity, RigidStaticComponent& comp)
//		{
//			ETPX_CTX;
//
//			if (!comp.RigidStatic)
//			{
//				comp.RigidStatic = PhysX.createRigidStatic(entity.GetTransform().PxTransform());
//
//				ET_ASSERT(comp.RigidStatic);
//			}
//			entity.world().PhysScene().addActor(*comp.RigidStatic);
//		},
//		[](Entity entity, RigidStaticComponent& comp)
//		{
//			Physics::ClearShapes(*comp.RigidStatic);
//
//			entity.world().PhysScene().removeActor(*comp.RigidStatic);
//		}
//	);
//}



World::World()
{

	m_ChunkSystem = new ChunkSystem(this);


	// Init ECS

	//_InitComponentListen(*this);

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
	
}

World::~World()
{
	delete m_ChunkSystem;

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

	{
		ET_PROFILE_("PhysX");

		m_PxScene->simulate(1.0 / 60.0f);
		m_PxScene->fetchResults(true);
	}

	WorldInfo& wi = GetWorldInfo();
	wi.TimeInhabited += dt;
	wi.DayTime += dt / wi.DayTimeLength;
	wi.DayTime -= (int)wi.DayTime;  // trunc to [0-1]


	{
		ET_PROFILE_("ChunkSystem");

		m_ChunkSystem->OnTick(dt);
	}
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

