//
// Created by Dreamtowards on 2022/7/8.
//

#ifndef ETHERTIA_BLOCKLOG_H
#define ETHERTIA_BLOCKLOG_H

#include "Block.h"

class BlockLog : public Block
{

    void getVertexData(VertexBuffer *vbuf, World *world, glm::vec3 chunkpos, glm::vec3 rpos) override {

        internalPutCube(vbuf, world, chunkpos, rpos, BlockTextures::LOG);
    }

};

#endif //ETHERTIA_BLOCKLOG_H
