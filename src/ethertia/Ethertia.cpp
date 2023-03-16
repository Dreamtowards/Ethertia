

#include <ethertia/Ethertia.h>

#include <ethertia/render/RenderEngine.h>
#include <ethertia/render/Window.h>
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
#include <ethertia/vr/OpenVR.h>
#include <ethertia/init/Sounds.h>

#include <ethertia/imgui/ImGuis.h>
#include <ethertia/world/Biome.h>
#include <ethertia/render/deferred/GeometryRenderer.h>


static void Init();
static void Destroy();
static void RunMainLoop();


// Entry Point & Entire Lifetime.

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
    Settings::loadSettings();
    NoiseGen::initSIMD();
    Loader::checkWorkingDirectory();

    for (const std::string& modpath : Settings::MODS) {
        ModLoader::loadMod(modpath);
    }
    //OpenVR::init();

    Ethertia::isRunning() = true;
    g_Window = new Window(Settings::displayWidth, Settings::displayHeight, Ethertia::Version::name().c_str());
    ShaderProgram::loadAll();
    RenderEngine::init();
    AudioEngine::init();
    Log::info("Core {}, {}, endian {}", std::thread::hardware_concurrency(), Loader::sys_target_name(), std::endian::native == std::endian::big ? "big" : "little");

    // Materials & Items
    Materials::registerMaterialItems();  // before items tex load.
    MaterialTextures::load();
    MaterialMeshes::load();
    ItemTextures::load();

    Sounds::load();


    // Network
    ClientConnectionProc::initPackets();

    // Client Controls.
    Controls::initControls();

    g_Player = new EntityPlayer();  // before gui init. when gui init, needs get Player ptr. e.g. Inventory
    g_Player->position() = {10, 10, 10};
    g_Player->switchGamemode(Gamemode::SPECTATOR);
    g_Player->setFlying(true);

//    GuiIngame::initGUIs();

    // Proc Threads
    ChunkMeshProc::initThread();
    ChunkGenProc::initThread();
    Ethertia::getAsyncScheduler().initThread();
    Ethertia::getScheduler().m_ThreadId = std::this_thread::get_id();



    ImGuis::Init();




    Material::REGISTRY.dbgPrintEntries("Materials");
    Item::REGISTRY.dbgPrintEntries("Items");
    Biome::REGISTRY.dbgPrintEntries("Biomes");
    Command::REGISTRY.dbgPrintEntries("Commands");

}

// System Cleanup
static void Destroy()
{
    Settings::saveSettings();

    if (Ethertia::getWorld()) {
        Ethertia::unloadWorld();
    }

    NetworkSystem::deinit();

    RenderEngine::deinit();
    AudioEngine::deinit();

    ImGuis::Destroy();

    glfwTerminate();
}


// MainLoop.
// frequence=fps.
static void RunMainLoop()
{
    PROFILE("Frame");
    Ethertia::getTimer().update(Ethertia::getPreciseTime());
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

        window.resetDeltas();
        glfwPollEvents();
        Controls::handleContinuousInput();
    }

    {
        PROFILE("Render");
        RenderCommand::Clear();

        if (world)
        {
            PROFILE("World");
            RenderEngine::RenderWorld();
        }
        {
            PROFILE("GUI");
            //renderGUI();
            ImGuis::RenderGUI();
        }
    }

    {
        PROFILE("SwapBuffer");
        window.swapBuffers();

        AudioEngine::checkAlError("Frame");
        AudioEngine::setListenerPosition(Ethertia::getCamera().position);
        AudioEngine::setListenerOrientation(Ethertia::getCamera().direction);
    }
}

