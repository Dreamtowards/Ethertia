//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_CHUNK_H
#define ETHERTIA_CHUNK_H

#include <ethertia/entity/EntityMesh.h>
#include <ethertia/util/Mth.h>
#include <ethertia/util/AABB.h>
#include <ethertia/util/UnifiedTypes.h>
#include <ethertia/init/MaterialTextures.h>
#include <ethertia/world/Cell.h>

class World;

class Chunk
{
public:
    static const int SIZE = 16;

    Cell m_Cells[16*16*16] = {};

    glm::vec3 position;
    World* m_World;

    bool m_Populated = false;

    bool m_MeshInvalid = false;  // invalidMesh

    uint64_t m_CreatedTime = 0;  // millis timestamp.

    float m_InhabitedTime = 0;  // seconds.

    // VertexData positions. for fix Normal Smoothing at Chunk Boundary.
    // not use Physics Collision triangle positions yet, it's struct too complicated, BVH triangles, maybe later.
//    std::vector<float> vert_positions;

    /// the 'proxy' entity, for unified functionalities e.g. collisions
    EntityMesh* m_MeshTerrain = nullptr;

    EntityMesh* m_MeshVegetable = nullptr;

    bool m_Meshing = false;  // MeshGen, till Uploaded.

    std::vector<VertexBuffer*> m_BakeMeshes;

    Chunk(glm::vec3 p, World* w) : position(p), m_World(w) {

        m_CreatedTime = Timer::timestampMillis();  // will be reset when Load Chunk From File.

        // init the proxy entity
        m_MeshTerrain = new EntityMesh();
        m_MeshTerrain->setPosition(position);
        m_MeshTerrain->m_TypeTag = Entity::TypeTag::T_CHUNK_TERRAIN;

        m_MeshVegetable = new EntityMesh();
        m_MeshVegetable->setPosition(position);
        m_MeshVegetable->m_TypeTag = Entity::TypeTag::T_CHUNK_VEGETABLE;
        m_MeshVegetable->setNoCollision();
    }
    ~Chunk() {


    }

    inline Cell& getCell(int rx, int ry, int rz) {
        return m_Cells[blockidx(rx, ry, rz)];
    }
    inline Cell& getCell(glm::ivec3 rp) {
        return getCell(rp.x, rp.y, rp.z);
    }

    inline void setCell(int rx, int ry, int rz, const Cell& c) {
        m_Cells[blockidx(rx,ry,rz)] = c;
    }
    inline void setCell(glm::ivec3 rp, const Cell& c) {
        setCell(rp.x, rp.y, rp.z, c);
    }

    inline static glm::ivec3 rpos(glm::vec3 p) {
        return glm::ivec3(Mth::floor(p.x) & 15,
                          Mth::floor(p.y) & 15,
                          Mth::floor(p.z) & 15);
    }

    inline static int blockidx(int x, int y, int z) {
        // assert(x >= 0 && x < 16 && y >= 0 && y < 16 && z >= 0 && z < 16);
        return x << 8 | y << 4 | z;
    }
    inline static glm::vec3 chunkpos(glm::vec3 p) {
        return Mth::floor(p, SIZE);
    }
    inline static bool validchunkpos(glm::vec3 chunkpos) {
        return Mth::mod(chunkpos.x, 16.0f) == 0 &&
               Mth::mod(chunkpos.y, 16.0f) == 0 &&
               Mth::mod(chunkpos.z, 16.0f) == 0;
    }
    static bool outbound(glm::vec3 p) {
        return p.x >= 16 || p.y >= 16 || p.z >= 16 ||
               p.x < 0 || p.y < 0 || p.z < 0;
    }

    [[nodiscard]] AABB getAABB() const {
        return AABB(position, position + glm::vec3(16));
    }

    void requestRemodel() {
        m_MeshInvalid = true;
    }
    void invalidateMesh() {
        m_MeshInvalid = true;
    }

};

#endif //ETHERTIA_CHUNK_H
