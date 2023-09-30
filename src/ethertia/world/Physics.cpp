

#include "Physics.h"

#include <ethertia/util/Assert.h>
#include <ethertia/util/Log.h>


using namespace physx;

PxDefaultAllocator		g_PxAllocator;
PxDefaultErrorCallback	g_PxErrorCallback;
PxFoundation*			g_PxFoundation	= nullptr;
PxPvd*					g_PxPVD			= nullptr;
PxPhysics*				g_PxPhysics		= nullptr;
PxDefaultCpuDispatcher* g_PxCpuDispatcher = nullptr;


void Physics::Init(bool enablePvd, bool recordMemoryAllocations)
{
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


	Log::info("PhysX {}.{}.{} initializing..",
		PX_PHYSICS_VERSION_MAJOR, PX_PHYSICS_VERSION_MINOR, PX_PHYSICS_VERSION_BUGFIX);
}


void Physics::Release()
{

	//PxCloseExtensions();
	PX_RELEASE(g_PxCpuDispatcher);
	PX_RELEASE(g_PxPhysics);
	if (g_PxPVD)
	{
		PxPvdTransport* tmp = g_PxPVD->getTransport();
		PX_RELEASE(g_PxPVD);
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