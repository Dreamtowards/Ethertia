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

    Cell blocks[16*16*16] = {};

    glm::vec3 position;
    World* world;

    bool populated = false;

    bool needUpdateModel = true;

    // VertexData positions. for fix Normal Smoothing at Chunk Boundary.
    // not use Physics Collision triangle positions yet, it's struct too complicated, BVH triangles, maybe later.
//    std::vector<float> vert_positions;

    /// the 'proxy' entity, for unified functionalities e.g. collisions
    EntityMesh* m_MeshTerrain = nullptr;

    EntityMesh* m_MeshVegetable = nullptr;

    Chunk(glm::vec3 p, World* w) : position(p), world(w) {

        // init the proxy entity
        m_MeshTerrain = new EntityMesh();
        m_MeshTerrain->setPosition(position);

        m_MeshVegetable = new EntityMesh();
        m_MeshVegetable->setPosition(position);
        m_MeshVegetable->m_FaceCulling = false;
    }
    ~Chunk() {


    }

    Cell& getCell(int rx, int ry, int rz) {
        return blocks[blockidx(rx, ry, rz)];
    }
    Cell& getCell(glm::ivec3 rp) {
        return getCell(rp.x, rp.y, rp.z);
    }

    void setCell(int rx, int ry, int rz, const Cell& c) {
        blocks[blockidx(rx,ry,rz)] = c;
    }
    void setCell(glm::ivec3 rp, const Cell& c) {
        setCell(rp.x, rp.y, rp.z, c);
    }

    static glm::ivec3 rpos(glm::vec3 p) {
        return glm::ivec3(Mth::floor(p.x) & 15,
                          Mth::floor(p.y) & 15,
                          Mth::floor(p.z) & 15);
    }

    static inline int blockidx(int x, int y, int z) {
        // assert(x >= 0 && x < 16 && y >= 0 && y < 16 && z >= 0 && z < 16);
        return x << 8 | y << 4 | z;
    }
    static inline glm::vec3 chunkpos(glm::vec3 p) {
        return Mth::floor(p, SIZE);
    }
    static inline bool validchunkpos(glm::vec3 chunkpos) {
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
        needUpdateModel = true;
    }

};

#endif //ETHERTIA_CHUNK_H
