

#include <ethertia/Ethertia.h>

#include <ethertia/render/RenderEngine.h>
#include <ethertia/render/Window.h>
#include <ethertia/world/World.h>
#include <ethertia/util/Loader.h>
#include <ethertia/util/Assert.h>
#include <ethertia/util/Math.h>
#include <ethertia/util/BenchmarkTimer.h>
#include <ethertia/util/Timer.h>
#include <ethertia/init/Settings.h>
#include <ethertia/init/Controls.h>
#include <ethertia/init/DebugStat.h>
#include <ethertia/init/ItemTextures.h>
#include <ethertia/init/MaterialMeshes.h>
#include <ethertia/imgui/ImwInspector.h>  // tmp
//#include <ethertia/mod/ModLoader.h>
//#include <ethertia/network/client/ClientConnectionProc.h>
//#include <ethertia/item/recipe/Recipes.h>
//#include <ethertia/render/chunk/proc/ChunkMeshProc.h>
//#include <ethertia/render/chunk/proc/ChunkGenProc.h>
//#include <ethertia/network/client/NetworkSystem.h>
//#include <ethertia/command/Commands.h>
// #include <ethertia/audio/AudioEngine.h>
//#include <ethertia/vr/OpenVR.h>
//#include <ethertia/init/Sounds.h>
#include <ethertia/world/Physics.h>
#include <ethertia/imgui/Imgui.h>
#include <ethertia/imgui/Imw.h>

#include <thread>
#include <stdx/str.h>
#include <stdx/thread_pool.h>


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
static Timer        g_Timer;
static HitCursor    g_HitCursor;
static Profiler     g_Profiler;
static Camera       g_Camera;


static std::unique_ptr<stdx::thread_pool> g_ThreadPool;

static std::thread::id g_MainThreadId{};


// System Initialization.
static void Init()
{
    BENCHMARK_TIMER_MSG("System initialized in {}.\n");
    Settings::LoadSettings();
    //NoiseGen::InitSIMD();
    Log::info("{}, hardware_concurrency: {}x, {}-endian, cpu: {}", (Loader::os_arch()), std::thread::hardware_concurrency(), std::endian::native == std::endian::big ? "big" : "little", Loader::cpuid());

    g_ThreadPool = std::make_unique<stdx::thread_pool>(std::thread::hardware_concurrency());
    g_MainThreadId = std::this_thread::get_id();

    //for (const std::string& modpath : Settings::Mods) {
    //    ModLoader::LoadMod(modpath);
    //}
    //OpenVR::init();

    Ethertia::isRunning() = true;

    Window::Init(Settings::DisplayWidth, Settings::DisplayHeight, Ethertia::Version::name().c_str());
    RenderEngine::Init();
    // AudioEngine::init();
    // NetworkSystem::init();

    Physics::Init();

    ImwInspector::InitComponentInspectors();  // tmp

    // Materials & Items
    MaterialMeshes::Load();
    ItemTextures::Load();
//    Sounds::load();

    //Recipes::init();  // after mtl-items register.

    //g_Player = new EntityPlayer();  // before gui init. when gui init, needs get Player ptr. e.g. Inventory
    //g_Player->position() = {0, 10, 0};
    //g_Player->switchGamemode(Gamemode::CREATIVE);
    //g_Player->setFlying(true);



    // Proc Threads
    //ChunkMeshProc::initThread();
    //ChunkGenProc::initThread();
    Controls::initConsoleThread();


//    Material::REGISTRY.dbgPrintEntries("Materials");
//    Item::REGISTRY.dbgPrintEntries("Items");
//    Biome::REGISTRY.dbgPrintEntries("Biomes");
//    Command::REGISTRY.dbgPrintEntries("Commands");
//    Recipe::REGISTRY.dbgPrintEntries("Recipes");


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

    if (Ethertia::GetWorld()) {
        Ethertia::UnloadWorld();
    }

    // NetworkSystem::deinit();

    Physics::Release();


    RenderEngine::Destroy();
//    AudioEngine::deinit();

    Window::Destroy();
}


// MainLoop. called every display frame.
static void RunMainLoop()
{
    PROFILE("Frame");
    double time_framebegin = Ethertia::getPreciseTime();
    Ethertia::getTimer().update(time_framebegin);
    float dt = Ethertia::getDelta();
    World* world = Ethertia::GetWorld();

    {
        PROFILE("Tick");

        while (Ethertia::getTimer().polltick())
        {
            PROFILE("WorldTick");

            if (world)
            {
                world->OnTick(dt);
            }
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








#pragma region LoadWorld, UnloadWorld

void Ethertia::LoadWorld(const std::string& savedir, const WorldInfo* worldinfo)
{
    ET_ASSERT(Ethertia::GetWorld() == nullptr);

    g_World = new World();// savedir, worldinfo);
    World* world = g_World;

    //world->addEntity(getPlayer());

    //ChunkMeshProc::g_Running = 1;
    //ChunkGenProc::g_Running = 1;

    Log::info("Loading world '{}'", savedir);


//    getScheduler()->addTask([](){
//        // not now. while handling GUI press/click. shouldn't remove gui.
//        Ethertia::getRootGUI()->removeAllGuis();
//        Ethertia::getRootGUI()->addGui(GuiIngame::Inst());
//    });

}

void Ethertia::UnloadWorld()
{
    ET_ASSERT(Ethertia::GetWorld() != nullptr);

    Log::info("Unloading World...");

    Ethertia::getHitCursor().reset();


    //Log::info("Cleaning MeshGen");
    //ChunkMeshProc::g_Running = -1;
    //Timer::wait_for(&ChunkMeshProc::g_Running, 0);  // before delete chunks
    //
    //
    ////Ethertia::GetWorld()->unloadAllChunks();
    //
    //Log::info("Cleaning ChunkGen");
    //ChunkGenProc::g_Running = -1;
    //Timer::wait_for(&ChunkGenProc::g_Running, 0);

    World* oldWorld = Ethertia::GetWorld();
    g_World = nullptr;


    delete oldWorld;


    Log::info("World unloaded.");
}


#pragma endregion




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



void Ethertia::PrintMessage(const std::string& msg) {
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

World* Ethertia::GetWorld() { return g_World; }

stdx::thread_pool& Ethertia::GetThreadPool() { return *g_ThreadPool; }

bool Ethertia::InMainThread() { return std::this_thread::get_id() == g_MainThreadId; }


EntityPlayer* Ethertia::getPlayer() { return g_Player; }

float Ethertia::getPreciseTime() { return (float)Window::PreciseTime(); }
float Ethertia::getDelta() { return getTimer().getDelta(); }
Camera& Ethertia::getCamera() { return g_Camera; }
HitCursor& Ethertia::getHitCursor() { return g_HitCursor; }
Profiler& Ethertia::getProfiler() { return g_Profiler; }
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












//void Entity::onRender()
//{
//    GeometryRenderer::render(
//            m_Model, position(), getRotation(), m_DiffuseMap);
//}

//void EntityMesh::onRender()
//{
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
//}



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