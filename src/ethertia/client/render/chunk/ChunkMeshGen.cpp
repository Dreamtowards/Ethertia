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
        glm::vec3 dir = dirCubeFace(i);
        auto neib = Ethertia::getWorld()->getBlock(chunk->position + rpos + dir);

        if (//Chunk::outbound(adjacent) ||
            //chunk->getBlock(adjacent) == 0
            neib == 0) {
            putFace(vbuf, i, rpos, BlockTextures::SAND);
        }
    }
}

// invoke from ChunkGen thread.
void ChunkMeshGen::genMesh(Chunk* chunk)  {
    VertexBuffer* vbuf = new VertexBuffer();
    Log::info("New Buf: "+std::to_string(vbuf->vertexCount()));

    for (int rx = 0; rx < 16; ++rx) {
        for (int ry = 0; ry < 16; ++ry) {
            for (int rz = 0; rz < 16; ++rz) {

                int blockId = chunk->getBlock(rx, ry, rz);

                if (blockId == Blocks::STONE) {
                    putCube(*vbuf, glm::vec3(rx, ry, rz), chunk);
                }

            }
        }
    }

    if (vbuf->vertexCount() == 0)
        return;  // skip empty chunk.

    Ethertia::getExecutor()->exec([chunk, vbuf]() {
        chunk->model = Loader::loadModel(vbuf);
        Log::info("Loaded Chunk Model "+Log::str(chunk->model)+" VAO: "+std::to_string(chunk->model->vaoId)+" vcount: "+std::to_string(chunk->model->vertexCount));

        // todo: WHY cannot delete?
        //        delete vbuf;

    });
}