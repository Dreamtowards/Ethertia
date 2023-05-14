//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_WORLD_H
#define ETHERTIA_WORLD_H

#include <unordered_map>
#include <mutex>
#include <glm/vec3.hpp>
#include <glm/gtx/hash.hpp>
#include <entt/entt.hpp>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

#include <ethertia/world/Cell.h>
#include <ethertia/util/Scheduler.h>
#include <ethertia/item/ItemStack.h>

class Entity;
class Chunk;
class ChunkGenerator;
class ChunkLoader;

struct WorldInfo
{

    uint64_t Seed = 0;
    std::string Name;

    float DayTime = 0.25;        // 0-1. 0: midnight, = t*24h.; 0->0AM, 0.25->6AM, 0.5->0PM, 0.75->6PM
    float DayLength = 12*60;  // seconds per day.
    float InhabitedTime = 0;  // seconds world running.

    int ProtocolVersion = 0;
    uint64_t SavedTime = 0;

    inline static const float WORLD_SAVE_INTERVAL = 2*60;

    static uint64_t ofSeed(const char* str) {
        if (!str[0])  // empty
            return Timer::timestampMillis();
        try {
            return std::stoll(str);
        } catch (std::invalid_argument err) {
            // Log::info("Not a number seed, use string hash instead.");
            return std::hash<std::string>()(str);
        }
    }

    struct WorldGenProperties
    {
        enum WorldType
        {
            SUPER_FLAT
        };
    };

};

class World
{
public:

    ChunkGenerator* m_ChunkGenerator = nullptr;
    ChunkLoader*    m_ChunkLoader = nullptr;

    // Loaded Chunks
    std::unordered_map<glm::vec3, Chunk*> m_Chunks;
    std::mutex m_LockChunks;

    // Unlaoded Chunks waiting to be batch-save.
    std::vector<Chunk*> m_UnloadedChunks;
    std::mutex m_LockUnloadedChunks;


    // Loaded Entities
    std::vector<Entity*> m_Entities;
    std::mutex m_LockEntities;


    // Bullet Physics.
    btDynamicsWorld* m_DynamicsWorld = nullptr;

    WorldInfo m_WorldInfo;

    entt::registry m_EnttRegistry;


    // worldinfo always nullptr (auto read.) except the first Create World.
    World(const std::string& savedir, const WorldInfo* worldinfo = nullptr);
    ~World();

    Cell& getCell(glm::vec3 p);
    Cell& getCell(int x, int y, int z) { return getCell({x,y,z}); }

    void setCell(glm::vec3 p, const Cell& c);
    void setCell(int x, int y, int z, const Cell& c) { setCell({x,y,z}, c); }

    void requestRemodel(glm::vec3 p, bool detectNeighbour = true);

    void invalidateCellFp(glm::vec3 center, int r);

    void processEntityCollision();  // main thread.




    Chunk* provideChunk(glm::vec3 p);

    void unloadChunk(glm::vec3 p);

    void saveUnloadedChunks();

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

    template<typename T>
    std::vector<T> getEntities_();



    void dropItem(glm::vec3 pos, const ItemStack& stack, glm::vec3 vel = {0,0,0});

    // include drop item
    void digCell(glm::vec3 p);

    // void addParticle();

    // void createExplosion();

    void onTick(float dt);


    float getDayTime() {
        return m_WorldInfo.DayTime;
    }

    uint64_t getSeed() {
        return m_WorldInfo.Seed;
    }











    bool raycast(glm::vec3 begin, glm::vec3 end, glm::vec3& p, glm::vec3& n, btCollisionObject** obj) const;


    static void populate(World* world, glm::vec3 chunkpos);


    static Cell& _GetCell(Chunk* chunk, glm::vec3 rp);


//    struct DayTime
//    {
//        // 6AM - 6PM.
//        static bool isDay(float daytime) {
//            return daytime > 0.25 && daytime < 0.75;
//        }
//    };
};

#endif //ETHERTIA_WORLD_H
