//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_CHUNK_H
#define ETHERTIA_CHUNK_H

#include <ethertia/util/Mth.h>
#include <ethertia/util/UnifiedTypes.h>
#include <ethertia/client/render/Model.h>
#include <ethertia/util/AABB.h>
#include <ethertia/block/stat/BlockState.h>
#include <ethertia/entity/EntityMesh.h>

class World;

class Chunk
{
public:
    static const int SIZE = 16;

    BlockState blocks[16*16*16] = {};


    glm::vec3 position;
    World* world;

    bool populated = false;

    Model* model = nullptr;  // client tmp
    bool needUpdateModel = true;

    EntityMesh* proxy = nullptr;

    Chunk(glm::vec3 p) {
        position = p;

        proxy = new EntityMesh();
        proxy->setPosition(position);
    }
    ~Chunk() {

    }

    BlockState& getBlock(int rx, int ry, int rz) {
        return blocks[blockidx(rx, ry, rz)];
    }
    BlockState& getBlock(glm::ivec3 rp) {
        return getBlock(rp.x, rp.y, rp.z);
    }

    void setBlock(int rx, int ry, int rz, const BlockState& block) {
        blocks[blockidx(rx,ry,rz)] = block;
    }
    void setBlock(glm::ivec3 rp, const BlockState& block) {
        setBlock(rp.x, rp.y, rp.z, block);
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

    AABB getAABB() const {
        return AABB(position, position + glm::vec3(16));
    }

    void requestRemodel() {
        needUpdateModel = true;
    }

};

#endif //ETHERTIA_CHUNK_H
