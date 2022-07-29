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
#include <ethertia/entity/Entity.h>
#include <ethertia/util/Timer.h>
#include <ethertia/util/Collections.h>
#include <ethertia/util/Mth.h>

#include "ethertia/util/Eth.h"
#include "ethertia/client/Window.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include "glm/gtx/string_cast.hpp"

class World
{
    std::unordered_map<glm::vec3, Chunk*> chunks;

    std::vector<Entity*> entities;

    ChunkGenerator chunkGenerator;

public:

    u8 getBlock(glm::vec3 blockpos) {
        Chunk* chunk = getLoadedChunk(blockpos);
        if (!chunk) return 0;
        return chunk->getBlock(blockpos);
    }
    u8 getBlock(int x, int y, int z) {
        return getBlock(glm::vec3(x,y,z));
    }

    void setBlock(glm::vec3 blockpos, u8 blockID) {
        Chunk* chunk = getLoadedChunk(blockpos);
        if (!chunk) return;
        chunk->setBlock(blockpos, blockID);
    }
    void setBlock(int x, int y, int z, u8 blockID) {
        setBlock(glm::vec3(x,y,z), blockID);
    }

    Chunk* provideChunk(glm::vec3 p);

    void unloadChunk(glm::vec3 p) {
        auto itr = chunks.find(Chunk::chunkpos(p));
        if (itr == chunks.end())
            throw std::logic_error("Failed unload chunk. Not exists. "+glm::to_string(p));
        delete itr->second;
        chunks.erase(itr);
    }

    Chunk* getLoadedChunk(glm::vec3 p) {
        return chunks[Chunk::chunkpos(p)];
    }

    std::unordered_map<glm::vec3, Chunk*> getLoadedChunks() {
        return chunks;
    }

    void addEntity(Entity* e) {

        entities.push_back(e);
    }

    void removeEntity(Entity* e) {
        Collections::erase(entities, e);
    }

    static void collideAABB(glm::vec3& p, glm::vec3& v, glm::vec3 min, glm::vec3 max) {



    }

