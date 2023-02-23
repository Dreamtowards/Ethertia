//
// Created by Dreamtowards on 2022/12/31.
//

#include <ethertia/world/World.h>

#include <ethertia/world/Chunk.h>
#include <ethertia/entity/Entity.h>
#include <ethertia/entity/EntityMesh.h>
#include <ethertia/entity/player/EntityPlayer.h>
#include <ethertia/entity/EntityDroppedItem.h>
#include <ethertia/util/Timer.h>
#include <ethertia/util/Collections.h>
#include <ethertia/util/Mth.h>
#include <ethertia/util/Scheduler.h>
#include <ethertia/Ethertia.h>
#include <ethertia/render/Window.h>
#include <ethertia/world/ChunkLoader.h>
#include <ethertia/world/gen/ChunkGenerator.h>
#include <ethertia/render/RenderEngine.h>
#include <ethertia/render/chunk/proc/ChunkGenProc.h>


World::World(const std::string& savedir, const WorldInfo* worldinfo)
{

    m_ChunkLoader = new ChunkLoader(savedir);  //  = "saves/dim-1"

    if (worldinfo) {
        m_WorldInfo = *worldinfo;
    } else {
        m_ChunkLoader->loadWorldInfo(m_WorldInfo);
    }

    m_ChunkGenerator = new ChunkGenerator();


    // init Phys
    {
        btCollisionConfiguration* collconf = new btDefaultCollisionConfiguration();
        btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collconf);
        btBroadphaseInterface* broadphase = new btDbvtBroadphase();
        btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();

        m_DynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, nullptr);
    }
}

World::~World() {

    m_ChunkLoader->saveWorldInfo(m_WorldInfo);

    for (int i = m_DynamicsWorld->getNumCollisionObjects()-1; i >= 0; --i) {
        btCollisionObject* obj = m_DynamicsWorld->getCollisionObjectArray()[i];

        m_DynamicsWorld->removeCollisionObject(obj);
    }
    // todo: delete entities.

    delete ((btCollisionDispatcher*)m_DynamicsWorld->getDispatcher())->getCollisionConfiguration();
    delete m_DynamicsWorld->getDispatcher();
    delete m_DynamicsWorld->getConstraintSolver();
    delete m_DynamicsWorld->getBroadphase();  // why after dw
    delete m_DynamicsWorld;

    m_DynamicsWorld = nullptr;
}

Cell& World::getCell(glm::vec3 p)  {
    Chunk* chunk = getLoadedChunk(p);
    if (!chunk) {
        static Cell EMP{0,0};
        return EMP;  // shouldn't
    }
    return chunk->getCell(Chunk::rpos(p));
}

void World::setCell(glm::vec3 p, const Cell& c)  {
    Chunk* chunk = getLoadedChunk(p);
    if (!chunk) return;
    glm::ivec3 bp = Chunk::rpos(p);
    chunk->setCell(bp, c);
}


static bool isNeighbourChunksAllLoaded(World* world, glm::vec3 chunkpos) {
    return world->getLoadedChunk(chunkpos + glm::vec3(-16, 0, 0)) &&
           world->getLoadedChunk(chunkpos + glm::vec3(16, 0, 0)) &&
           world->getLoadedChunk(chunkpos + glm::vec3(0, -16, 0)) &&
           world->getLoadedChunk(chunkpos + glm::vec3(0, 16, 0)) &&
           world->getLoadedChunk(chunkpos + glm::vec3(0, 0, -16)) &&
           world->getLoadedChunk(chunkpos + glm::vec3(0, 0, 16));
}

static void tryPopulate(World* world, glm::vec3 chunkpos) {
    Chunk* c = world->getLoadedChunk(chunkpos);
    if (c && !c->m_Populated && isNeighbourChunksAllLoaded(world, chunkpos)) {  // && isNeighbourAllLoaded()
        World::populate(world, chunkpos);
        c->m_Populated = true;
    }
}

