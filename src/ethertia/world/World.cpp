//
// Created by Dreamtowards on 2022/4/22.
//

#include "World.h"

#include <ethertia/client/Loader.h>
#include <ethertia/client/render/chunk/ChunkMeshGen.h>
#include <ethertia/client/Ethertia.h>


void tmpDoRebuildModel(Chunk* chunk, World* world) {
    auto* vbuf = ChunkMeshGen::genMesh(chunk, world);
    if (vbuf) {
        Ethertia::getExecutor()->exec([chunk, vbuf]() {
            delete chunk->model;
            chunk->model = Loader::loadModel(vbuf);
            delete vbuf;
        });
    }
}

Chunk* World::provideChunk(glm::vec3 p) {
    Chunk* chunk = getLoadedChunk(p);
    if (chunk) return chunk;
    glm::vec3 chunkpos = Chunk::chunkpos(p);

    // chunk = chunkLoader.loadChunk();

    if (!chunk) {
        chunk = chunkGenerator.generateChunk(chunkpos);
    }

    loadedChunks[chunkpos] = chunk;

    // check populates
    Chunk* down = getLoadedChunk(chunkpos - glm::vec3(0, 16, 0));
    if (down && !down->populated) {
        populate(this, down->position);
        down->populated = true;
        Log::info("Populated");
        tmpDoRebuildModel(down, this);
    }

    return chunk;
}
