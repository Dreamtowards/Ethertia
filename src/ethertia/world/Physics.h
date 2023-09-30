
#pragma once

#include <PxPhysicsAPI.h>


class Physics
{
public:

	static void Init(bool enablePvd = true, bool recordMemoryAllocations = true);

	static void Release();


	static physx::PxPvd* Pvd();

	static physx::PxPhysics* Phys();

	static physx::PxDefaultCpuDispatcher* CpuDispatcher();
};