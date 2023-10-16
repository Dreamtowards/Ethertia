

#include "Chunk.h"



Chunk::Chunk(World* world, glm::ivec3 cp) : m_World(world), chunkpos(cp)
{


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
    comp.RigidStatic = PhysX.createRigidStatic(Physics::PxTransform(entity.GetTransform()));
    comp.RigidStatic->userData = (void*)entity.id();
    m_World->PhysScene().addActor(*comp.RigidStatic);
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

    Physics::ClearShapes(*comp.RigidStatic);

    m_World->PhysScene().removeActor(*comp.RigidStatic);
    PX_RELEASE(comp.RigidStatic);

    m_World->DestroyEntity(entity);

}
