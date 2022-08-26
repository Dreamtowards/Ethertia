//
// Created by Dreamtowards on 2022/7/8.
//

#ifndef ETHERTIA_BLOCKLEAVES_H
#define ETHERTIA_BLOCKLEAVES_H

#include "Block.h"

class BlockLeaves : public Block
{
public:

    AtlasFrag* frag;
    float randf;

    BlockLeaves(AtlasFrag* frag, float randf = 0.65f) : frag(frag), randf(randf) {}

    bool isOpaque() override {
        return false;
    }

    void getVertexData(VertexBuffer* vbuf, glm::vec3 rpos, Chunk* chunk) override {

//        internalPutCube(vbuf, rpos, chunk, BlockTextures::GLASS);

        internalPutLeaves(vbuf, rpos, chunk, frag);
    }
};

#endif //ETHERTIA_BLOCKLEAVES_H
