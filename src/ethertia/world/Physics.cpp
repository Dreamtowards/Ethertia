

#include "Physics.h"

#include <ethertia/util/Assert.h>
#include <ethertia/util/Log.h>
#include <ethertia/util/BenchmarkTimer.h>


using namespace physx;

PxDefaultAllocator		g_PxAllocator;
PxDefaultErrorCallback	g_PxErrorCallback;
PxFoundation*			g_PxFoundation	= nullptr;
PxPvd*					g_PxPVD			= nullptr;
PxPhysics*				g_PxPhysics		= nullptr;
PxDefaultCpuDispatcher* g_PxCpuDispatcher = nullptr;


void Physics::Init(bool enablePvd, bool recordMemoryAllocations)
{
	BENCHMARK_TIMER;
	Log::info("PhysX {}.{}.{} initializing..\1",
		PX_PHYSICS_VERSION_MAJOR, PX_PHYSICS_VERSION_MINOR, PX_PHYSICS_VERSION_BUGFIX);

	g_PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, g_PxAllocator, g_PxErrorCallback);
	if (!g_PxFoundation)
		ET_ERROR("PxCreateFoundation failed!");

	// PhysX Visual Debugger
	if (enablePvd)
	{
		g_PxPVD = PxCreatePvd(*g_PxFoundation);
		PxPvdTransport* pvdTransport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		g_PxPVD->connect(*pvdTransport, PxPvdInstrumentationFlag::eALL);
	}

	g_PxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PxFoundation, PxTolerancesScale(), recordMemoryAllocations, g_PxPVD);
	if (!g_PxPhysics)
		ET_ERROR("PxCreatePhysics failed.");

	if (!PxInitExtensions(*g_PxPhysics, g_PxPVD))
		ET_ERROR("PxInitExtensions failed.");

	//PxSetPhysXDelayLoadHook(&gDelayLoadHook);
	//PxSetPhysXCookingDelayLoadHook(&gDelayLoadHook);
	//PxSetPhysXCommonDelayLoadHook(&gDelayLoadHook);
	//PxSetPhysXGpuLoadHook(&g_PxGpuLoadHook);

	g_PxCpuDispatcher = PxDefaultCpuDispatcherCreate(2);



	Physics::dbg_DefaultMaterial = g_PxPhysics->createMaterial(0.5, 0.5, 0.6);
}


void Physics::Release()
{

	PxCloseExtensions();
	PX_RELEASE(g_PxCpuDispatcher);
	PX_RELEASE(g_PxPhysics);
	if (g_PxPVD)
	{
		PxPvdTransport* tmp = g_PxPVD->getTransport();
		PX_RELEASE(g_PxPVD); g_PxPVD = nullptr;
		PX_RELEASE(tmp);
	}
	PX_RELEASE(g_PxFoundation);
}


PxPvd* Physics::Pvd() {
	return g_PxPVD;
}

PxPhysics* Physics::Phys() {
	return g_PxPhysics;
}

PxDefaultCpuDispatcher* Physics::CpuDispatcher() {
	return g_PxCpuDispatcher;
}



const char* Physics::GeometryName(physx::PxGeometryType::Enum e)
{
	switch (e)
	{
	case PxGeometryType::Enum::eSPHERE:		return "Sphere";
	case PxGeometryType::Enum::ePLANE:		return "Plane";
	case PxGeometryType::Enum::eCAPSULE:	return "Capsule";
	case PxGeometryType::Enum::eBOX:		return "Box";
	case PxGeometryType::Enum::eCONVEXMESH: return "ConvexMesh";
	case PxGeometryType::Enum::ePARTICLESYSTEM:	return "ParticleSystem";
	case PxGeometryType::Enum::eTETRAHEDRONMESH:return "TetrahedronMesh";
	case PxGeometryType::Enum::eTRIANGLEMESH:	return "TriangleMesh";
	case PxGeometryType::Enum::eHEIGHTFIELD:return "HeightField";
	case PxGeometryType::Enum::eHAIRSYSTEM: return "HairSystem";
	case PxGeometryType::Enum::eCUSTOM:		return "Custom";
	default: return "?";
	}
}




PxTriangleMesh* Physics::CreateTriangleMesh(std::span<const float> points, std::span<const uint32_t> indices)
{
	PxTolerancesScale scale;
	PxCookingParams params(scale);
	// disable mesh cleaning - perform mesh validation on development configurations
	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
	// disable edge precompute, edges are set for each triangle, slows contact generation
	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
	// lower hierarchy for internal mesh
	// params.meshCookingHint = PxMeshCookingHint::eCOOKING_PERFORMANCE;

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = points.size() / 8;  // sizeof(float[3])
	meshDesc.points.stride = sizeof(float) * 8;
	meshDesc.points.data = points.data();

	meshDesc.triangles.count = indices.size() / 3;  // sizeof(uint32[3])
	meshDesc.triangles.stride = sizeof(uint32_t) * 3;
	meshDesc.triangles.data = indices.data();

	// mesh should be validated before cooked without the mesh cleaning
	//ET_ASSERT(PxValidateTriangleMesh(params, meshDesc));

	return PxCreateTriangleMesh(params, meshDesc);  // PhysX->getPhysicsInsertionCallback()
}