    void tick() {
        float dt = Timer::T_DELTA;

        for (Entity* e : entities) {
            // motion
            e->prevposition = e->position;

            e->position += e->velocity * dt;

            float vels = glm::length(e->velocity);
            float linearDamping = 0.001f;
            e->velocity *= vels < 0.01 ? 0 : Mth::pow(linearDamping, dt);

            // collide
            // get aabb of blocks that inside (prevpos, currpos)
            // for each axis xyz, test nearest plane, and clip position & velocity

//            for (int dx = Mth::floor(e->position.x); dx < Mth::ceil(e->prevposition.x); ++dx) {
//                for (int dy = Mth::floor(e->position.y); dy < Mth::ceil(e->prevposition.y); ++dy) {
//                    for (int dz = Mth::floor(e->position.z); dz < Mth::ceil(e->prevposition.z); ++dz) {
//
//                    }
//                }
//            }

            {
                u8 b = getBlock(e->position);
                if (b && !Eth::getWindow()->isAltKeyDown()) {
                    glm::vec3& v = e->velocity;

                    glm::vec3 pp = e->prevposition;
                    glm::vec3 d = e->position - e->prevposition;

                    glm::vec3 min = Mth::floor(e->position, 1);
                    glm::vec3 max = min + glm::vec3(1);


//                    e->velocity *= 0;
//                    e->position = e->prevposition;

                    // minimal penetration

//                    float pn[6] = {min.x - p.x,
//                                   max.x - p.x,
//                                   min.y - p.y,
//                                   max.y - p.y,
//                                   min.z - p.z,
//                                   max.z - p.z};
//
//                    int mp = 0;
//                    for (int i = 1; i < 6; ++i) {
//                        if (Mth::abs(pn[i]) < Mth::abs(pn[mp]))
//                            mp = i;
//                    }
//
//                    Log::info("Coll T{}", mp);
//
//
//                    if (mp == 0) {
//                        p.x = min.x;
//                        v.x = 0;
//                    } else if (mp == 1) {
//                        p.x = max.x;
//                        v.x = 0;
//                    } else if (mp == 2) {
//                        p.y = min.y;
//                        v.y = 0;
//                    } else if (mp == 3) {
//                        p.y = max.y;
//                        v.y = 0;
//                    } else if (mp == 4) {
//                        p.z = min.z;
//                        v.z = 0;
//                    } else if (mp == 5) {
//                        p.z = max.z;
//                        v.z = 0;
//                    }

                    if (v.y != 0 && AABB::intersects(min, max, pp, 0) && AABB::intersects(min, max, pp, 2)) {
                        if (v.y < 0 && pp.y <= max.y) {
                            dp.y = max.y;
                            v.y = 0;
                        } else if (v.y > 0 && p.y >= min.y) {
                            dp.y = min.y;
                            v.y = 0;
                        }
                        Log::info("Coll Y{}");
                    }
                    if (v.x != 0 && AABB::intersects(min, max, dp, 1) && AABB::intersects(min, max, dp, 2)) {
                        if (v.x < 0 && p.x <= max.x) {
                            dp.x = max.x;
                            v.x = 0;
                        } else if (v.x > 0 && p.x >= min.x) {
                            dp.x = min.x;
                            v.x = 0;
                        }
                    }
                    if (v.z != 0 && AABB::intersects(min, max, dp, 0), AABB::intersects(min, max, dp, 1)) {
                        if (v.z < 0 && p.z <= max.z) {
                            dp.z = max.z;
                            v.z = 0;
                        } else if (v.z > 0 && p.z >= min.z) {
                            dp.z = min.z;
                            v.z = 0;
                        }
                    }

                    e->position = dp;

                }
            }



        }

    }


    static void tmpDoRebuildModel(Chunk* chunk, World* world);

    static void populate(World* world, glm::vec3 chunkpos)
    {
        for (int dx = 0; dx < 16; ++dx) {
            for (int dz = 0; dz < 16; ++dz) {
                int x = chunkpos.x + dx;
                int z = chunkpos.z + dz;
                int nextAir = -1;

                for (int dy = 0; dy < 16; ++dy) {
                    int y = chunkpos.y + dy;
                    u8 tmpbl = world->getBlock(x, y, z);
                    if (tmpbl == 0 || tmpbl == Blocks::WATER)
                        continue;
                    if (tmpbl != Blocks::STONE)
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

        for (int dx = 0; dx < 16; ++dx) {
            for (int dz = 0; dz < 16; ++dz) {
                int x = chunkpos.x + dx;
                int z = chunkpos.z + dz;

                if (Mth::hash(x*z) < (2.5/256.0f)) {
                    for (int dy = 0; dy < 16; ++dy) {
                        int y = chunkpos.y + dy;
                        if (world->getBlock(x, y, z) == Blocks::GRASS) {

                            float f = Mth::hash(x*z*y);
                            int h = 3+f*8;
                            int r = 3;
                            for (int lx = -r; lx <= r; ++lx) {
                                for (int lz = -r; lz <= r; ++lz) {
                                    for (int ly = -r; ly <= r*f*3; ++ly) {
                                        if (Mth::sq(Mth::abs(lx)) + Mth::sq(Mth::abs(lz)) + Mth::sq(Mth::abs(ly*f)) > r*r)
                                            continue;
                                        world->setBlock(x+lx, y+ly+h+Mth::hash(y)*4, z+lz, Blocks::LEAVES);
                                    }
                                }
                            }
                            for (int i = 0; i < h; ++i) {

                                world->setBlock(x, y+i+1, z, Blocks::LOG);
                            }
                            break;
                        }
                    }
                }
            }
        }

    }
};

#endif //ETHERTIA_WORLD_H
