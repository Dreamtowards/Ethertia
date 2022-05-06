//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_BLOCK_H
#define ETHERTIA_BLOCK_H

#include <ethertia/client/render/VertexBuffer.h>
#include <ethertia/init/BlockTextures.h>

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

    virtual void getVertexData(VertexBuffer* vbuf, World* world, glm::vec3 chunkpos, glm::vec3 rpos) = 0;


    void internalPutCube(VertexBuffer* vbuf, World* world, glm::vec3 chunkpos, glm::vec3 rpos, TextureAtlas::AtlasFragment* frag);
};

#endif //ETHERTIA_BLOCK_H
