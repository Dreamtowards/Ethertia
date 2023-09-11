

#include <ethertia/Ethertia.h>

#include <ethertia/render/RenderEngine.h>
#include <ethertia/render/Window.h>
#include <ethertia/world/World.h>
#include <ethertia/util/Loader.h>
#include <ethertia/util/Timer.h>
#include <ethertia/util/Scheduler.h>
#include <ethertia/entity/Entity.h>
#include <ethertia/entity/player/EntityPlayer.h>
#include <ethertia/render/chunk/proc/ChunkMeshProc.h>
#include <ethertia/render/chunk/proc/ChunkGenProc.h>
#include <ethertia/init/Settings.h>
#include <ethertia/network/client/ClientConnectionProc.h>
//#include <ethertia/network/client/NetworkSystem.h>
#include <ethertia/init/Controls.h>
#include <ethertia/world/gen/ChunkGenerator.h>
//#include <ethertia/command/Commands.h>
#include <ethertia/world/ChunkLoader.h>
// #include <ethertia/audio/AudioEngine.h>
#include <ethertia/init/ItemTextures.h>
#include <ethertia/init/MaterialMeshes.h>
#include <ethertia/mod/ModLoader.h>
//#include <ethertia/vr/OpenVR.h>
//#include <ethertia/init/Sounds.h>
#include <ethertia/item/recipe/Recipes.h>

#include <ethertia/imgui/Imgui.h>
#include <ethertia/world/Biome.h>
#include <ethertia/imgui/Imw.h>


static void Init();
static void Destroy();
static void RunMainLoop();


int main()
{
    Init();

    while (Ethertia::isRunning())
    {
        RunMainLoop();
    }

    Destroy();

    return 0;
}

static Window*      g_Window = nullptr;
static World*       g_World = nullptr;
static EntityPlayer*g_Player = nullptr;
static Scheduler    g_Scheduler;        // main thread tasks executor.
static Scheduler    g_AsyncScheduler;
static Timer        g_Timer;
static HitCursor    g_HitCursor;
static Profiler     g_Profiler;
static Camera       g_Camera;


// System Initialization.
static void Init()
{
    BENCHMARK_TIMER_MSG("System initialized in {}.\n");
    Settings::LoadSettings();
    NoiseGen::initSIMD();
    //    Log::info("{}, hardware_concurrency: {}x, {}-endian, cpu: {}", (Loader::sys_target()), std::thread::hardware_concurrency(), std::endian::native == std::endian::big ? "big" : "little", Loader::cpuid());

    for (const std::string& modpath : Settings::Mods) {
        ModLoader::loadMod(modpath);
    }
    //OpenVR::init();

    Ethertia::isRunning() = true;

    Window::Init(Settings::DisplayWidth, Settings::DisplayHeight, Ethertia::Version::name().c_str());
    RenderEngine::Init();
    // AudioEngine::init();
    NetworkSystem::init();


    // Materials & Items
    MaterialMeshes::Load();
    ItemTextures::Load();
//    Sounds::load();

    Recipes::init();  // after mtl-items register.

    g_Player = new EntityPlayer();  // before gui init. when gui init, needs get Player ptr. e.g. Inventory
    g_Player->position() = {0, 10, 0};
    g_Player->switchGamemode(Gamemode::CREATIVE);
    g_Player->setFlying(true);


    // Proc Threads
    ChunkMeshProc::initThread();
    ChunkGenProc::initThread();
    Ethertia::getAsyncScheduler().initThread();
    Ethertia::getScheduler().m_ThreadId = std::this_thread::get_id();
    Controls::initConsoleThread();




    Material::REGISTRY.dbgPrintEntries("Materials");
    Item::REGISTRY.dbgPrintEntries("Items");
    Biome::REGISTRY.dbgPrintEntries("Biomes");
//    Command::REGISTRY.dbgPrintEntries("Commands");
    Recipe::REGISTRY.dbgPrintEntries("Recipes");


#ifdef ET_DEBUG
    // 现在还没做高效能批量存储，现在的离散存储很慢 对磁盘也不太好 所以干脆禁用
    Log::info("Disabled Chunk Load/Save");
    Dbg::dbg_NoChunkLoad = true;
    Dbg::dbg_NoChunkSave = true;
#endif

}


// System Cleanup
static void Destroy()
{
    Settings::SaveSettings();

    if (Ethertia::getWorld()) {
        Ethertia::unloadWorld();
    }

    NetworkSystem::deinit();


    RenderEngine::Destroy();
//    AudioEngine::deinit();

    Window::Destroy();
}


