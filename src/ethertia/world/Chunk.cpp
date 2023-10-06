

#include "Chunk.h"



Chunk::Chunk(World* world, glm::ivec3 cp) : m_World(world), chunkpos(cp)
{
	++Chunk::dbg_ChunkAlive;


}

void Chunk::_LoadToWorld()
{

    m_NeedRebuildMesh = true;  // RequestRemesh


    // Really? when create chunk object, it might not been put to the world yet
    this->entity = m_World->CreateEntity();

    entity.GetTransform().position() = chunkpos;

    auto& comp = entity.AddComponent<ChunkComponent>();
    comp.VertexData = new VertexData();


    ETPX_CTX;
    comp.RigidStatic = PhysX.createRigidStatic(entity.GetTransform().PxTransform());
    entity.world().PhysScene().addActor(*comp.RigidStatic);
}

Chunk::~Chunk()
{

    auto& comp = entity.GetComponent<ChunkComponent>();

    // Delete Old vkx::VertexBuffer
    if (comp.VertexBuffer)
    {
        vkx::ctx().Device.waitIdle();
        delete comp.VertexBuffer;
        comp.VertexBuffer = nullptr;
    }
    if (comp.VertexData)
    {
        delete comp.VertexData;
        comp.VertexData = nullptr;
    }

    m_World->DestroyEntity(entity);

	--Chunk::dbg_ChunkAlive;
}
