//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_WORLD_H
#define ETHERTIA_WORLD_H

#include <unordered_map>
#include <mutex>
#include <glm/vec3.hpp>

#include <ethertia/world/Chunk.h>
#include <ethertia/world/ChunkLoader.h>
#include <ethertia/world/gen/ChunkGenerator.h>
#include <ethertia/entity/Entity.h>
#include <ethertia/entity/EntityMesh.h>
#include <ethertia/util/Timer.h>
#include <ethertia/util/Collections.h>
#include <ethertia/util/Mth.h>
#include <ethertia/util/concurrent/Scheduler.h>
#include <ethertia/Ethertia.h>
#include <ethertia/render/Window.h>
#include <ethertia/world/Octree.h>
#include <ethertia/world/Cell.h>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

//#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/gtx/string_cast.hpp>

class World
{
    std::unordered_map<glm::vec3, Chunk*> chunks;

    std::vector<Entity*> entities;

    ChunkGenerator chunkGenerator;

    ChunkLoader chunkLoader{"saves/dim-1/"};

public:
    btDiscreteDynamicsWorld* dynamicsWorld;

    std::mutex lock_ChunkList;

    World() {

        // init Phys
        {
            btCollisionConfiguration* collconf = new btDefaultCollisionConfiguration();
            btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collconf);

            btBroadphaseInterface* broadphase = new btDbvtBroadphase();

            btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();

            dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collconf);
        }
    }

    ~World() {

        delete dynamicsWorld->getConstraintSolver();
        delete dynamicsWorld->getDispatcher();
        delete dynamicsWorld;
        delete dynamicsWorld->getBroadphase();
    }

    Cell& getCell(glm::vec3 p) {
        Chunk* chunk = getLoadedChunk(p);
        if (!chunk) return Materials::STAT_EMPTY;  // shouldn't
        return chunk->getCell(Chunk::rpos(p));
    }
    Cell& getCell(int x, int y, int z) {
        return getCell(glm::vec3(x,y,z));
    }

    void setCell(glm::vec3 p, const Cell& c) {
        Chunk* chunk = getLoadedChunk(p);
        if (!chunk) return;
        glm::ivec3 bp = Chunk::rpos(p);
        chunk->setCell(bp, c);

    }
    void setCell(int x, int y, int z, const Cell& m) {
        setCell(glm::vec3(x,y,z), m);
    }
    void requestRemodel(glm::vec3 p, bool detectNeighbour = true) {
        Chunk* chunk = getLoadedChunk(p);
        if (!chunk) return;
        glm::ivec3 bp = Chunk::rpos(p);

        chunk->requestRemodel();

        if (detectNeighbour) {
            Chunk* tmp = nullptr;
            if (bp.x == 0 && (tmp=getLoadedChunk(p - glm::vec3(1, 0, 0)))) tmp->requestRemodel();
            if (bp.x == 15 && (tmp=getLoadedChunk(p + glm::vec3(1, 0, 0)))) tmp->requestRemodel();
            if (bp.y == 0 && (tmp=getLoadedChunk(p - glm::vec3(0, 1, 0)))) tmp->requestRemodel();
            if (bp.y == 15 && (tmp=getLoadedChunk(p + glm::vec3(0, 1, 0)))) tmp->requestRemodel();
            if (bp.z == 0 && (tmp=getLoadedChunk(p - glm::vec3(0, 0, 1)))) tmp->requestRemodel();
            if (bp.z == 15 && (tmp=getLoadedChunk(p + glm::vec3(0, 0, 1)))) tmp->requestRemodel();
        }
    }










    Chunk* provideChunk(glm::vec3 p) {
        Chunk* chunk = getLoadedChunk(p);
        if (chunk) return chunk;
        glm::vec3 chunkpos = Chunk::chunkpos(p);

        chunk = chunkLoader.loadChunk(chunkpos, this);

        if (!chunk) {
            chunk = chunkGenerator.generateChunk(chunkpos, this);
        }

        assert(chunk);

        chunks[chunkpos] = chunk;

        Ethertia::getScheduler()->addTask([=]() {

            addEntity(chunk->m_MeshTerrain);
            addEntity(chunk->m_MeshVegetable);
        });

        // check populates
    tryPopulate(this, chunkpos + glm::vec3(0, 0, 0));
    tryPopulate(this, chunkpos + glm::vec3(-16, 0, 0));
    tryPopulate(this, chunkpos + glm::vec3(16, 0, 0));
    tryPopulate(this, chunkpos + glm::vec3(0, -16, 0));
    tryPopulate(this, chunkpos + glm::vec3(0, 16, 0));
    tryPopulate(this, chunkpos + glm::vec3(0, 0, -16));
    tryPopulate(this, chunkpos + glm::vec3(0, 0, 16));

        return chunk;
    }

    bool isNeighbourChunksAllLoaded(World* world, glm::vec3 chunkpos) {
        return world->getLoadedChunk(chunkpos + glm::vec3(-16, 0, 0)) &&
               world->getLoadedChunk(chunkpos + glm::vec3(16, 0, 0)) &&
               world->getLoadedChunk(chunkpos + glm::vec3(0, -16, 0)) &&
               world->getLoadedChunk(chunkpos + glm::vec3(0, 16, 0)) &&
               world->getLoadedChunk(chunkpos + glm::vec3(0, 0, -16)) &&
               world->getLoadedChunk(chunkpos + glm::vec3(0, 0, 16));
    }
    void tryPopulate(World* world, glm::vec3 chunkpos) {
        Chunk* c = world->getLoadedChunk(chunkpos);
        if (c && !c->populated && isNeighbourChunksAllLoaded(world, chunkpos)) {  // && isNeighbourAllLoaded()
            World::populate(world, chunkpos);
            c->populated = true;
        }
    }











    void unloadChunk(glm::vec3 p) {
        auto it = chunks.find(Chunk::chunkpos(p));
        if (it == chunks.end())
            throw std::logic_error("Failed unload chunk. Not exists. "+glm::to_string(p));

        Chunk* chunk = it->second;
        if (chunk) {
            removeEntity(chunk->m_MeshTerrain);
            removeEntity(chunk->m_MeshVegetable);
        }

        chunks.erase(it);
        delete chunk;
    }

    Chunk* getLoadedChunk(glm::vec3 p) {
        auto it = chunks.find(Chunk::chunkpos(p));
        if (it == chunks.end())
            return nullptr;
        return it->second;
    }

    std::unordered_map<glm::vec3, Chunk*>& getLoadedChunks() {
        return chunks;
    }

    void addEntity(Entity* e) {
        assert(Collections::find(entities, e) == -1);  // make sure the entity is not in this world.

        entities.push_back(e);
        e->onLoad(dynamicsWorld);
    }

    void removeEntity(Entity* e) {
        Collections::erase(entities, e);
        e->onUnload(dynamicsWorld);
    }

    std::vector<Entity*>& getEntities() {
        return entities;
    }














    bool raycast(glm::vec3 begin, glm::vec3 end, glm::vec3& p, glm::vec3& n) {
        btVector3 _beg(begin.x, begin.y, begin.z);
        btVector3 _end(end.x, end.y, end.z);
        btCollisionWorld::ClosestRayResultCallback rayCallback(_beg, _end);

        dynamicsWorld->rayTest(_beg, _end, rayCallback);
        if (rayCallback.hasHit()) {
            p = Mth::vec3(rayCallback.m_hitPointWorld);
            n = Mth::vec3(rayCallback.m_hitNormalWorld);
            return true;
        }
        return false;
    }



    void tick() {
        float dt = Timer::T_DELTA;

//        dynamicsWorld->stepSimulation(dt);

//        return;
//        for (Entity* e : entities) {
//            // motion
//            e->prevposition = e->position;
//
//            e->position += e->velocity * dt;
//
//            float vels = glm::length(e->velocity);
//            float linearDamping = 0.001f;
//            e->velocity *= vels < 0.01 ? 0 : Mth::pow(linearDamping, dt);
//
//            // collide
//            // get aabb of blocks that inside (prevpos, currpos)
//            // for each axis xyz, test nearest plane, and clip position & velocity
//
////            for (int dx = Mth::floor(e->position.x); dx < Mth::ceil(e->prevposition.x); ++dx) {
////                for (int dy = Mth::floor(e->position.y); dy < Mth::ceil(e->prevposition.y); ++dy) {
////                    for (int dz = Mth::floor(e->position.z); dz < Mth::ceil(e->prevposition.z); ++dz) {
////
////                    }
////                }
////            }
//
//            {
//                if (false ) {  // || !Ethertia::getWindow()->isAltKeyDown()
//                    glm::vec3& v = e->velocity;
//
//                    glm::vec3 pp = e->prevposition;
//                    AABB self = AABB({pp - glm::vec3(0.5f)},
//                                     {pp + glm::vec3(0.5f)});
//
//                    glm::vec3 d = e->position - e->prevposition;
//                    const glm::vec3 od = d;
//
//                    glm::vec3 bmin = glm::floor(glm::min(e->position, e->prevposition) - glm::vec3(0.5f));
//                    glm::vec3 bmax = glm::ceil(glm::max(e->position, e->prevposition) + glm::vec3(0.5f));
//
//                    for (int dx = bmin.x; dx < bmax.x; ++dx) {
//                        for (int dy = bmin.y; dy < bmax.y; ++dy) {
//                            for (int dz = bmin.z; dz < bmax.z; ++dz) {
//
//                                glm::vec3 min(dx, dy, dz);
//                                glm::vec3 max = min + glm::vec3(1);
//
//                                BlockState b = getBlock(min);
//                                if (!b.id) continue;
//
//                                collideAABB(self, d, AABB(min, max));
//                            }
//                        }
//                    }
//
//
//                    if (d.x != od.x) v.x = 0;
//                    if (d.y != od.y) v.y = 0;
//                    if (d.z != od.z) v.z = 0;
//
//                    e->position = e->prevposition + d;
//
//                }
//            }
//
//
//
//        }

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
                    Cell tmpbl = world->getCell(x, y, z);
//                    if (tmpbl.id == 0)// || tmpbl.id == Blocks::WATER)
//                        continue;
                    if (tmpbl.id != Materials::STONE)
                        continue;

                    if (nextAir < dy) {
                        for (int i = 0;i < 16-dy; ++i) {
                            if (world->getCell(x, y+i, z).id == 0) {
                                nextAir = dy+i;
                                break;
                            }
                        }
                    }

                    int nextToAir = nextAir - dy;

                    u8 replace = Materials::STONE;
                    if (y < 3 && nextToAir < 3 && world->chunkGenerator.noise.noise(x/60.0, y/60.0, z/60.0) > 0.1) {
                        replace = Materials::SAND;
                    } else if (nextToAir == 1 //|| nextToAir == 2
                              ) {
                        replace = Materials::GRASS;
                    } else if (nextToAir < 4) {
                        replace = Materials::DIRT;
                    }
                    world->getCell(x, y, z).id = replace;
                }
            }

        }

        // Grass