// MainLoop.
// frequence=fps.
static void RunMainLoop()
{
    PROFILE("Frame");
    double time_framebegin = Ethertia::getPreciseTime();
    Ethertia::getTimer().update(time_framebegin);
    float dt = Ethertia::getDelta();
    World* world = Ethertia::getWorld();
    Window& window = Ethertia::getWindow();

    {
        PROFILE("SyncTask");
        Ethertia::getScheduler().processTasks(0.01);
    }

    {
        PROFILE("Tick");

        while (Ethertia::getTimer().polltick())
        {
            // runTick();
        }
        // these are Temporary. should be put in World::onTick().

        if (world)
        {
            PROFILE("Phys");
            g_Player->m_PrevVelocity = g_Player->m_Rigidbody->getLinearVelocity();
            world->m_DynamicsWorld->stepSimulation(dt);

            world->processEntityCollision();

            world->forLoadedChunks([](Chunk* chunk)
            {
                chunk->m_InhabitedTime += Ethertia::getDelta();
            });

            world->onTick(dt);
        }
    }

    {
        PROFILE("Input");
        {
            PROFILE("PollEvents");
            Window::PollEvents();
        }
        {
            PROFILE("HandleInputs");
            Controls::handleInput();
        }
    }
    {
        PROFILE("ProcGUI");

        {
            PROFILE("Imgui::NewFrame");
            Imgui::NewFrame();
        }


        PROFILE("Imgui::ShowWindows");
        Imw::ShowDockspaceAndMainMenubar();
        Imgui::ShowWindows();
    }

    {
        PROFILE("Render");
        RenderEngine::Render();
    }

    {
        PROFILE("FpsCap");
//        window.setVSync(Settings::s_Vsync);
//        window.swapBuffers();

        // Sync FPS.
//        if (Settings::s_FpsCap) {
//            const double till = time_framebegin + (1.0/Settings::s_FpsCap);
//            while (Ethertia::getPreciseTime() < till) {
//                Timer::sleep_for(1);
//            }
//        }

        Dbg::_fps_frame(Ethertia::getPreciseTime());

//        AudioEngine::checkAlError("Frame");
//        AudioEngine::setListenerPosition(Ethertia::getCamera().position);
//        AudioEngine::setListenerOrientation(Ethertia::getCamera().direction);
    }
}




//void Ethertia::runTick()
//{
//    if (m_World) {
//
//        m_World->tick();
//    }
//}

void Ethertia::loadWorld(const std::string& savedir, const WorldInfo* worldinfo)
{
    assert(getWorld() == nullptr);
    assert(Ethertia::getScheduler().numTasks() == 0);  // main-scheduler should be world-isolated. at least now.

    g_World = new World(savedir, worldinfo);
    World* world = g_World;

    world->addEntity(getPlayer());

    ChunkMeshProc::g_Running = 1;
    ChunkGenProc::g_Running = 1;

    Log::info("Loading world @\"{}\" *{}", world->m_ChunkLoader->m_ChunkDir, world->m_WorldInfo.Seed);


//    getScheduler()->addTask([](){
//        // not now. while handling GUI press/click. shouldn't remove gui.
//        Ethertia::getRootGUI()->removeAllGuis();
//        Ethertia::getRootGUI()->addGui(GuiIngame::Inst());
//    });

}

void Ethertia::unloadWorld()
{
    assert(getWorld());
    Log::info("Unloading World...");
    Ethertia::getHitCursor().reset();


    Log::info("Cleaning MeshGen");
    ChunkMeshProc::g_Running = -1;
    Timer::wait_for(&ChunkMeshProc::g_Running, 0);  // before delete chunks


    getWorld()->unloadAllChunks();

    Log::info("Cleaning ChunkGen");
    ChunkGenProc::g_Running = -1;
    Timer::wait_for(&ChunkGenProc::g_Running, 0);

    World* oldWorld = getWorld();
    g_World = nullptr;


    Log::info("Sync Tasks {}", getScheduler().numTasks());
    // make sure no Task remain. Task is world-isolated., Exec after other chunk-proc threads cuz they may addTask().
    getScheduler().processTasks(Mth::Inf);
    assert(getScheduler().numTasks() == 0);

    Log::info("Async Tasks {}", getAsyncScheduler().numTasks());
    getAsyncScheduler().processTasks(Mth::Inf);  // why? for what?
    assert(getAsyncScheduler().numTasks() == 0);


    delete oldWorld;


    Log::info("World unloaded.");

    // remove gui after GuiEventPolling
    // getScheduler()->addTask([](){
    //     Ethertia::getRootGUI()->removeAllGuis();
    //     Ethertia::getRootGUI()->addGui(GuiScreenMainMenu::Inst());
    // });
}




