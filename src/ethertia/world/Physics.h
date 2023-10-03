
#pragma once

#include <PxPhysicsAPI.h>

#define ETPX_CTX physx::PxPhysics& PhysX = *Physics::Phys();

using namespace physx;

#include <span>

class Physics
{
public:
	inline static PxMaterial* dbg_DefaultMaterial = nullptr;

	static void Init(bool enablePvd = true, bool recordMemoryAllocations = true);

	static void Release();


	static physx::PxPvd* Pvd();

	static physx::PxPhysics* Phys();

	static physx::PxDefaultCpuDispatcher* CpuDispatcher();



	static const char* GetGeometryTypeName(physx::PxGeometryType::Enum e);



	static PxTriangleMesh* CreateTriangleMesh(const std::span<float[3]> points, const std::span<PxU32[3]> triIndices);
};