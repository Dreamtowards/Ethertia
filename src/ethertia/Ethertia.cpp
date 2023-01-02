

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
#include <ethertia/world/gen/ChunkGenerator.h>


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
    ChunkGenerator::initSIMD();
    m_RootGUI = new GuiRoot();
    m_RenderEngine = new RenderEngine();

    MaterialTextures::init();

    ChunkRenderProcessor::initWorkerThread();
    m_AsyncScheduler.initWorkerThread("Async Tasks");

    GuiIngame::initGUIs();

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
        m_Scheduler.processTasks(0.005);
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
            m_Player->m_PrevVelocity = m_Player->m_Rigidbody->getLinearVelocity();
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
    else if (cmd == "/entity")
    {
        if (args[1] == "new")
        {
            if (args[2] == "mesh")
            {
                EntityMesh* entity = new EntityMesh();
                entity->setPosition(player->getPosition());

                VertexBuffer* vbuf = Loader::loadOBJ(Loader::loadAssetsStr("entity/plane.obj"));
                entity->setMesh(vbuf->vertexCount(), vbuf->positions.data());
                entity->updateModel(Loader::loadModel(vbuf));

                Ethertia::getWorld()->addEntity(entity);

                Ethertia::notifyMessage(Strings::fmt("EntityMesh created."));
            }
        }
        else
        {
            Entity* target = Ethertia::getBrushCursor().hitEntity;
            if (!target) {
                notifyMessage("failed, no target entity.");
                return;
            }

            if (args[1] == "mesh")
            {
                const std::string& path = args[2];
                if (!Loader::fileExists(path)){
                    notifyMessage(Strings::fmt("No mesh file on: ", path));
                    return;
                }

                EntityMesh* eMesh = (EntityMesh*)target;

                VertexBuffer* vbuf = Loader::loadOBJ(Loader::loadFileStr(path));
                eMesh->updateModel(Loader::loadModel(vbuf));
                eMesh->setMesh(vbuf->vertexCount(), vbuf->positions.data());

                notifyMessage("Mesh updated.");
            }
            else if (args[1] == "diff")
            {
                const std::string& path = args[2];
                if (!Loader::fileExists(path)){
                    notifyMessage(Strings::fmt("No texture file on: ", path));
                    return;
                }

                EntityMesh* entity = (EntityMesh*)Ethertia::getBrushCursor().hitEntity;
                entity->m_DiffuseMap = Loader::loadTexture(Loader::loadPNG(Loader::loadFile(path)));

                notifyMessage("Texture updated.");
            }
            else if (args[1] == "delete")
            {

                Ethertia::getWorld()->removeEntity(target);

                notifyMessage("Entity deleted.");
            }

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

