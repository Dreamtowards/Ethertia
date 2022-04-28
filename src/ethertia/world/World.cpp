//
// Created by Dreamtowards on 2022/4/22.
//

#include "World.h"

#include <ethertia/client/Loader.h>
#include <ethertia/client/render/chunk/ChunkMeshGen.h>
#include <ethertia/client/Ethertia.h>



Chunk* World::provideChunk(glm::vec3 p) {
    Chunk* chunk = getLoadedChunk(p);
    if (chunk) return chunk;
    glm::vec3 chunkpos = Chunk::chunkpos(p);

    // chunk = chunkLoader.loadChunk();

    if (!chunk) {
        chunk = chunkGenerator.generateChunk(chunkpos);
    }

    loadedChunks[chunkpos] = chunk;

    auto* vbuf = ChunkMeshGen::genMesh(chunk, this);
    if (vbuf) {
        Ethertia::getExecutor()->exec([=]() {
            chunk->model = Loader::loadModel(vbuf);
            delete vbuf;
        });
    }
    return chunk;
}
