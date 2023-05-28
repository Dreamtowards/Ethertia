//
// Created by Dreamtowards on 2022/5/5.
//

#ifndef ETHERTIA_BLOCKGRASS_H
#define ETHERTIA_BLOCKGRASS_H

#include "Block.h"

class BlockGrass : public Block
{

    void getVertexData(VertexBuffer* vbuf, glm::vec3 rpos, Chunk* chunk) override {

        internalPutCube(vbuf, rpos, chunk, BlockTextures::GRASS);
    }
};

#endif //ETHERTIA_BLOCKGRASS_H
