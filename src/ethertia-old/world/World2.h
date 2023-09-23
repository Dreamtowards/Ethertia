
#pragma once



class Cell
{
public:
	
	float		value	= 0;
	Material*	mtl		= nullptr;

	glm::vec3 p;
	glm::vec3 norm;

};


class Chunk
{

};


// ChunkProvider
class ChunkSystem
{
public:

	// get loaded chunk.
	// return nullptr if chunk at the chunkpos is not loaded
	std::shared_ptr<Chunk> GetChunk(glm::vec3 chunkpos);

	const std::unordered_map<glm::vec3, std::shared_ptr<Chunk>>& GetChunks();

	void OnTick();

	Chunk* LoadChunk(glm::vec3 chunkpos);
	// ProvideChunk()?

	void SaveChunk();

	void UnloadChunk(std::shared_ptr<Chunk> chunk);

	void QueueUnload(Chunk* chunk);

	void QueueUnloadAll(Chunk* chunk);


private:

	std::unordered_map<glm::vec3, std::shared_ptr<Chunk>> m_Chunks;

};



class Entity
{
public:
	
	Entity() = default;
	Entity(entt::entity handle, World* world) : m_EntityHandle(handle), m_World(world) {}
	Entity(const Entity& other) = default;

	template<typename T, typename... Args>
	T& AddComponent(Args&&... args)
	{
		STDX_ASSERT(!HasComponent<T>(), "Entity already has the component!");
		T& component = m_World->m_EntityRegistry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		m_World->OnComponentAdded<T>(*this, component);
		return component;
	}

	template<typename T>
	void RemoveComponent()
	{
		STDX_ASSERT(HasComponent<T>(), "Entity does not have component!");
		m_World->m_EntityRegistry.remove<T>(m_EntityHandle);
	}

	template<typename T>
	T& GetComponent()
	{
		STDX_ASSERT(HasComponent<T>(), "Entity does not have component!");
		return m_World->m_EntityRegistry.get<T>(m_EntityHandle);
	}

	template<typename T>
	bool HasComponent()
	{
		return m_World->m_EntityRegistry.has<T>(m_EntityHandle);
	}


	operator bool() const { return m_EntityHandle != entt::null; }
	operator entt::entity() const { return m_EntityHandle; }

	bool operator==(const Entity& other) const { return m_EntityHandle == other.m_EntityHandle && m_World == other.m_World; }
	bool operator!=(const Entity& other) const { return !(*this == other); }

	// GetUUID, GetName


	// Transform

	class Component
	{
	public:


	};

private:

	entt::entity m_EntityHandle{ entt::null };

	World* m_World = nullptr;
};


class World
{
public:
	using uuid_t = uint64_t;


	void CreateEntity();  // uuid, name

	void DestroyEntity(Entity* entity);

	void DuplicateEntity(Entity entity);

	Entity* FindEntity(uuid_t uuid);



	void Tick(float dt);



};