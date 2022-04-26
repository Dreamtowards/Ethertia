//
// Created by Dreamtowards on 2022/4/25.
//

#include "ChunkMeshGen.h"

#include <ethertia/client/Ethertia.h>

float ChunkMeshGen::CUBE_POS[] = {
        // Left -X
        0, 0, 1, 0, 1, 1, 0, 1, 0,
        0, 0, 1, 0, 1, 0, 0, 0, 0,
        // Right +X
        1, 0, 0, 1, 1, 0, 1, 1, 1,
        1, 0, 0, 1, 1, 1, 1, 0, 1,
        // Bottom -Y
        0, 0, 1, 0, 0, 0, 1, 0, 0,
        0, 0, 1, 1, 0, 0, 1, 0, 1,
        // Bottom +Y
        0, 1, 1, 1, 1, 1, 1, 1, 0,
        0, 1, 1, 1, 1, 0, 0, 1, 0,
        // Front -Z
        0, 0, 0, 0, 1, 0, 1, 1, 0,
        0, 0, 0, 1, 1, 0, 1, 0, 0,
        // Back +Z
        1, 0, 1, 1, 1, 1, 0, 1, 1,
        1, 0, 1, 0, 1, 1, 0, 0, 1,
};

float ChunkMeshGen::CUBE_UV[] = {
        1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,  // One Face.
        1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
        1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
        1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
        1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
        1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0,
};

float ChunkMeshGen::CUBE_NORM[] = {
       -1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,
        1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
        0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0,
        0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
        0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1, 0, 0,-1,
        0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1
};

void ChunkMeshGen::putCube(VertexBuffer &vbuf, glm::vec3 rpos, Chunk* chunk) {

    for (int i = 0; i < 6; ++i) {
        glm::vec3 adjacent = rpos+dirCubeFace(i);

        if (Chunk::outbound(adjacent) ||
            chunk->getBlock(adjacent) == 0) {  // use world.getBlock? but local chunk faster.
            putFace(vbuf, i, rpos, BlockTextures::SAND);
        }
    }
}