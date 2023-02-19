

#include <ethertia/Ethertia.h>

#include <ethertia/render/RenderEngine.h>
#include <ethertia/render/Window.h>
#include <ethertia/gui/GuiRoot.h>
#include <ethertia/gui/screen/GuiIngame.h>
#include <ethertia/gui/screen/GuiScreenMainMenu.h>
#include <ethertia/gui/screen/GuiScreenPause.h>
#include <ethertia/render/chunk/MarchingCubesMeshGen.h>
#include <ethertia/render/chunk/SurfaceNetsMeshGen.h>
#include <ethertia/world/World.h>
#include <ethertia/util/Loader.h>
#include <ethertia/util/Timer.h>
#include <ethertia/util/Scheduler.h>
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
#include <ethertia/init/ItemTextures.h>
#include <ethertia/item/Items.h>
#include <ethertia/init/MaterialMeshes.h>
#include <ethertia/mod/ModLoader.h>
//#include <ethertia/vr/OpenVR.h>


int main()
{
    Ethertia::run();


    return 0;
}


void Ethertia::start()
{
    BENCHMARK_TIMER_MSG("System initialized in {}.\n");
    Settings::loadSettings();
    Loader::checkWorkingDirectory();

    for (const std::string& modpath : Settings::MODS) {
        ModLoader::loadMod(modpath);
    }
    //OpenVR::init();

    m_Running = true;
    m_Window = new Window(Settings::displayWidth, Settings::displayHeight, Ethertia::Version::name().c_str());
    m_RootGUI = new GuiRoot();
    m_RenderEngine = new RenderEngine();  // todo RenderEngine::init();
    m_AudioEngine = new AudioEngine();  // todo AudioEngine::init();
    ChunkGenerator::initSIMD();
    Log::info("Core {}, {}, endian {}", std::thread::hardware_concurrency(), Loader::sys_target_name(), std::endian::native == std::endian::big ? "big" : "little");

    // Materials & Items
    Materials::registerMaterialItems();  // before items tex load.
    MaterialTextures::load();
    MaterialMeshes::load();
    ItemTextures::load();

    // Network
    ClientConnectionProc::initPackets();

    // Client Controls.
    Controls::initControls();

    m_Player = new EntityPlayer();  // before gui init. when gui init, needs get Player ptr. e.g. Inventory
    GuiIngame::initGUIs();

    // Proc Threads
    ChunkMeshProc::initThread();
    ChunkGenProc::initThread();
    m_AsyncScheduler.initThread();
    m_Scheduler.m_ThreadId = std::this_thread::get_id();


    m_Player->setPosition({10, 10, 10});
    m_Player->switchGamemode(Gamemode::SPECTATOR);
    m_Player->setFlying(true);

//    ReloadControl::init();
//    ReloadControl::reload("");

    // NetworkSystem::connect("127.0.0.1", 8081);


//    auto data = Loader::loadFile("cat.ogg");
//    AudioBuffer* buf = Loader::loadOGG(data);
//
//    AudioSource* src = new AudioSource();
//
//    src->QueueBuffer(buf->m_BufferId);
//    src->play();

    Ethertia::getRegistry<Material>()->dbgPrintEntries("Materials");
    Ethertia::getRegistry<Item>()->dbgPrintEntries("Items");
    Ethertia::getRegistry<Biome>()->dbgPrintEntries("Biomes");
    Ethertia::getRegistry<Command>()->dbgPrintEntries("Commands");



    if (m_AudioEngine->m_CaptureDevice)
        m_AudioEngine->startCapture();

}

