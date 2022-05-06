//
// Created by Dreamtowards on 2022/5/5.
//

#ifndef ETHERTIA_BLOCKGRASS_H
#define ETHERTIA_BLOCKGRASS_H

#include "Block.h"

class BlockGrass : public Block
{

    void getVertexData(VertexBuffer* vbuf, World* world, glm::vec3 chunkpos, glm::vec3 rpos) override {

        internalPutCube(vbuf, world, chunkpos, rpos, BlockTextures::GRASS);
    }
};

#endif //ETHERTIA_BLOCKGRASS_H
