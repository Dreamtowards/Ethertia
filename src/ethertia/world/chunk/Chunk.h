//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_CHUNK_H
#define ETHERTIA_CHUNK_H

#include <ethertia/util/Mth.h>

class Chunk
{
    unsigned char blocks[16*16*16];

public:
    int vao;  // client tmp
    static constexpr int SIZE = 16;

    unsigned char getBlock(int rx, int ry, int rz) {
        return blocks[blockidx(rx, ry, rz)];
    }

    static inline int blockidx(int x, int y, int z) {
        return x << 8 | y << 4 | z;
    }

    static inline glm::vec3 chunkpos(glm::vec3 p) {
        return Mth::floor(p, SIZE);
    }


};

#endif //ETHERTIA_CHUNK_H
