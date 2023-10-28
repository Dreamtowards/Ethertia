




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
	// RenderPipeline
	// IsCastShadow
	// IsReceiveShadow
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
	float Intensity = 1.0f;

	glm::vec3 Attenuation{0.5, 0.01, 0.001};

	float ConeAngle = 40;
	float ConeFalloff = 10;
};