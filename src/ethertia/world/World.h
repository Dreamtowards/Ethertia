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

    static void collideAABB(const AABB& self, glm::vec3& d, const AABB& coll) {


        if (d.y != 0 && AABB::intersectsAxis(self, coll, 0) && AABB::intersectsAxis(self, coll, 2)) {
            if (d.y < 0 && self.min.y >= coll.max.y) {
                d.y = absmin(d.y, coll.max.y - self.min.y);
            } else if (d.y > 0 && self.max.y <= coll.min.y) {
                d.y = absmin(d.y, coll.min.y - self.max.y);
            }
        }
        if (d.x != 0 && AABB::intersectsAxis(self, coll, 1) && AABB::intersectsAxis(self, coll, 2)) {
            if (d.x < 0 && self.min.x >= coll.max.x) {
                d.x = absmin(d.x, coll.max.x - self.min.x);
            } else if (d.x > 0 && self.max.x <= coll.min.x) {
                d.x = absmin(d.x, coll.min.x - self.max.x);
            }
        }
        if (d.z != 0 && AABB::intersectsAxis(self, coll, 0) && AABB::intersectsAxis(self, coll, 1)) {
            if (d.z < 0 && self.min.z >= coll.max.z) {
                d.z = absmin(d.z, coll.max.z - self.min.z);
            } else if (d.z > 0 && self.max.z <= coll.min.z) {
                d.z = absmin(d.z, coll.min.z - self.max.z);
            }
        }

    }

    static float absmin(float a, float b) {
        return Mth::abs(a) < Mth::abs(b) ? a : b;
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
                if (!Eth::getWindow()->isAltKeyDown()) {
                    glm::vec3& v = e->velocity;

                    glm::vec3 pp = e->prevposition;
                    AABB self = AABB({pp - glm::vec3(0.5f)},
                                     {pp + glm::vec3(0.5f)});

                    glm::vec3 d = e->position - e->prevposition;
                    const glm::vec3 od = d;

                    glm::vec3 bmin = glm::floor(glm::min(e->position, e->prevposition) - glm::vec3(0.5f));
                    glm::vec3 bmax = glm::ceil(glm::max(e->position, e->prevposition) + glm::vec3(0.5f));

                    for (int dx = bmin.x; dx < bmax.x; ++dx) {
                        for (int dy = bmin.y; dy < bmax.y; ++dy) {
                            for (int dz = bmin.z; dz < bmax.z; ++dz) {

                                glm::vec3 min(dx, dy, dz);
                                glm::vec3 max = min + glm::vec3(1);

                                u8 b = getBlock(min);
                                if (!b) continue;

                                collideAABB(self, d, AABB(min, max));
                            }
                        }
                    }


                    if (d.x != od.x) v.x = 0;
                    if (d.y != od.y) v.y = 0;
                    if (d.z != od.z) v.z = 0;

                    e->position = e->prevposition + d;

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
