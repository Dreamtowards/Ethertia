//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_CHUNK_H
#define ETHERTIA_CHUNK_H

#include <ethertia/util/Mth.h>
#include <ethertia/util/UnifiedTypes.h>
#include <ethertia/client/render/Model.h>
#include <ethertia/util/AABB.h>

class World;

class Chunk
{
public:
    static const int SIZE = 16;

    u8 blocks[16*16*16] = {};


    glm::vec3 position;
    World* world;

    bool populated = false;

    Model* model = nullptr;  // client tmp
    bool needUpdateModel = true;

    Chunk() {
    }
    ~Chunk() {
        if (model) {
            // todo: main thread.
//            delete model;
        }
    }

    u8 getBlock(int rx, int ry, int rz) {
        return blocks[blockidx(rx, ry, rz)];
    }
    u8 getBlock(glm::ivec3 rp) {
        return getBlock(rp.x, rp.y, rp.z);
    }

    void setBlock(int rx, int ry, int rz, u8 blockID) {
        blocks[blockidx(rx,ry,rz)] = blockID;
    }
    void setBlock(glm::ivec3 rp, u8 block) {
        setBlock(rp.x, rp.y, rp.z, block);
    }

    static glm::ivec3 rpos(glm::vec3 p) {
        return glm::ivec3(Mth::floor(p.x) & 15,
                          Mth::floor(p.y) & 15,
                          Mth::floor(p.z) & 15);
    }

    glm::vec3 getPosition() const {
        return position;
    }

    static inline int blockidx(int x, int y, int z) {
        assert(x >= 0 && x < 16 && y >= 0 && y < 16 && z >= 0 && z < 16);
        return x << 8 | y << 4 | z;
    }
    static inline glm::vec3 chunkpos(glm::vec3 p) {
        return Mth::floor(p, SIZE);
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
