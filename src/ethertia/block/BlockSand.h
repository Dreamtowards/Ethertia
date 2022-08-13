//
// Created by Dreamtowards on 2022/5/5.
//

#ifndef ETHERTIA_BLOCKSAND_H
#define ETHERTIA_BLOCKSAND_H

#include "Block.h"

class BlockSand : public Block
{

    void getVertexData(VertexBuffer* vbuf, glm::vec3 rpos, Chunk* chunk) override {

        internalPutCube(vbuf, rpos, chunk, BlockTextures::SAND);
    }
};

#endif //ETHERTIA_BLOCKSAND_H
