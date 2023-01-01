

#include <ethertia/Ethertia.h>

#include <ethertia/render/RenderEngine.h>
#include <ethertia/render/Window.h>
#include <ethertia/gui/GuiRoot.h>
#include <ethertia/gui/screen/GuiIngame.h>
#include <ethertia/gui/screen/GuiScreenMainMenu.h>
#include <ethertia/gui/screen/GuiScreenChat.h>
#include <ethertia/gui/screen/GuiScreenPause.h>
#include <ethertia/render/chunk/BlockyMeshGen.h>
#include <ethertia/render/chunk/MarchingCubesMeshGen.h>
#include <ethertia/render/chunk/SurfaceNetsMeshGen.h>
#include <ethertia/world/World.h>
#include <ethertia/util/Loader.h>
#include <ethertia/util/Timer.h>
#include <ethertia/util/concurrent/Scheduler.h>
#include <ethertia/util/Strings.h>
#include <ethertia/entity/Entity.h>
#include <ethertia/entity/player/EntityPlayer.h>
#include <ethertia/render/chunk/ChunkRenderProcessor.h>
#include <ethertia/init/Settings.h>
#include <ethertia/network/client/ClientConnectionProc.h>
#include <ethertia/network/client/NetworkSystem.h>
#include <ethertia/init/Controls.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>


int main()
{
    Ethertia::run();


    return 0;
}


void Ethertia::start()
{
    BenchmarkTimer _tm(nullptr, "System initialized in {}.\n");
    Settings::loadSettings();

    m_Running = true;
    m_Window = new Window(Settings::displayWidth, Settings::displayHeight, "Dysplay");
    m_RootGUI = new GuiRoot();
    m_RenderEngine = new RenderEngine();

    MaterialTextures::init();

    ChunkRenderProcessor::initWorkerThread();
    m_AsyncScheduler.initWorkerThread("Async Tasks");


    GuiIngame::INST = new GuiIngame();
    m_RootGUI->addGui(GuiIngame::INST);

    GuiScreenMainMenu::INST = new GuiScreenMainMenu();
    GuiScreenChat::INST = new GuiScreenChat();
    GuiScreenPause::INST = new GuiScreenPause();
    m_RootGUI->addGui(GuiScreenPause::INST);

    Controls::initControls();

    ClientConnectionProc::initPackets();

    NetworkSystem::init();
    // NetworkSystem::connect("127.0.0.1", 8081);


    m_Player = new EntityPlayer();
    m_Player->setPosition({10, 10, 10});
    m_Player->switchGamemode(Gamemode::SPECTATOR);
    m_Player->setFlying(true);

    Ethertia::loadWorld();


}

void Ethertia::runMainLoop()
{
    PROFILE("Frame");
    m_Timer.update(getPreciseTime());

    {
        PROFILE("SyncTask");
        m_Scheduler.processTasks(0.001);
    }

    {
        PROFILE("Tick");

        while (m_Timer.polltick())
        {
            runTick();
        }
        if (m_World)
        {
            PROFILE("Phys");
            m_World->m_DynamicsWorld->stepSimulation(getDelta());

            m_World->processEntityCollision();

        }
    }

    {
        PROFILE("Input");

        m_Window->resetDeltas();
        glfwPollEvents();
        Controls::handleInput();
    }

    {
        PROFILE("Render");
        if (m_World)
        {
            PROFILE("World");
            m_RenderEngine->renderWorld(m_World);
        }

        {
            PROFILE("GUI");
            renderGUI();
            if (m_Window->isKeyDown(GLFW_KEY_L)) {
                EntityRenderer::tmpLightDir = getCamera()->direction;
                EntityRenderer::tmpLightPos = getCamera()->position;
            }
        }
    }

    {
        PROFILE("SwapBuffer");
        m_Window->swapBuffers();
    }
}

void Ethertia::renderGUI()
{
    GuiRoot* rootGUI = Ethertia::getRootGUI();


    rootGUI->onLayout();

    rootGUI->updateHovers(Ethertia::getWindow()->getMousePos());

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    rootGUI->onDraw();

    glEnable(GL_DEPTH_TEST);

}


