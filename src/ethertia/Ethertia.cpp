

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
#include <ethertia/render/chunk/proc/ChunkMeshProc.h>
#include <ethertia/render/chunk/proc/ChunkGenProc.h>
#include <ethertia/init/Settings.h>
#include <ethertia/network/client/ClientConnectionProc.h>
#include <ethertia/network/client/NetworkSystem.h>
#include <ethertia/init/Controls.h>
#include <ethertia/world/gen/ChunkGenerator.h>
#include <ethertia/command/Commands.h>
#include <ethertia/world/ChunkLoader.h>
#include <ethertia/audio/AudioEngine.h>

//#include <yaml-cpp/yaml.h>
//#include <nbt/nbt_tags.h>


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
    BENCHMARK_TIMER(nullptr, "System initialized in {}.\n");
    Settings::loadSettings();

    m_Running = true;
    m_Window = new Window(Settings::displayWidth, Settings::displayHeight, std::string("Ethertia "+Ethertia::Version::name()).c_str());
    m_RootGUI = new GuiRoot();
    m_RenderEngine = new RenderEngine();
    m_AudioEngine = new AudioEngine();
    ChunkGenerator::initSIMD();
    Log::info("Core ", std::thread::hardware_concurrency());

    MaterialTextures::init();
    GuiIngame::initGUIs();
    Commands::initCommands();
    Controls::initControls();

    ClientConnectionProc::initPackets();

    ChunkMeshProc::initThread();
    ChunkGenProc::initThread();
    m_AsyncScheduler.initThread();
    m_Scheduler.m_ThreadId = std::this_thread::get_id();


    m_Player = new EntityPlayer();

    m_Player->setPosition({10, 10, 10});
    m_Player->switchGamemode(Gamemode::SPECTATOR);
    m_Player->setFlying(true);

    // NetworkSystem::connect("127.0.0.1", 8081);



    auto data = Loader::loadFile("cat.ogg");
    AudioBuffer* buf = Loader::loadOGG(data);

    AudioSource* src = new AudioSource();

    src->QueueBuffer(buf->m_BufferId);
    src->play();



}

void Ethertia::runMainLoop()
{
    PROFILE("Frame");
    m_Timer.update(getPreciseTime());

    {
        PROFILE("SyncTask");
        m_Scheduler.processTasks(0.1);
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

            m_World->forLoadedChunks([](Chunk* chunk)
            {
                chunk->m_InhabitedTime += getDelta();
            });
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

        m_RenderEngine->clearRenderBuffer();
        if (m_World)
        {
            PROFILE("World");
            m_RenderEngine->renderWorld(m_World);
        }
        {
            PROFILE("GUI");
            renderGUI();
//            if (m_Window->isKeyDown(GLFW_KEY_L)) {
//                EntityRenderer::tmpLightDir = getCamera()->direction;
//                EntityRenderer::tmpLightPos = getCamera()->position;
//            }
        }
    }

    {
        PROFILE("SwapBuffer");
        m_Window->swapBuffers();
        AudioEngine::checkAlError("Frame");

        m_AudioEngine->setPosition(Ethertia::getCamera()->position);
        m_AudioEngine->setOrientation(Ethertia::getCamera()->direction);
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

    if (Ethertia::getWorld()) {
        Ethertia::unloadWorld();
    }

    NetworkSystem::deinit();

    delete m_RootGUI;
    delete m_RenderEngine;
    delete m_AudioEngine;

    glfwTerminate();
}

void Ethertia::loadWorld()
{
    assert(m_World == nullptr);
    assert(Ethertia::getScheduler()->numTasks() == 0);  // main-scheduler should be world-isolated. at least now.

    m_World = new World("saves/world1", 1342);
    m_World->addEntity(m_Player);

    ChunkMeshProc::g_Running = 1;
    ChunkGenProc::g_Running = 1;

    Log::info("Loading world @\"{}\" *{}", m_World->m_ChunkLoader->m_ChunkDir, m_World->m_Seed);


    Ethertia::getRootGUI()->removeAllGuis();
    Ethertia::getRootGUI()->addGui(GuiIngame::INST);
}

void Ethertia::unloadWorld()
{
    assert(m_World);

    Ethertia::getBrushCursor().reset();

    ChunkMeshProc::g_Running = -1;
    ChunkGenProc::g_Running = -1;

    m_World->unloadAllChunks();

    Timer::wait_for(&ChunkMeshProc::g_Running, 0);
    Timer::wait_for(&ChunkGenProc::g_Running, 0);

    // make sure no Task remain. Task is world-isolated., Exec after other chunk-proc threads cuz they may addTask().
    getScheduler()->processTasks(Mth::Inf);

    // tmp save
    getAsyncScheduler()->processTasks(Mth::Inf);

    assert(m_Scheduler.numTasks() == 0);

    delete m_World;
    m_World = nullptr;

    Log::info("World unloaded.");

    // remove gui after GuiEventPolling
    getScheduler()->addTask([](){
        Ethertia::getRootGUI()->removeAllGuis();
        Ethertia::getRootGUI()->addGui(GuiScreenMainMenu::INST);
    });
}




void Ethertia::dispatchCommand(const std::string& cmdline) {
    if (cmdline.empty()) return;

    if (cmdline[0] != '/') {
        if (!NetworkSystem::m_Connection) {
            Ethertia::notifyMessage("Failed send chat, you haven't connect a server.");
            return;
        }
        NetworkSystem::SendPacket(PacketChat{ cmdline });
        return;
    }

    std::vector<std::string> args = Strings::splitSpaces(cmdline);
    int argc = args.size();
    EntityPlayer* player = Ethertia::getPlayer();

    std::string cmd = args[0].substr(1);  // sub the leading '/'

    auto it = Commands::COMMANDS.find(cmd);
    if (it == Commands::COMMANDS.end()) {
        Ethertia::notifyMessage(Strings::fmt("Unknown command: {} ({})", cmd, cmdline));
        return;
    }

    // Execute command

    it->second(args);

    //todo: sender? that only make sense on serverside.
}

void Ethertia::notifyMessage(const std::string& msg) {
    Log::info("[MSG/C] ", msg);
    GuiMessageList::INST->addMessage(msg);
}

bool Ethertia::isIngame() { return getRootGUI()->last() == GuiIngame::INST; } // && !m_Window->isKeyDown(GLFW_KEY_GRAVE_ACCENT); }

float Ethertia::getPreciseTime() { return (float)Window::getPreciseTime(); }

float Ethertia::getDelta() { return m_Timer.getDelta(); }

Camera* Ethertia::getCamera() { return &m_RenderEngine->m_Camera; }

float Ethertia::getAspectRatio() {
    Window* w = getWindow(); if (w->getHeight() == 0) return 0;
    return (float)w->getWidth() / (float)w->getHeight();
}






#include <stb/stb_vorbis.c>


AudioBuffer* Loader::loadOGG(std::pair<char*, size_t> data) {
    int channels = 0;
    int sample_rate = 0;
    int16_t* pcm = nullptr;
    int len = stb_vorbis_decode_memory((unsigned char*)data.first, data.second, &channels, &sample_rate, &pcm);
    if (len == -1)
        throw std::runtime_error("failed decode ogg.");
    assert(pcm);

    // Log::info("Load ogg, pcm size {}, freq {}, cnls {}", Strings::size_str(len), sample_rate, channels);
    assert(channels == 2 || channels == 1);
    ALuint format = channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;

    AudioBuffer* buf = new AudioBuffer();
    buf->buffer_data(format, pcm, len, sample_rate);
    return buf;
}