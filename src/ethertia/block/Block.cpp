//
// Created by Dreamtowards on 2022/5/5.
//


#include <ethertia/client/render/chunk/ChunkMeshGen.h>
#include "Block.h"


void Block::internalPutCube(VertexBuffer *vbuf, World *world, glm::vec3 chunkpos, glm::vec3 rpos, TextureAtlas::AtlasFragment* frag) {

    ChunkMeshGen::putCube(vbuf, rpos, world->getLoadedChunk(chunkpos), world, frag);
}