void Ethertia::runTick()
{

    if (m_World) {

        m_World->tick();
    }
}

void Ethertia::destroy()
{
    Settings::saveSettings();
    NetworkSystem::deinit();

    delete m_RootGUI;
    delete m_World;
    delete m_RenderEngine;

    glfwTerminate();
}

void Ethertia::loadWorld() {
    assert(m_World == nullptr);

    m_World = new World();
    m_World->addEntity(m_Player);
}

void Ethertia::unloadWorld() {


    delete m_World;
    m_World = nullptr;
}




Entity* ofCommandEntityExpr(const std::string& expr) {
    Entity* result = nullptr;
    if (expr == "@t") {
        result = Ethertia::getBrushCursor().hitEntity;
        if (!result) Log::warn("Failed, invalid target entity");
    } else if (expr == "@s") {
        result = Ethertia::getPlayer();
    }
    return result;
}

void Ethertia::dispatchCommand(const std::string& cmdline) {
    if (cmdline.empty()) return;

    if (cmdline[0] != '/') {
        if (!NetworkSystem::m_Connection) {
            Ethertia::notifyMessage("Failed send chat, you haven't connect a server.");
            return;
        }
        NetworkSystem::SendPacket(PacketChat{
                cmdline
        });
        return;
    }

    std::vector<std::string> args = Strings::splitSpaces(cmdline);
    int argc = args.size();

    std::string& cmd = args[0];
    EntityPlayer* player = Ethertia::getPlayer();

    if (cmd == "/tp")
    {
        Entity* src = player;
        if (argc == 4) { // /tp <x> <y> <z>
            src->setPosition(Mth::vec3(&args[1]));
        } else if (argc == 5) {
            src = ofCommandEntityExpr(args[1]);
            src->setPosition(Mth::vec3(&args[2]));
        } else if (argc == 3) {
            src = ofCommandEntityExpr(args[1]);
            Entity* dst = ofCommandEntityExpr(args[2]);
            src->setPosition(dst->getPosition());
        }
        Ethertia::notifyMessage("Teleported");
    }
    else if (cmd == "/gamemode")
    {
        int mode = std::stoi(args[1]);

        player->switchGamemode(mode);

        Ethertia::notifyMessage(Strings::fmt("Gamemode has been set to {}", Gamemode::nameOf(mode)));
    }
    else if (cmd == "/fly")
    {
        player->setFlying(!player->isFlying());

        Ethertia::notifyMessage(Strings::fmt("Flymode tuned to {}", player->isFlying()));
    }
    else if (cmd == "/connect")  //  /connect 127.0.0.1:8081
    {
        std::string hostname = args[1];
        int port = std::stoi(args[2]);

        NetworkSystem::connect(hostname, port);
    }
    else if (cmd == "/mesh")
    {
        if (args[1] == "new") {
            const std::string& path = args[2];
            if (!Loader::fileExists(path)){
                notifyMessage(Strings::fmt("No mesh file on: ", path));
                return;
            }

            EntityMesh* entity = new EntityMesh();
            entity->setPosition(player->getPosition());

            VertexBuffer* vbuf = Loader::loadOBJ(Loader::loadFileStr(path));
            entity->setMesh_Model(vbuf->positions.data(), Loader::loadModel(vbuf));

            Ethertia::getWorld()->addEntity(entity);

            Ethertia::notifyMessage(Strings::fmt("Added EntityMesh Model ", path));
        } else if (args[1] == "diff") {
            const std::string& path = args[2];
            if (!Loader::fileExists(path)){
                notifyMessage(Strings::fmt("No mesh file on: ", path));
                return;
            }

            EntityMesh* entity = (EntityMesh*)Ethertia::getBrushCursor().hitEntity;
            entity->m_DiffuseMap = Loader::loadTexture(Loader::loadPNG(Loader::loadFile(path)));

        }
    }
    else if (cmd == "/heal")
    {
        player->m_Health = 1.0f;
    }
    else
    {
        Ethertia::notifyMessage(Strings::fmt("Unknown command: ", cmdline));
    }
}

