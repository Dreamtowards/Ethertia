//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_CHUNK_H
#define ETHERTIA_CHUNK_H

#include <ethertia/util/Mth.h>
#include <ethertia/util/QuickTypes.h>
#include <ethertia/client/render/Model.h>

class Chunk
{
public:
    ubyte blocks[16*16*16] = {};

    Model* model;  // client tmp
    static constexpr int SIZE = 16;

    glm::vec3 position;

    Chunk() {
    }

    ubyte getBlock(int rx, int ry, int rz) {
        return blocks[blockidx(rx, ry, rz)];
    }
    ubyte getBlock(glm::vec3 blockpos) { //!MOD
        return getBlock((int)blockpos.x & 15, (int)blockpos.y & 15, (int)blockpos.z & 15);
    }
    void setBlock(int rx, int ry, int rz, ubyte blockId) {
        blocks[blockidx(rx,ry,rz)] = blockId;
    }

    glm::vec3 getPosition() {
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

};

#endif //ETHERTIA_CHUNK_H
