
#pragma once

#include <stdint.h>

#include "World.h"


class Entity
{
public:
	Entity() = default;
	Entity(entt::entity handle, World* world, uint64_t uuid) : m_EntityHandle(handle), m_World(world), m_UUID(uuid) {}
	Entity(const Entity& other) = default;

	//template<typename T, typename... Args>
	//T& AddComponent(Args&&... args)
	//{
	//	STDX_ASSERT(!HasComponent<T>(), "Entity already has the component!");
	//	T& component = m_World->m_EntityRegistry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
	//	m_World->OnComponentAdded<T>(*this, component);
	//	return component;
	//}
	//
	//template<typename T>
	//void RemoveComponent()
	//{
	//	STDX_ASSERT(HasComponent<T>(), "Entity does not have component!");
	//	m_World->m_EntityRegistry.remove<T>(m_EntityHandle);
	//}
	//
	//template<typename T>
	//T& GetComponent() 
	//{
	//	STDX_ASSERT(HasComponent<T>(), "Entity does not have component!");
	//	return m_World->m_EntityRegistry.get<T>(m_EntityHandle);
	//}
	//
	//template<typename T>
	//bool HasComponent() const
	//{
	//	return m_World->m_EntityRegistry.has<T>(m_EntityHandle);
	//}


	operator bool() const { return m_EntityHandle != entt::null; }
	operator entt::entity() const { return m_EntityHandle; }

	bool operator==(const Entity& other) const { return m_EntityHandle == other.m_EntityHandle && m_World == other.m_World; }
	bool operator!=(const Entity& other) const { return !(*this == other); }

	// GetName, GetTransform

	uint64_t uuid() const { return m_UUID; }

	// Transform

	class Component
	{
	public:


	};

private:

	entt::entity m_EntityHandle{ entt::null };

	World* m_World = nullptr;

	uint64_t m_UUID;
};