//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_BLOCK_H
#define ETHERTIA_BLOCK_H

#include <ethertia/client/render/VertexBuffer.h>
#include <assets/etc/_depreca/BlockTextures.h>

class World;

class Block
{


public:

    virtual bool isOpaque() {
        return true;
    }

    // isCollidable
    // getAABB
    // getHardness
    // getResistance
    // replaceable

    /// vbuf: produced vertices buffer. positions: InChunk Coordinate.
    /// rpos: InChunk blockpos
    /// chunk: could get world, chunkpos
    virtual void getVertexData(VertexBuffer* vbuf, glm::vec3 rpos, Chunk* chunk) = 0;


    void internalPutCube(VertexBuffer* vbuf, glm::vec3 rpos, Chunk* chunk, TextureAtlas::Region* frag);

    void internalPutLeaves(VertexBuffer* vbuf, glm::vec3 rpos, Chunk* chunk, TextureAtlas::Region* frag, float randf = 0.65f);
};

#endif //ETHERTIA_BLOCK_H
