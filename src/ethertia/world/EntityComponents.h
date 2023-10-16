




#include <ethertia/world/Physics.h>


#include <ethertia/render/VertexData.h>
#include <vkx/vkx.hpp>

/*
struct MeshRenderComponent
{
	VertexData* VertexData = nullptr;

	vkx::VertexBuffer* VertexBuffer = nullptr;
};



// Physics Collision Shape
// UE: 
struct ColliderComponent
{
	physx::PxShape* Shape = nullptr;

	physx::PxMaterial* Material = nullptr;
};


// Graphics Render

struct RendererComponent
{
	// Input VertexData
	// Material: Shader, Uniforms

	int sth;
};


struct RigidStaticComponent
{
	physx::PxRigidStatic* RigidStatic = nullptr;
};

struct RigidDynamicComponent
{
	physx::PxRigidDynamic* RigidDynamic = nullptr;
};
*/



struct ChunkComponent
{

	physx::PxRigidStatic* RigidStatic = nullptr;

	VertexData* VertexData = nullptr;

	vkx::VertexBuffer* VertexBuffer = nullptr;

};

struct NativeScriptComponent
{
	// std::function<void()> OnCreate; OnUpdate, OnDestroy
};