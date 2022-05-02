//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_BLOCK_H
#define ETHERTIA_BLOCK_H

#include <ethertia/client/render/VertexBuffer.h>
#include <ethertia/world/World.h>
#include <ethertia/client/render/chunk/ChunkMeshGen.h>

class Block
{

    virtual void getVertexData(VertexBuffer& vbuf, World* world, glm::vec3 chunkpos, glm::vec3 rpos) {

        ChunkMeshGen::putCube(vbuf, rpos, world->getLoadedChunk(chunkpos), chunkpos, world, BlockTextures::GRASS);
    }
};

#endif //ETHERTIA_BLOCK_H
