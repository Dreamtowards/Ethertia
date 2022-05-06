//
// Created by Dreamtowards on 2022/5/5.
//

#ifndef ETHERTIA_BLOCKSAND_H
#define ETHERTIA_BLOCKSAND_H

#include "Block.h"

class BlockSand : public Block
{

    void getVertexData(VertexBuffer* vbuf, World* world, glm::vec3 chunkpos, glm::vec3 rpos) override {

        internalPutCube(vbuf, world, chunkpos, rpos, BlockTextures::SAND);
    }
};

#endif //ETHERTIA_BLOCKSAND_H
