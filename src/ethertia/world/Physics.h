
#pragma once

#include <PxPhysicsAPI.h>

#define ETPX_CTX physx::PxPhysics& PhysX = *Physics::Phys();

class Physics
{
public:

	static void Init(bool enablePvd = true, bool recordMemoryAllocations = true);

	static void Release();


	static physx::PxPvd* Pvd();

	static physx::PxPhysics* Phys();

	static physx::PxDefaultCpuDispatcher* CpuDispatcher();
};