//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_WORLD_H
#define ETHERTIA_WORLD_H

#include <unordered_map>
#include <glm/vec3.hpp>

#include <ethertia/world/chunk/Chunk.h>
#include <ethertia/world/gen/ChunkGenerator.h>
#include <ethertia/init/Blocks.h>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include "glm/gtx/string_cast.hpp"

class World
{
    std::unordered_map<glm::vec3, Chunk*> loadedChunks;

    ChunkGenerator chunkGenerator;

public:

    ubyte getBlock(glm::vec3 blockpos) {
        Chunk* chunk = getLoadedChunk(blockpos);
        if (!chunk) return 0;
        return chunk->getBlock(blockpos);
    }
    ubyte getBlock(int x, int y, int z) {
        return getBlock(glm::vec3(x,y,z));
    }

    void setBlock(glm::vec3 blockpos, ubyte blockID) {
        Chunk* chunk = getLoadedChunk(blockpos);
        if (!chunk) return;
        chunk->setBlock(blockpos, blockID);
    }
    void setBlock(int x, int y, int z, ubyte blockID) {
        setBlock(glm::vec3(x,y,z), blockID);
    }

    Chunk* provideChunk(glm::vec3 p);

    void unloadChunk(glm::vec3 p) {
        auto itr = loadedChunks.find(Chunk::chunkpos(p));
        if (itr == loadedChunks.end())
            throw std::logic_error("Failed unload chunk. Not exists. "+glm::to_string(p));
        delete itr->second;
        loadedChunks.erase(itr);
    }

    Chunk* getLoadedChunk(glm::vec3 p) {
        return loadedChunks[Chunk::chunkpos(p)];
    }

    std::unordered_map<glm::vec3, Chunk*> getLoadedChunks() {
        return loadedChunks;
    }




    static void populate(World* world, glm::vec3 chunkpos)
    {
        for (int dx = 0; dx < 16; ++dx) {
            for (int dz = 0; dz < 16; ++dz) {
                int x = chunkpos.x + dx;
                int z = chunkpos.z + dz;
                int nextAir = -1;

                for (int dy = 0; dy < 16; ++dy) {
                    int y = chunkpos.y + dy;
                    ubyte tmpbl = world->getBlock(x, y, z);
                    if (tmpbl == 0 || tmpbl == Blocks::WATER)
                        continue;

                    if (nextAir < dy) {
                        for (int i = 0;; ++i) {
                            if (world->getBlock(x, y+i, z) == 0) {
                                nextAir = dy+i;
                                break;
                            }
                        }
                    }

                    int nextToAir = nextAir - dy;

                    ubyte replace = Blocks::STONE;
                    if (y < 3 && nextToAir < 3 && world->chunkGenerator.noise.noise(x/60.0, y/60.0, z/60.0) > 0.1) {
                        replace = Blocks::SAND;
                    } else if (nextToAir == 1) {
                        replace = Blocks::GRASS;
                    } else if (nextToAir < 4) {
                        replace = Blocks::DIRT;
                    }
                    world->setBlock(x, y, z, replace);
                }
            }
        }

    }
};

#endif //ETHERTIA_WORLD_H