void Ethertia::notifyMessage(const std::string& msg) {
    Log::info("[MSG/C] ", msg);
    GuiMessageList::INST->addMessage(msg);
}

bool Ethertia::isIngame() { return getRootGUI()->last() == GuiIngame::INST && !m_Window->isKeyDown(GLFW_KEY_GRAVE_ACCENT); }

float Ethertia::getPreciseTime() { return (float)Window::getPreciseTime(); }

float Ethertia::getDelta() { return m_Timer.getDelta(); }

Camera* Ethertia::getCamera() { return &m_RenderEngine->m_Camera; }

float Ethertia::getAspectRatio() {
    Window* w = getWindow(); if (w->getHeight() == 0) return 0;
    return (float)w->getWidth() / (float)w->getHeight();
}



static void processPlayerCollide(EntityPlayer* player, float appliedImpulse, btVector3 norm,
                                 const btVector3& localpoint, const btVector3& worldpoint)
{

    player->m_AppliedImpulse += appliedImpulse;

    if (localpoint.y() < 0 && norm.dot({0, -1, 0}) > 0.5) {
        player->m_OnGround = true;
    }

}
void World::processEntityCollision() {

    // clear impulse
    Ethertia::getPlayer()->m_AppliedImpulse = 0;
    Ethertia::getPlayer()->m_OnGround = false;


    btDispatcher* disp = m_DynamicsWorld->getDispatcher();
    int numManifolds = disp->getNumManifolds();

    for (int i = 0; i < numManifolds; ++i)
    {
        btPersistentManifold* manifold = disp->getManifoldByIndexInternal(i);
        const btCollisionObject* objA = static_cast<const btCollisionObject*>(manifold->getBody0());
        const btCollisionObject* objB = static_cast<const btCollisionObject*>(manifold->getBody1());

        Entity* ptrA = static_cast<Entity*>(objA->getUserPointer());
        Entity* ptrB = static_cast<Entity*>(objB->getUserPointer());

        int numContacts = manifold->getNumContacts();
        for (int j = 0; j < numContacts; ++j)
        {
            btManifoldPoint& pt = manifold->getContactPoint(j);

            // if (pt.getDistance() <= 0)  ~< 0.03

            // const btVector3& ptA = pt.m_positionWorldOnA;
            // const btVector3& ptB = pt.m_positionWorldOnB;
            // const btVector3& normB = pt.m_normalWorldOnB;

            if (EntityPlayer* player = dynamic_cast<EntityPlayer*>(ptrA)) {
                processPlayerCollide(player, pt.getAppliedImpulse(), -pt.m_normalWorldOnB, pt.m_localPointA, pt.m_positionWorldOnA);
            }
            if (EntityPlayer* player = dynamic_cast<EntityPlayer*>(ptrB)) {
                processPlayerCollide(player, pt.getAppliedImpulse(), pt.m_normalWorldOnB, pt.m_localPointB, pt.m_positionWorldOnB);
            }

        }
    }

    EntityPlayer* player = Ethertia::getPlayer();
//    if (player->m_OnGround)
//        Log::info("onG");
    if (player->m_AppliedImpulse > 200)
        Log::info("AppliedI ", player->m_AppliedImpulse);

    if (player->m_AppliedImpulse > 300) {
        player->m_Health -= player->m_AppliedImpulse / 3000;
    }
}

//void EntityPlayer::onTick() {

    // Should not make a new Test, just check collision manifolds.
//    OnGroundCheck check;
//    m_World->m_DynamicsWorld->contactTest(m_Rigidbody, check);
//
//    m_OnGround = check.m_TestGround;

//    if (m_OnGround)
//    Log::info("OnGround ", m_OnGround);

//    if (m_OnGround && m_Gamemode == Gamemode::SURVIVAL) {
//        m_Rigidbody->setGravity({0, 0,  0});
//    } else {
//        m_Rigidbody->setGravity({0, -10,  0});
//    }
//}