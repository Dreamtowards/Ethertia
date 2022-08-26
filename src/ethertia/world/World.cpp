//
// Created by Dreamtowards on 2022/4/22.
//

#include "World.h"

#include <ethertia/client/Loader.h>
#include <ethertia/client/render/chunk/BlockyChunkMeshGen.h>
#include <ethertia/client/Ethertia.h>


//void World::tmpDoRebuildModel(Chunk* chunk, World* world) {
//    auto* vbuf = ChunkMeshGen::genMesh(chunk, world);
//    if (vbuf) {
//        Ethertia::getExecutor()->exec([chunk, vbuf]() {
//            delete chunk->model;
//            chunk->model = Loader::loadModel(vbuf);
//            delete vbuf;
//        });
//    }
//}

bool isNeighboursAllLoaded(World* world, glm::vec3 chunkpos) {
    return world->getLoadedChunk(chunkpos + glm::vec3(-16, 0, 0)) &&
           world->getLoadedChunk(chunkpos + glm::vec3(16, 0, 0)) &&
           world->getLoadedChunk(chunkpos + glm::vec3(0, -16, 0)) &&
           world->getLoadedChunk(chunkpos + glm::vec3(0, 16, 0)) &&
           world->getLoadedChunk(chunkpos + glm::vec3(0, 0, -16)) &&
           world->getLoadedChunk(chunkpos + glm::vec3(0, 0, 16));
}

void tryPopulate(World* world, glm::vec3 chunkpos) {
    Chunk* c = world->getLoadedChunk(chunkpos);
    if (c && !c->populated && isNeighboursAllLoaded(world, chunkpos)) {  // && isNeighbourAllLoaded()
        World::populate(world, chunkpos);
        c->populated = true;
    }
}

Chunk* World::provideChunk(glm::vec3 p) {
    Chunk* chunk = getLoadedChunk(p);
    if (chunk) return chunk;
    glm::vec3 chunkpos = Chunk::chunkpos(p);

    // chunk = chunkLoader.loadChunk();

    if (!chunk) {
        chunk = chunkGenerator.generateChunk(chunkpos, this);
    }

    chunks[chunkpos] = chunk;

    // check populates
//    tryPopulate(this, chunkpos + glm::vec3(0, 0, 0));
//    tryPopulate(this, chunkpos + glm::vec3(-16, 0, 0));
//    tryPopulate(this, chunkpos + glm::vec3(16, 0, 0));
//    tryPopulate(this, chunkpos + glm::vec3(0, -16, 0));
//    tryPopulate(this, chunkpos + glm::vec3(0, 16, 0));
//    tryPopulate(this, chunkpos + glm::vec3(0, 0, -16));
//    tryPopulate(this, chunkpos + glm::vec3(0, 0, 16));

    return chunk;
}


