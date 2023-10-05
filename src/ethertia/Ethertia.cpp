

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
    uint64_t s;
    Loader::ram(&s,&s,&s,&s); return 1;
    Init();

    while (Ethertia::IsRunning())
    {
        RunMainLoop();
    }

    Destroy();

    return 0;
}

static World*       g_World     = nullptr;
//static EntityPlayer*g_Player    = nullptr;
static Profiler     g_Profiler;
static Camera       g_Camera;

static std::unique_ptr<stdx::thread_pool> g_ThreadPool;
static std::thread::id g_MainThreadId{};

static void _InitConsoleThread();


// System Initialization.
static void Init()
{
    BENCHMARK_TIMER_MSG("System initialized in {}.\n");
    Settings::LoadSettings();
    //NoiseGen::InitSIMD();
    Log::info("{}, hardware_concurrency: {}x, {}, {}-endian", Loader::os_arch(), std::thread::hardware_concurrency(), Loader::cpuid(), std::endian::native == std::endian::big ? "B" : "L");

    g_ThreadPool = std::make_unique<stdx::thread_pool>(std::thread::hardware_concurrency());
    g_MainThreadId = std::this_thread::get_id();

    //for (const std::string& modpath : Settings::Mods) {
    //    ModLoader::LoadMod(modpath);
    //}
    //OpenVR::init();

    Ethertia::IsRunning() = true;

    Window::Init(Settings::DisplayWidth, Settings::DisplayHeight, Ethertia::GetVersion(true).c_str());
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
    _InitConsoleThread();


//    Material::REGISTRY.dbgPrintEntries("Materials");
//    Item::REGISTRY.dbgPrintEntries("Items");
//    Biome::REGISTRY.dbgPrintEntries("Biomes");
//    Command::REGISTRY.dbgPrintEntries("Commands");
//    Recipe::REGISTRY.dbgPrintEntries("Recipes");



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
    ET_PROFILE_("Frame");
    double time_framebegin = Ethertia::GetPreciseTime();
    Ethertia::GetTimer().update(time_framebegin);
    float dt = Ethertia::GetDelta();
    World* world = Ethertia::GetWorld();

    {
        ET_PROFILE_("Tick");

        while (Ethertia::GetTimer().polltick())
        {
            ET_PROFILE_("WorldTick");

            if (world)
            {
                world->OnTick(dt);
            }
        }
    }

    {
        ET_PROFILE_("Input");
        {
            ET_PROFILE_("PollEvents");
            Window::PollEvents();
        }
        {
            ET_PROFILE_("HandleInputs");
            Controls::handleInput();
        }
    }
    {
        ET_PROFILE_("ProcGUI");

        {
            ET_PROFILE_("Imgui::NewFrame");
            Imgui::NewFrame();
        }


        ET_PROFILE_("Imgui::ShowWindows");
        Imw::ShowDockspaceAndMainMenubar();
        Imgui::ShowWindows();
    }

    {
        ET_PROFILE_("Render");
        RenderEngine::Render();
    }

    {
        ET_PROFILE_("FpsCap");
//        window.setVSync(Settings::s_Vsync);
//        window.swapBuffers();

        // Sync FPS.
//        if (Settings::s_FpsCap) {
//            const double till = time_framebegin + (1.0/Settings::s_FpsCap);
//            while (Ethertia::getPreciseTime() < till) {
//                Timer::sleep_for(1);
//            }
//        }

        Dbg::_fps_frame(Ethertia::GetPreciseTime());

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
    ET_ASSERT(Ethertia::GetWorld());

    Log::info("Unloading World...");

    Ethertia::GetHitCursor().reset();

    ImwInspector::SelectedEntity = {};

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


#pragma region DispatchCommand, PrintMessage, _InitConsoleThread

void Ethertia::DispatchCommand(const std::string& cmdline) {
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


static void _InitConsoleThread()
{
    new std::thread([]()
    {
        Log::info("Console thread is ready");

        while (Ethertia::IsRunning())
        {
            std::string line;
            std::getline(std::cin, line);

            Ethertia::DispatchCommand(line);
        }
    });
}

#pragma endregion


#pragma region Getters

bool& Ethertia::IsRunning() {
    static bool _IsRunning = false;
    return _IsRunning;
}
bool& Ethertia::isIngame() {
    static bool _IsControllingGame = false;
    return _IsControllingGame;
}

Timer& Ethertia::GetTimer() {
    static Timer _Timer;
    return _Timer;
}
HitCursor& Ethertia::GetHitCursor() {
    static HitCursor _HitResult;
    return _HitResult;
}
Camera& Ethertia::GetCamera() { return g_Camera; }
Profiler& Ethertia::GetProfiler() { return g_Profiler; }

World* Ethertia::GetWorld() { return g_World; }
stdx::thread_pool& Ethertia::GetThreadPool() { return *g_ThreadPool; }
bool Ethertia::InMainThread() { return std::this_thread::get_id() == g_MainThreadId; }

float Ethertia::GetPreciseTime() { return (float)Window::PreciseTime(); }
float Ethertia::GetDelta() { return GetTimer().getDelta(); }

Ethertia::Viewport Ethertia::GetViewport() {
    if (ImwGame::Viewport.x == Mth::Inf) {  // World Viewport Window Closed
        glm::vec2 s = Window::Size();
        return {0,0,0,0}; // { 0, 0, s.x, s.y };
    } else {
        glm::vec4 v = ImwGame::Viewport;
        return { v.x, v.y, v.z, v.w };
    }
}

const std::string Ethertia::GetVersion(bool fullname) 
{
    static std::string _VerName = ET_VERSION_SNAPSHOT ? 
        std::format("{} *{}.{}.{}", ET_VERSION_SNAPSHOT, ET_VERSION_MAJOR, ET_VERSION_MINOR, ET_VERSION_PATCH) :
        std::format("{}.{}.{}", ET_VERSION_MAJOR, ET_VERSION_MINOR, ET_VERSION_PATCH);

    if (fullname) 
    {
        static std::string _FullVerName = "Ethertia Alpha " + _VerName;
        return _FullVerName;
    }
    return _VerName;
}

#pragma endregion








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
    matProjection = glm::perspective(Mth::radians(fov), Ethertia::GetViewport().AspectRatio(), 0.01f, 1000.0f);

    // ViewFrustum
    m_Frustum.set(matProjection * matView);

}