void Ethertia::runMainLoop()
{
    PROFILE("Frame");
    m_Timer.update(getPreciseTime());

    {
        PROFILE("SyncTask");
        m_Scheduler.processTasks(0.01);
    }

    {
        PROFILE("Tick");

        while (m_Timer.polltick())
        {
            // runTick();
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

            m_World->tick(getDelta());
        }
    }

    {
        PROFILE("Input");

        m_Window->resetDeltas();
        glfwPollEvents();
        Controls::handleContinuousInput();
    }

    {
        PROFILE("Render");

        RenderEngine::clearRenderBuffer();

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
    glDisable(GL_CULL_FACE);

    GuiInventory::HOVER_ITEM = nullptr;

    rootGUI->onDraw();

    GuiInventory::doDrawHoveredItem();
    GuiInventory::doDrawPickingItem();

//    Gui::drawRect(0, Gui::maxHeight()-300, 300, 300, {
//        .tex = ShadowRenderer::fboDepthMap->texDepth,
//        .channel_mode = Gui::DrawRectArgs::C_RGB
//    });


    if (ChunkGenProc::dbg_SavingChunks) {
        Gui::drawString(Gui::maxWidth() - 32, Gui::maxHeight() - 32, "Saving chunks...", Colors::YELLOW, 16, {-1, -1});
    }

    if (!WorldEdit::selection.empty()) {
        AABB& s = WorldEdit::selection;
        RenderEngine::drawLineBox(s.min, s.size(), Colors::GREEN);
    }

    glEnable(GL_DEPTH_TEST);

}


void Ethertia::runTick()
{

//    if (m_World) {
//
//        m_World->tick();
//    }
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

void Ethertia::loadWorld(const std::string& savedir, const WorldInfo* worldinfo)
{
    assert(m_World == nullptr);
    assert(Ethertia::getScheduler()->numTasks() == 0);  // main-scheduler should be world-isolated. at least now.

    m_World = new World(savedir, worldinfo);
    m_World->addEntity(m_Player);

    ChunkMeshProc::g_Running = 1;
    ChunkGenProc::g_Running = 1;

    Log::info("Loading world @\"{}\" *{}", m_World->m_ChunkLoader->m_ChunkDir, m_World->m_WorldInfo.Seed);


    getScheduler()->addTask([](){
        // not now. while handling GUI press/click. shouldn't remove gui.
        Ethertia::getRootGUI()->removeAllGuis();
        Ethertia::getRootGUI()->addGui(GuiIngame::Inst());
    });

}

void Ethertia::unloadWorld()
{
    assert(m_World);
    Log::info("Unloading World...");

    Ethertia::getBrushCursor().reset();

    m_World->unloadAllChunks();

    World* oldWorld = m_World;
    m_World = nullptr;

    ChunkMeshProc::g_Running = -1;
    ChunkGenProc::g_Running = -1;

    Log::info("Cleaning MeshGen");
    Timer::wait_for(&ChunkMeshProc::g_Running, 0);
    Log::info("Cleaning ChunkGen");
    Timer::wait_for(&ChunkGenProc::g_Running, 0);

    Log::info("Sync Tasks");
    // make sure no Task remain. Task is world-isolated., Exec after other chunk-proc threads cuz they may addTask().
    m_Scheduler.processTasks(Mth::Inf);
    assert(m_Scheduler.numTasks() == 0);

    Log::info("Async Tasks");
    m_AsyncScheduler.processTasks(Mth::Inf);  // why? for what?
    assert(m_AsyncScheduler.numTasks() == 0);


    delete oldWorld;


    Log::info("World unloaded.");

    // remove gui after GuiEventPolling
    getScheduler()->addTask([](){
        Ethertia::getRootGUI()->removeAllGuis();
        Ethertia::getRootGUI()->addGui(GuiScreenMainMenu::Inst());
    });
}




void Ethertia::dispatchCommand(const std::string& cmdline) {
    if (cmdline.empty()) return;

    if (cmdline[0] != '/') {
        if (!NetworkSystem::m_Connection) {
            _SendMessage("Failed send chat, you haven't connect a server.");
            return;
        }
        NetworkSystem::SendPacket(PacketChat{ cmdline });
        return;
    }

    std::vector<std::string> args = Strings::splitSpaces(cmdline);
    int argc = args.size();
    EntityPlayer* player = Ethertia::getPlayer();

    std::string cmd = args[0].substr(1);  // sub the leading '/'

    Command* command = (Command*)Command::REGISTRY.get(cmd);
    if (!command) {
        _SendMessage("Unknown command: {} ({})", cmd, cmdline);
        return;
    }

    // Execute command

    try
    {
        command->onCommand(args);
    }
    catch (...)
    {
        std::exception_ptr p = std::current_exception();

        _SendMessage("Error occurred when command execution.");
    }

    //todo: sender? that only make sense on serverside.
}

void Ethertia::notifyMessage(const std::string& msg) {
    Log::info("[MSG/C] ", msg);
    GuiMessageList::Inst()->addMessage(msg);
}

bool Ethertia::isIngame() { return getRootGUI()->last() == GuiIngame::Inst() && !m_Window->isKeyDown(GLFW_KEY_GRAVE_ACCENT); }

float Ethertia::getPreciseTime() { return (float)Window::getPreciseTime(); }

float Ethertia::getDelta() { return m_Timer.getDelta(); }

Camera* Ethertia::getCamera() { return &RenderEngine::g_Camera; }

float Ethertia::getAspectRatio() {
    Window* w = getWindow(); if (w->getHeight() == 0) return 0;
    return (float)w->getWidth() / (float)w->getHeight();
}







void ShaderProgram::setViewProjection(bool view)
{
    setMatrix4f("matProjection", RenderEngine::matProjection);

    setMatrix4f("matView", view ? RenderEngine::matView : glm::mat4(1.0));
}

float Scheduler::_intl_program_time() {
    return Ethertia::getPreciseTime();
}




void Entity::onRender()
{
    RenderEngine::entityRenderer->renderGeometryChunk(
            m_Model, getPosition(), getRotation(), m_DiffuseMap);
}

void EntityMesh::onRender()
{
    bool isFoliage = !m_FaceCulling;  if (isFoliage && RenderEngine::dbg_NoVegetable) return;

    if (isFoliage)
        glDisable(GL_CULL_FACE);

    Ethertia::getRenderEngine()->entityRenderer->renderGeometryChunk(
            m_Model, getPosition(), getRotation(), m_DiffuseMap, 0);//isFoliage ? 0.1 : 0.0);

    if (isFoliage)
        glEnable(GL_CULL_FACE);  // set back
}


// Mth.h
namespace glm {
    std::ostream& operator<<(std::ostream& s, const glm::vec3& v) {
        s << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return s;
    }
}