Chunk* World::provideChunk(glm::vec3 p)  {
    Chunk* chunk = getLoadedChunk(p);
    if (chunk) return chunk;
    glm::vec3 chunkpos = Chunk::chunkpos(p);

    {
        PROFILE_X(ChunkGenProc::gp_ChunkGen, "Load");  // not accurate. many times wouldn't load

        dbg_ChunkProvideState = 2;
        chunk = m_ChunkLoader->loadChunk(chunkpos, this);
        dbg_ChunkProvideState = 0;
    }

    if (!chunk)
    {
        PROFILE_X(ChunkGenProc::gp_ChunkGen, "Gen");

        dbg_ChunkProvideState = 1;
        chunk = m_ChunkGenerator->generateChunk(chunkpos, this);
        dbg_ChunkProvideState = 0;
    }

    {
        LOCK_GUARD(m_LockChunks);
        m_Chunks[chunkpos] = chunk;
    }

    Ethertia::getScheduler()->addTask([this, chunk, chunkpos]()
    {
        // bug: CNS 这个延时操作，可能刚添加进来的区块 立刻被卸载了，
        if (!getLoadedChunk(chunkpos))
            return;  // already been unloaded/deleted.
        addEntity(chunk->m_MeshTerrain);
        addEntity(chunk->m_MeshVegetable);
    });

    PROFILE_X(ChunkGenProc::gp_ChunkGen, "Popula");
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


void World::saveUnloadedChunks() {
    // chunks will be force save when: 1. 太多区块在卸载列表 2. 太久没保存
    LOCK_GUARD(m_LockUnloadedChunks);

    int i = 0;
    for (Chunk* chunk : m_UnloadedChunks)
    {
        dbg_SavingChunks = m_UnloadedChunks.size() - i;
        if (ChunkGenProc::g_Running == -1) {  // when unloadWorld().
            Log::info("Saving chunk {} ({}/{})", chunk->position, i+1, m_UnloadedChunks.size()); ++i;
        }

        m_ChunkLoader->saveChunk(chunk);

        Entity* meshTerr = chunk->m_MeshTerrain;
        Entity* meshVege = chunk->m_MeshVegetable;

        Ethertia::getScheduler()->addTask([this, meshTerr, meshVege]()
        {
            // CNS 有时Entity根本不会被加入世界 (当加入世界时发现区块已被卸载 将取消加入世界
            // ps 真的假的 为什么我现在不明白了
            if (meshTerr->m_World) {
                removeEntity(meshTerr);
                removeEntity(meshVege);
            }
        }, -1);  // ? after addEntity().


        // Delete chunk but after MeshGen thread finish use.
        if (chunk->m_MeshingState == Chunk::MESHING) {
//            Ethertia::getAsyncScheduler()->addDelayTask([chunk](){
            Log::info("UnloadChunk waiting for MeshDone.");
            Timer::wait_for([&](){return chunk->m_MeshingState == Chunk::MESH_VALID || chunk->m_MeshingState == Chunk::MESH_INVALID;});
                delete chunk;
//            }, 2.0f);
        } else {
            delete chunk;
        }
    }
    dbg_SavingChunks = 0;

    m_UnloadedChunks.clear();

}

void World::unloadChunk(glm::vec3 p) {
    Chunk* chunk = nullptr;
    {
        LOCK_GUARD(m_LockChunks);
        auto it = m_Chunks.find(Chunk::chunkpos(p));
        if (it == m_Chunks.end())
            throw std::logic_error(Strings::fmt("Failed unload chunk. Not exists. {}", p));
        chunk = it->second;
        assert(chunk);

        // later save.
        {
            LOCK_GUARD(m_LockUnloadedChunks);
            m_UnloadedChunks.push_back(chunk);
        }

        m_Chunks.erase(it);
    }
    chunk->m_World = nullptr;  // mesh gen needed for delay remove. but MeshUpload need immediate know is unloaded

}

Chunk* World::getLoadedChunk(glm::vec3 p, bool quick) {
    if (!quick)
        LOCK_GUARD(m_LockChunks);

    auto it = m_Chunks.find(Chunk::chunkpos(p));
    if (it == m_Chunks.end())
        return nullptr;
    return it->second;
}



void World::addEntity(Entity* e)
{
    assert(Ethertia::inMainThread());  // Ensure main thread.
    assert(e != nullptr);
    assert(Collections::find(m_Entities, e) == -1);  // make sure the entity is not in this world.

    e->m_World = this;
    m_Entities.push_back(e);

    auto old_grav = e->m_Rigidbody->getGravity(); // fix little stupid auto-change gravity

    // add entity bodies to DynamicsWorld.
    e->onLoad(m_DynamicsWorld);

    e->m_Rigidbody->setGravity(old_grav);
}

void World::removeEntity(Entity* e)
{
    assert(Ethertia::inMainThread());
    assert(e);
    assert(Collections::find(m_Entities, e) != -1);

    Collections::erase(m_Entities, e);

    e->onUnload(m_DynamicsWorld);

    e->m_World = nullptr;
}

const std::vector<Entity*>& World::getEntities() {
    assert(Ethertia::inMainThread());
    return m_Entities;
}

template<typename T>
std::vector<T> World::getEntities_() {  // just temporary
    assert(Ethertia::inMainThread());
    std::vector<T> filtered;
    for (void* e : m_Entities) {
        if (dynamic_cast<T>(e)) {
            filtered.push_back(e);
        }
    }
    return filtered;
}


void World::dropItem(glm::vec3 p, const ItemStack& stack, glm::vec3 vel)
{
    assert(!stack.empty());
    EntityDroppedItem* eDroppedItem = new EntityDroppedItem();
    eDroppedItem->m_DroppedItem = stack;
    eDroppedItem->setPosition(p);
    eDroppedItem->applyLinearVelocity(vel);
    addEntity(eDroppedItem);
}

void World::onTick(float dt)
{

    m_WorldInfo.InhabitedTime += dt;
    m_WorldInfo.DayTime = Mth::mod(m_WorldInfo.DayTime + dt/m_WorldInfo.DayLength, 1.0f);
}


bool World::raycast(glm::vec3 begin, glm::vec3 end, glm::vec3& p, glm::vec3& n, btCollisionObject** obj) const {
    btVector3 _beg(begin.x, begin.y, begin.z);
    btVector3 _end(end.x, end.y, end.z);
    btCollisionWorld::ClosestRayResultCallback _raycallback(_beg, _end);

    m_DynamicsWorld->rayTest(_beg, _end, _raycallback);
    if (_raycallback.hasHit()) {
        p = Mth::vec3(_raycallback.m_hitPointWorld);
        n = Mth::vec3(_raycallback.m_hitNormalWorld);

        if (obj) {
            *obj = (btCollisionObject*)_raycallback.m_collisionObject;
        }
        return true;
    }
    return false;
}




void World::requestRemodel(glm::vec3 p, bool detectNeighbour) {
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

void World::invalidateCellFp(glm::vec3 center, int n)
{
    for (int rx = -n; rx <= n; ++rx) {
        for (int ry = -n; ry <= n; ++ry) {
            for (int rz = -n; rz <= n; ++rz) {

                getCell(center+glm::vec3(rx,ry,rz)).fp.x = INFINITY;
            }
        }
    }
}


Cell& World::_GetCell(Chunk* chunk, glm::vec3 rp)  {
    if (!Ethertia::getWorld()) {
        static Cell EMP{0,0};
        return EMP;  // No..
    }
    return Chunk::outbound(rp) ? /*chunk->m_World avoid interrupt Meshing*/Ethertia::getWorld()->getCell(chunk->position + rp) : chunk->getCell(rp);
}


void World::populate(World* world, glm::vec3 chunkpos) {
    float noiseSand[16*16];
    float noiseFern[16*16];

    ChunkGenerator::Perlin()->GenUniformGrid2D(noiseSand, chunkpos.x, chunkpos.z, 16, 16, 1/60.0f, world->getSeed());

    for (int dx = 0; dx < 16; ++dx) {
        for (int dz = 0; dz < 16; ++dz) {
            int x = chunkpos.x + dx;
            int z = chunkpos.z + dz;
            int nextAir = -1;
            const int idxXZ = ChunkGenerator::IdxXZ(dx, dz);

            for (int dy = 0; dy < 16; ++dy) {
                int y = chunkpos.y + dy;
                Cell tmpbl = world->getCell(x, y, z);
                if (tmpbl.mtl != Materials::STONE)  // AIR or WATER.
                    continue;

                if (nextAir < dy) {
                    for (int i = 0; i < 16 - dy; ++i) {
                        if (world->getCell(x, y + i, z).mtl == 0) {
                            nextAir = dy + i;
                            break;
                        }
                    }
                }

                int nextToAir = nextAir - dy;

                Material* replace = Materials::STONE;
                if (y < 3 && nextToAir < 3 && noiseSand[idxXZ] > 0.1) {
                    replace = Materials::SAND;
                } else if (nextToAir == 1 //|| nextToAir == 2
                        ) {
                    bool tallgrass = noiseFern[idxXZ] > 0.2;
                    replace = tallgrass ? Materials::MOSS : Materials::GRASS;
                } else if (nextToAir < 4) {
                    replace = Materials::DIRT;
                }
                world->getCell(x, y, z).mtl = replace;
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
                    if (world->getCell(x, y, z).mtl == Materials::STONE &&
                        world->getCell(x, y - 1, z).mtl == 0) {


                        for (int i = 0; i < 32 * Mth::hash(x * y * 47923); ++i) {

                            Cell &c = world->getCell(x, y - 1 - i, z);
                            if (c.mtl)
                                break;
                            c.mtl = Materials::LEAVES;
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

            if (Mth::hash(x * z) < (1.5 / 256.0f)) {  // Make Tree
                for (int dy = 0; dy < 16; ++dy) {
                    int y = chunkpos.y + dy;
                    if (world->getCell(x, y, z).mtl == Materials::GRASS) {

                        float f = Mth::hash(x * z * y);
                        int trunkHeight = 3 + f * 8;
                        int leavesRadius = 2 + f * 5;

                        Material* _leaf = Materials::LEAVES;
//                            if (f > 0.8f) {
//                                h *= 1.6f;
//                                _leaf = Blocks::LEAVES_JUNGLE;
//                            } else if (f < 0.2f) _leaf = Blocks::LEAVES_APPLE;


                        // Leaves
                        for (int lx = -leavesRadius; lx <= leavesRadius; ++lx) {
                            for (int lz = -leavesRadius; lz <= leavesRadius; ++lz) {
                                for (int ly = -leavesRadius; ly <= leavesRadius + f * trunkHeight; ++ly) {
                                    if (Mth::sq(Mth::abs(lx)) + Mth::sq(Mth::abs(lz)) + Mth::sq(Mth::abs(ly)) >=
                                        leavesRadius * leavesRadius)
                                        continue;
//                                        if (_leaf == Blocks::LEAVES && Mth::hash(x*y*z) < 0.2f)
//                                            _leaf = Blocks::LEAVES_APPLE;
                                    //y +Mth::hash(y)*4
//                                        world->setCell(x+lx, y+ly+h, z+lz, Cell(Materials::LEAVES, 0.0f));
                                    Cell& c_leaf = world->getCell(x + lx, y + ly + trunkHeight, z + lz);
                                    c_leaf.mtl = Materials::LEAVES;
                                    c_leaf.density = Mth::min(c_leaf.density, 0.0f);
                                }
                            }
                        }
                        // Trunk
                        for (int i = 0; i < trunkHeight; ++i) {

                            world->setCell({x, y + i, z},
                                           Cell(Materials::LOG, 2.0f * (1.2f - (float) i / trunkHeight)));
                        }
                        break;
                    }
                }
            }
        }
    }
}







static void processPlayerCollide(EntityPlayer* player, float appliedImpulse, const btVector3& norm,
                                 const btVector3& localpoint, const btVector3& worldpoint)
{

    player->m_AppliedImpulse += appliedImpulse;

    if (localpoint.y() < 0 && norm.dot({0, -1, 0}) > 0.5) {
        player->m_OnGround = true;
    }

    player->m_NumContactPoints++;

}
void World::processEntityCollision() {

    {
        // reset
        EntityPlayer* player = Ethertia::getPlayer();
        player->m_AppliedImpulse = 0;
        player->m_OnGround = false;
        player->m_NumContactPoints = 0;
    }


    btDispatcher* disp = m_DynamicsWorld->getDispatcher();
    int numManifolds = disp->getNumManifolds();

    for (int i = 0; i < numManifolds; ++i)
    {
        btPersistentManifold* manifold = disp->getManifoldByIndexInternal(i);
        const btCollisionObject* colA = static_cast<const btCollisionObject*>(manifold->getBody0());
        const btCollisionObject* colB = static_cast<const btCollisionObject*>(manifold->getBody1());

        Entity* ptrA = static_cast<Entity*>(colA->getUserPointer());
        Entity* ptrB = static_cast<Entity*>(colB->getUserPointer());

        EntityPlayer* player = Collections::ptr_or(dynamic_cast<EntityPlayer*>(ptrA), dynamic_cast<EntityPlayer*>(ptrB));
        bool playerIsA = player == ptrA;
//        if (!player) {
//            player = dynamic_cast<EntityPlayer*>(ptrB);
//            playerIsA = false;
//        }
        assert(!player || dynamic_cast<EntityPlayer*>(player));

        // player pick item
        if (player && Ethertia::getWindow()->isKeyDown(GLFW_KEY_P)) {
            EntityDroppedItem* eDroppedItem = dynamic_cast<EntityDroppedItem*>(playerIsA ? ptrB : ptrA);
            if (eDroppedItem) {
                removeEntity(eDroppedItem);
                ItemStack& stack = eDroppedItem->m_DroppedItem;
                player->m_Inventory.putItemStack(stack);
                delete eDroppedItem;  // there?
            }
        } else {
            // item auto merge
            EntityDroppedItem* itemA = dynamic_cast<EntityDroppedItem*>(ptrA);
            EntityDroppedItem* itemB = dynamic_cast<EntityDroppedItem*>(ptrB);
            if (itemA && itemB)
            {
                if (itemA->m_DroppedItem.stackableWith(itemB->m_DroppedItem)) {
                    removeEntity(itemA);
                    itemA->m_DroppedItem.moveTo(itemB->m_DroppedItem);
                    delete itemA;
                }
            }
        }


        // player collision impact.
        if (player) {
            int numContacts = manifold->getNumContacts();
            for (int j = 0; j < numContacts; ++j)
            {
                btManifoldPoint& pt = manifold->getContactPoint(j);
                // if (pt.getDistance() <= 0)  ~< 0.03
                // const btVector3& ptA = pt.m_positionWorldOnA;
                // const btVector3& ptB = pt.m_positionWorldOnB;
                // const btVector3& normB = pt.m_normalWorldOnB;

                if (playerIsA) {
                    processPlayerCollide(player, pt.getAppliedImpulse(), -pt.m_normalWorldOnB, pt.m_localPointA, pt.m_positionWorldOnA);
                } else {
                    processPlayerCollide(player, pt.getAppliedImpulse(), pt.m_normalWorldOnB, pt.m_localPointB, pt.m_positionWorldOnB);
                }
            }
        }
    }

    EntityPlayer* player = Ethertia::getPlayer();
//    if (player->m_OnGround)
//        Log::info("onG");
//    if (player->m_AppliedImpulse > 200)
//        Log::info("AppliedI ", player->m_AppliedImpulse);

    if (player->m_NumContactPoints) {
        btVector3 velDiff = player->m_PrevVelocity - player->m_Rigidbody->getLinearVelocity();
        float f = velDiff.length();
        if (f > 10) {
//            Log::info("Diff ", f);
            player->m_Health -= f / 8.0f;
        }
    }

    if (player->m_AppliedImpulse > 300) {
//        player->m_Health -= player->m_AppliedImpulse / 3000;
    }
}
