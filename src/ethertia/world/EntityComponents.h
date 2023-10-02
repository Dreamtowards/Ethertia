

#include <glm/glm.hpp>
#include <string>



// name as TagComponent instead of NameComponent, because 'Tag' could means some 'attached information' like IsEnabled
struct TagComponent
{
	std::string Name;

	bool IsEnabled;

	TagComponent(const std::string& name, bool enabled = true) : Name(name), IsEnabled(enabled) {}
};



struct TransformComponent
{
	glm::mat4 Transform;

	glm::vec3& position() { return reinterpret_cast<glm::vec3&>(Transform[3]); }
};





#include <ethertia/render/VertexData.h>
#include <vkx/vkx.hpp>

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