//void Ethertia::renderGUI()
//{
//    GuiRoot* rootGUI = Ethertia::getRootGUI();
//
//    rootGUI->onLayout();
//
//    rootGUI->updateHovers(Ethertia::getWindow()->getMousePos());
//
//    glDisable(GL_DEPTH_TEST);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glDisable(GL_CULL_FACE);
//
//    GuiInventory::HOVER_ITEM = nullptr;
//
//    rootGUI->onDraw();
//
//    GuiInventory::doDrawHoveredItem();
//    GuiInventory::doDrawPickingItem();
//
////    Gui::drawRect(0, Gui::maxHeight()-300, 300, 300, {
////        .tex = ShadowRenderer::fboDepthMap->texDepth,
////        .channel_mode = Gui::DrawRectArgs::C_RGB
////    });
//
//    HitCursor& cur = Ethertia::getHitCursor();
//    if (cur.cell_breaking_time)
//    {
//        float fullBreakingTime = cur.cell->mtl->m_Hardness;
//        float t = cur.cell_breaking_time / fullBreakingTime;
//        Gui::drawString(64, 256, Strings::fmt("{}/{}s", Mth::floor_dn(cur.cell_breaking_time, 1), fullBreakingTime));
//        Gui::drawRect(64, 256+16, 120,   4, Colors::BLACK80);
//        Gui::drawRect(64, 256+16, 120*t, 4, Colors::GREEN_DARK);
//    }
//
//    {
////        float x = Gui::maxWidth() - 64;
////        float y = Gui::maxHeight() - 64 - 40;
////        if (World::dbg_ChunkProvideState) {
////            Gui::drawString(x, y, World::dbg_ChunkProvideState == 1 ? "Generating Chunk.." : "Loading Chunk..",
////                            Colors::GRAY, 16, {-1, -1});
////        }
////        if (World::dbg_SavingChunks) {
////            Gui::drawString(x, y+16, Strings::fmt("Saving chunks... ({})", World::dbg_SavingChunks),
////                            Colors::GREEN_DARK, 16, {-1, -1});
////        }
////        if (ChunkMeshProc::dbg_NumChunksMeshInvalid > 0) {
////            Gui::drawString(x, y+32, Strings::fmt("Meshing chunks... ({})", ChunkMeshProc::dbg_NumChunksMeshInvalid),
////                            Colors::GRAY_DARK, 16, {-1, -1});
////        }
//    }
//
//    if (!WorldEdit::selection.empty()) {
//        AABB& s = WorldEdit::selection;
//        RenderEngine::drawLineBox(s.min, s.size(), Colors::GREEN);
//    }
//
//    glEnable(GL_DEPTH_TEST);
//
//}


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
//    GuiMessageList::Inst()->addMessage(msg);
    ImGuis::g_MessageBox.push_back(msg);
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

float Ethertia::getPreciseTime() { return (float)Window::getPreciseTime(); }
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
    if (ImGuis::wViewportXYWH.x != Mth::Inf) {
        glm::vec4 v = ImGuis::wViewportXYWH;
        g_Viewport = { v.x, v.y, v.z, v.w };
    } else {
        g_Viewport = {0, 0, (float)getWindow().getWidth(), (float)getWindow().getHeight()};
    }
    return g_Viewport;
}








void ShaderProgram::setViewProjection(bool view)
{
    setMatrix4f("matProjection", Ethertia::getCamera().matProjection);

    setMatrix4f("matView", view ? Ethertia::getCamera().matView : glm::mat4(1.0));
}

void ShaderProgram::reloadSources_() {
    Loader::loadShaderProgram(this, m_SourceLocation);
}


float Scheduler::_intl_program_time() {
    return Ethertia::getPreciseTime();
}




void Entity::onRender()
{
    GeometryRenderer::render(
            m_Model, position(), getRotation(), m_DiffuseMap);
}

void EntityMesh::onRender()
{
    bool isFoliage = !m_FaceCulling;  if (isFoliage && Dbg::dbg_NoVegetable) return;

    if (isFoliage)
        glDisable(GL_CULL_FACE);

    GeometryRenderer::render(
            m_Model, position(), getRotation(), m_DiffuseMap, isFoliage ? 0.1 : 0.0);

    if (isFoliage)
        glEnable(GL_CULL_FACE);  // set back
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