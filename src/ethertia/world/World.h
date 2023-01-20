//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_WORLD_H
#define ETHERTIA_WORLD_H

#include <unordered_map>
#include <mutex>
#include <glm/vec3.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/string_cast.hpp>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include <entt/entt.hpp>

#include <ethertia/world/Cell.h>
#include <ethertia/util/concurrent/Scheduler.h>

class Entity;
class Chunk;
class ChunkGenerator;
class ChunkLoader;

class World
{
    std::unordered_map<glm::vec3, Chunk*> m_Chunks;

    std::vector<Entity*> m_Entities;

    ChunkGenerator* m_ChunkGenerator = nullptr;

public:
    ChunkLoader* m_ChunkLoader = nullptr;

    btDynamicsWorld* m_DynamicsWorld = nullptr;

    std::mutex m_LockChunks;
    std::mutex m_LockEntities;

    uint32_t m_Seed = 0;

    // Seconds Per Day
    float m_DayTimeScale = 12*60;

    // [0, 1] t*24; 0: 0AM, 0.25: 6AM, 0.5: 0PM, 0.75: 6PM
    float m_DayTime = 0;

    // Total Seconds.
    float m_InhabitedTime = 0;

    entt::registry m_EnttRegistry;


    World(const std::string& savedir, uint32_t seed);
    ~World();

    Cell& getCell(glm::vec3 p);
    Cell& getCell(int x, int y, int z) { return getCell({x,y,z}); }

    void setCell(glm::vec3 p, const Cell& c);
    void setCell(int x, int y, int z, const Cell& c) { setCell({x,y,z}, c); }

    void requestRemodel(glm::vec3 p, bool detectNeighbour = true);

    void processEntityCollision();




    Chunk* provideChunk(glm::vec3 p);

    void unloadChunk(glm::vec3 p);

    // Quick: Effective find, not lock, no thread-safe.
    Chunk* getLoadedChunk(glm::vec3 p, bool quick = false);

    const std::unordered_map<glm::vec3, Chunk*>& getLoadedChunks() {
        return m_Chunks;
    }

    void forLoadedChunks(const std::function<void(Chunk*)>& fn) {
        LOCK_GUARD(m_LockChunks);
        for (auto& it : m_Chunks) {
            fn(it.second);
        }
    }
    void unloadAllChunks() { using glm::vec3;
        std::vector<vec3> posls;
        {
            LOCK_GUARD(m_LockChunks);
            for (auto& it : m_Chunks) {
                posls.push_back(it.first);
            }
        }
        for (vec3& p : posls) {
            unloadChunk(p);
        }
    }

    void addEntity(Entity* e);

    void removeEntity(Entity* e);

    const std::vector<Entity*>& getEntities();


    void tick(float dt);














    bool raycast(glm::vec3 begin, glm::vec3 end, glm::vec3& p, glm::vec3& n, btCollisionObject** obj) const;


    static void populate(World* world, glm::vec3 chunkpos);


    static Cell& _GetCell(Chunk* chunk, glm::vec3 rp);
};

#endif //ETHERTIA_WORLD_H
