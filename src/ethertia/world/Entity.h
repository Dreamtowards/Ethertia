
#pragma once

#include <entt/entt.hpp>

class World;


#pragma region Basic EntityCompnents: TagComponent, TransformComponent

#include <string>

/// named TagComponent instead of NameComponent, because 'Tag' could means some 'attached information' like IsEnabled
struct TagComponent
{
	std::string Name;

	bool IsEnabled = true;

};


#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <stdx/stdx.h>

struct TransformComponent
{
	glm::mat4 Transform;

	const glm::vec3& position() const { return stdx::cast<glm::vec3>(Transform[3]); }

	glm::vec3& position() { return stdx::cast<glm::vec3>(Transform[3]); }

	glm::mat3 basis() const { return glm::mat3(Transform); }

	glm::quat quat() const { return glm::quat_cast(basis()); }

};

#pragma endregion

#include <ethertia/world/EntityComponents.h>


class Entity
{
public:
	Entity() = default;
	Entity(entt::entity id, World* world) : m_EntityId(id), m_World(world) {}
	Entity(const Entity& other) = default;

	template<typename T, typename... Args>
	T& AddComponent(Args&&... args)
	{
		//STDX_ASSERT(!HasComponent<T>(), "Entity already has the component!");
		T& component = registry().emplace<T>(m_EntityId, std::forward<Args>(args)...);
		//m_World->OnComponentAdded<T>(m_EntityId, component);
		return component;
	}
	
	template<typename T>
	void RemoveComponent()
	{
		//STDX_ASSERT(HasComponent<T>(), "Entity does not have the component!");
		T& c = registry().remove<T>(m_EntityId);
		//m_World->OnComponentRemove(m_EntityId, c);
	}
	
	template<typename T>
	T& GetComponent() 
	{
		//STDX_ASSERT(HasComponent<T>(), "Entity does not have the component!");
		return registry().get<T>(m_EntityId);
	}
	
	template<typename T>
	bool HasComponent() const
	{
		return registry().any_of<T>(m_EntityId);
	}


	TransformComponent& GetTransform() { return GetComponent<TransformComponent>(); }
	TagComponent& GetTag() { return GetComponent<TagComponent>(); }
	


	operator bool() const { return m_EntityId != entt::null; }
	operator entt::entity() const { return m_EntityId; }
	operator entt::id_type() const { return static_cast<entt::id_type>(m_EntityId); }

	bool operator==(const Entity& other) const { return m_EntityId == other.m_EntityId && m_World == other.m_World; }
	bool operator!=(const Entity& other) const { return !(*this == other); }

	entt::id_type id() const { return static_cast<entt::id_type>(m_EntityId); }
	World& world() { return *m_World; }
	entt::registry& registry() const;


private:

	entt::entity m_EntityId{ entt::null };

	World* m_World = nullptr;
};