void Ethertia::dispatchCommand(const std::string& cmdline) {
    if (cmdline.empty()) return;
    Log::warn("Not supported yet. cmd {}", cmdline);

//    if (cmdline[0] != '/') {
//        if (!NetworkSystem::m_Connection) {
//            _SendMessage("Failed send chat, you haven't connect a server.");
//            return;
//        }
//        NetworkSystem::SendPacket(PacketChat{ cmdline });
//        return;
//    }
//
//    std::vector<std::string> args = Strings::splitSpaces(cmdline);
//
//    std::string cmd = args[0].substr(1);  // sub the leading '/'
//
//    Command* command = (Command*)Command::REGISTRY.get(cmd);
//    if (!command) {
//        _SendMessage("Unknown command: {} ({})", cmd, cmdline);
//        return;
//    }
//
//    // Execute command
//
//    try
//    {
//        command->onCommand(args);
//    }
//    catch (...)
//    {
//        std::exception_ptr p = std::current_exception();
//
//        _SendMessage("Error occurred when command execution.");
//    }

    //todo: sender? that only make sense on serverside.
}

void Ethertia::notifyMessage(const std::string& msg) {
    Log::info("[MSG/C] ", msg);
//    GuiMessageList::Inst()->addMessage(msg);
    Imw::Editor::ConsoleMessages.push_back(msg);
}

bool& Ethertia::isIngame() {
    static bool g_IsControllingGame = false;
    return g_IsControllingGame;
}
bool& Ethertia::isRunning() {
    static bool g_Running = false;
    return g_Running;
}

World* Ethertia::getWorld() { return g_World; }
EntityPlayer* Ethertia::getPlayer() { return g_Player; }

float Ethertia::getPreciseTime() { return (float)Window::PreciseTime(); }
float Ethertia::getDelta() { return getTimer().getDelta(); }
Camera& Ethertia::getCamera() { return g_Camera; }
HitCursor& Ethertia::getHitCursor() { return g_HitCursor; }
Profiler& Ethertia::getProfiler() { return g_Profiler; }
Scheduler& Ethertia::getAsyncScheduler() { return g_AsyncScheduler; }
Scheduler& Ethertia::getScheduler() { return g_Scheduler; }
Timer& Ethertia::getTimer() { return g_Timer; }
Window& Ethertia::getWindow() { return *g_Window; }


const Ethertia::Viewport& Ethertia::getViewport() {
    static Ethertia::Viewport g_Viewport;
    if (Imgui::wViewportXYWH.x != Mth::Inf) {
        glm::vec4 v = Imgui::wViewportXYWH;
        g_Viewport = { v.x, v.y, v.z, v.w };
    } else {
        g_Viewport = {0, 0, Window::Size().x, Window::Size().y};
    }
    return g_Viewport;
}










float Scheduler::_intl_program_time() {
    return Ethertia::getPreciseTime();
}




void Entity::onRender()
{
//    GeometryRenderer::render(
//            m_Model, position(), getRotation(), m_DiffuseMap);
}

void EntityMesh::onRender()
{
//    bool isFoliage = !m_FaceCulling;  if (isFoliage && Dbg::dbg_NoVegetable) return;
//
//    if (isFoliage)
//        glDisable(GL_CULL_FACE);
//
////    GeometryRenderer::render(
////            m_Model, position(), getRotation(), m_DiffuseMap, isFoliage ? 0.1 : 0.0);
//
//    if (isFoliage)
//        glEnable(GL_CULL_FACE);  // set back
}


// Mth.h
namespace glm
{
    std::ostream& operator<<(std::ostream& s, const glm::vec3& v) {
        s << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return s;
    }
}


void Camera::update(bool updateMatView)
{

    direction = Mth::eulerDirection(-eulerAngles.y, -eulerAngles.x);
    direction = glm::normalize(direction);

    actual_pos = position + -direction * len;

    // ViewMatrix
    if (updateMatView)
    matView = Mth::viewMatrix(actual_pos, eulerAngles);

    // ProjectionMatrix
    matProjection = glm::perspective(Mth::radians(fov), Ethertia::getViewport().getAspectRatio(), 0.01f, 1000.0f);

    // ViewFrustum
    m_Frustum.set(matProjection * matView);

}