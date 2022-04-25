//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_CHUNK_H
#define ETHERTIA_CHUNK_H

#include <ethertia/util/Mth.h>
#include <ethertia/util/QuickTypes.h>
#include <ethertia/client/render/Model.h>

typedef unsigned char ubyte;

class Chunk
{
    ubyte blocks[16*16*16] = {};

public:
    Model* model;  // client tmp
    static constexpr int SIZE = 16;

    glm::vec3 position;

    Chunk() {
    }

    ubyte getBlock(int rx, int ry, int rz) {
        return blocks[blockidx(rx, ry, rz)];
    }
    void setBlock(int rx, int ry, int rz, ubyte blockId) {
        blocks[blockidx(rx,ry,rz)] = blockId;
    }

    glm::vec3 getPosition() {
        return position;
    }

    static inline int blockidx(int x, int y, int z) {
        return x << 8 | y << 4 | z;
    }
    static inline glm::vec3 chunkpos(glm::vec3 p) {
        return Mth::floor(p, SIZE);
    }

};

#endif //ETHERTIA_CHUNK_H
