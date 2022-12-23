

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
#include <ethertia/entity/EntityCar.h>
#include <ethertia/entity/EntityRaycastCar.h>
#include <ethertia/entity/player/EntityPlayer.h>
#include <ethertia/render/chunk/ChunkRenderProcessor.h>
#include <ethertia/init/Settings.h>
#include <ethertia/network/NetworkProcessor.h>
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

    NetworkProcessor::init();
    NetworkProcessor::connect("127.0.0.1", 8081);

    Controls::initConsoleThread();


    m_Player = new EntityPlayer();
    m_Player->setPosition({10, 10, 10});
    m_Player->switchGamemode(Gamemode::SPECTATOR);

    Ethertia::loadWorld();
    m_Player->setFlying(true);


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
            m_World->dynamicsWorld->stepSimulation(getDelta());
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
    NetworkProcessor::deinit();

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

void Ethertia::dispatchCommand(const std::string& cmdline) {
    std::vector<std::string> args = Strings::splitSpaces(cmdline);
    std::string& cmd = args[0];
    EntityPlayer* player = Ethertia::getPlayer();

    if (cmd == "/tp")
    {
        // /tp <x> <y> <z>
        player->setPosition(glm::vec3(std::stof(args[1]), std::stof(args[2]), std::stof(args[3])));
    }
    else if (cmd == "/gamemode")
    {
        int mode = std::stoi(args[1]);

        player->switchGamemode(mode);

        Log::info("Gamemode: {}", mode);
    }
    else if (cmd == "/fly")
    {
        player->setFlying(!player->isFlying());

        Log::info("Flymode: {}", player->isFlying());
    }
}



bool Ethertia::isIngame() { return getRootGUI()->last() == GuiIngame::INST && !m_Window->isKeyDown(GLFW_KEY_GRAVE_ACCENT); }

float Ethertia::getPreciseTime() { return (float)Window::getPreciseTime(); }

float Ethertia::getDelta() { return m_Timer.getDelta(); }

Camera* Ethertia::getCamera() { return &m_RenderEngine->m_Camera; }

float Ethertia::getAspectRatio() {
    Window* w = getWindow(); if (w->getHeight() == 0) return 0;
    return (float)w->getWidth() / (float)w->getHeight();
}
