
#pragma once

#include <PxPhysicsAPI.h>

#define ETPX_CTX physx::PxPhysics& PhysX = *Physics::Phys();

using namespace physx;

#include <span>
#include <vector>
#include <stdx/stdx.h>

#include <ethertia/render/VertexData.h>

#include <ethertia/world/Entity.h>

class Physics
{
public:
	inline static PxMaterial* dbg_DefaultMaterial = nullptr;

	static void Init(bool enablePvd = true, bool recordMemoryAllocations = true);

	static void Release();


	static physx::PxPvd* Pvd();

	static physx::PxPhysics* Phys();

	static physx::PxDefaultCpuDispatcher* CpuDispatcher();



	static const char* GeometryName(physx::PxGeometryType::Enum e);



	static PxTriangleMesh* CreateTriangleMesh(std::span<const float> vtx, int vtx_stride, std::span<const uint32_t> indices);

	static PxTriangleMesh* CreateTriangleMesh(const VertexData& indexed) { return CreateTriangleMesh(indexed.vtx_span(), 8, indexed.idx_span()); }


	static void GetShapes(PxRigidActor& rigid, std::vector<PxShape*>& out)
	{
		PxU32 numShapes = rigid.getNbShapes();
		out.resize(numShapes);
		rigid.getShapes(out.data(), numShapes);
	}

	static void GetMaterials(PxShape& shape, std::vector<PxMaterial*>& out)
	{
		PxU32 num = shape.getNbMaterials();
		out.resize(num);
		shape.getMaterials(out.data(), num);
	}

	static void ClearShapes(PxRigidActor& rigid)
	{
		static std::vector<PxShape*> _Shapes;
		Physics::GetShapes(rigid, _Shapes);
		for (PxShape* shape : _Shapes)
		{
			rigid.detachShape(*shape);
		}
	}

	static PxTransform PxTransform(const TransformComponent& tc)
	{
		return {
			stdx::cast<PxVec3>(tc.position()),
			stdx::cast<PxQuat>(tc.quat())
		};
	}
};