//        for (int dx = 0; dx < 16; ++dx) {
//            for (int dz = 0; dz < 16; ++dz) {
//                int x = chunkpos.x + dx;
//                int z = chunkpos.z + dz;
//
//                float f = Mth::hash(x * z * 100);
//                if (f < (60 / 256.0f)) {
//
//                    for (int dy = 0; dy < 16; ++dy) {
//                        int y = chunkpos.y + dy;
//                        if (world->getCell(x, y, z).id == Materials::GRASS) {
//                            u8 b = Materials::LEAVES;
////                            if (f < (4/256.0f)) b = Blocks::RED_TULIP;
////                            else if (f < (30/256.0f)) b = Blocks::SHRUB;
////                            else if (f < (40/256.0f)) b = Blocks::FERN;
//
//                            world->getCell(x, y+1, z).id = b;
//                        }
//                    }
//                }
//            }
//        }

        // Vines
        for (int dx = 0; dx < 16; ++dx) {
            for (int dz = 0; dz < 16; ++dz) {
                int x = chunkpos.x + dx;
                int z = chunkpos.z + dz;

                if (Mth::hash(x * z * 2349242) < (12.0f / 256.0f)) {
                    for (int dy = 0; dy < 16; ++dy) {
                        int y = chunkpos.y + dy;
                        if (world->getCell(x, y, z).id == Materials::STONE &&
                            world->getCell(x, y-1, z).id == 0) {


                            for (int i = 0; i < 32 * Mth::hash(x*y*47923); ++i) {

                                Cell& c = world->getCell(x, y-1-i, z);
                                if (c.id)
                                    break;
                                c.id = Materials::LEAVES;
                            }

                            break;
                        }
                    }
                }
            }
        }

        // Trees
        for (int dx = 0; dx < 16; ++dx) {
            for (int dz = 0; dz < 16; ++dz) {
                int x = chunkpos.x + dx;
                int z = chunkpos.z + dz;

                if (Mth::hash(x*z) < (1.5/256.0f)) {  // Make Tree
                    for (int dy = 0; dy < 16; ++dy) {
                        int y = chunkpos.y + dy;
                        if (world->getCell(x, y, z).id == Materials::GRASS) {

                            float f = Mth::hash(x*z*y);
                            int trunkHeight = 3+f*8;
                            int leavesRadius = 2 + f * 5;

                            u8 _leaf = Materials::LEAVES;
//                            if (f > 0.8f) {
//                                h *= 1.6f;
//                                _leaf = Blocks::LEAVES_JUNGLE;
//                            } else if (f < 0.2f) _leaf = Blocks::LEAVES_APPLE;


                             // Leaves
                            for (int lx = -leavesRadius; lx <= leavesRadius; ++lx) {
                                for (int lz = -leavesRadius; lz <= leavesRadius; ++lz) {
                                    for (int ly = -leavesRadius; ly <= leavesRadius + f * trunkHeight; ++ly) {
                                        if (Mth::sq(Mth::abs(lx)) + Mth::sq(Mth::abs(lz)) + Mth::sq(Mth::abs(ly)) >= leavesRadius * leavesRadius)
                                            continue;
//                                        if (_leaf == Blocks::LEAVES && Mth::hash(x*y*z) < 0.2f)
//                                            _leaf = Blocks::LEAVES_APPLE;
                                        //y +Mth::hash(y)*4
//                                        world->setCell(x+lx, y+ly+h, z+lz, Cell(Materials::LEAVES, 0.0f));
                                        Cell& c_leaf = world->getCell(x+lx, y+ly+trunkHeight, z+lz);
                                        c_leaf.id = Materials::LEAVES;
                                        c_leaf.density = Mth::min(c_leaf.density, 0.0f);
                                    }
                                }
                            }
                            // Trunk
                            for (int i = 0; i < trunkHeight; ++i) {

                                world->setCell(x, y+i, z, Cell(Materials::LOG, 2.0f * (1.2f - (float)i / trunkHeight)));
                            }
                            break;
                        }
                    }
                }
            }
        }

    }



    static Cell& _GetCell(Chunk* chunk, glm::vec3 rp) {
        return Chunk::outbound(rp) ? chunk->world->getCell(chunk->position + rp) : chunk->getCell(rp);
    }
};

#endif //ETHERTIA_WORLD_H
