




#include <ethertia/world/Physics.h>


#include <ethertia/render/VertexData.h>
#include <vkx/vkx.hpp>

/*
struct MeshRenderComponent
{
	VertexData* VertexData = nullptr;

	vkx::VertexBuffer* VertexBuffer = nullptr;
};



// Physics Collision Shape  ColliderableComponent
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





struct RenderableComponent
{

};

struct LightComponent
{
	enum LightType
	{
		eDirectional,
		ePoint,
		eSpot,
		Count
	};
	inline static const char* LightTypeNames[] = {
		"Directional",
		"Point",
		"Spot"
	};

	LightType Type = eDirectional;
	glm::vec3 Color = {1, 1, 1};

	glm::vec3 Direction{};

	glm::vec3 Attenuation{0.5, 0.1, 0.01};

	float ConeAngle = 0;
	float ConeFalloff = 0;
};