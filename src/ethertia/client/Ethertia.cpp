

#include <ethertia/client/Ethertia.h>

#include <ethertia/client/render/RenderEngine.h>
#include <ethertia/client/Window.h>
#include <ethertia/client/gui/GuiRoot.h>
#include <ethertia/client/gui/screen/GuiIngame.h>
#include <ethertia/client/gui/screen/GuiScreenMainMenu.h>
#include <ethertia/client/gui/screen/GuiScreenChat.h>
#include <ethertia/client/render/chunk/BlockyMeshGen.h>
#include <ethertia/client/render/chunk/MarchingCubesMeshGen.h>
#include <ethertia/client/render/chunk/SurfaceNetsMeshGen.h>
#include <ethertia/world/World.h>
#include <ethertia/init/Init.h>
#include <ethertia/util/Timer.h>
#include <ethertia/util/concurrent/Executor.h>
#include <ethertia/util/Strings.h>
#include <ethertia/entity/Entity.h>
#include <ethertia/entity/EntityCar.h>
#include <ethertia/entity/EntityRaycastCar.h>
#include <ethertia/entity/player/EntityPlayer.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

//#include <ethertia/lang/Elytra.h>  et();

//static bool running = false;
//
//void init();
//void runMainLoop();
//void destroy();

int main()
{
    Ethertia::run();

    return 0;
}

//void init() {
//
//    running = true;
//
//}

bool Ethertia::running = false;

RenderEngine* Ethertia::renderEngine = nullptr;

Window Ethertia::window{};
Timer Ethertia::timer{};
Executor Ethertia::executor{std::this_thread::get_id()};

World* Ethertia::world = nullptr;
EntityPlayer* Ethertia::player = nullptr;
GuiRoot* Ethertia::rootGUI = nullptr;

static BrushCursor brushCursor;


void handleInput();
void renderGUI();
void initThreadChunkLoad();


void Ethertia::runMainLoop()
{
    timer.update(getPreciseTime());

    executor.processTasks();

    while (timer.polltick())
    {
        runTick();
    }
    if (world) {
//        player->rigidbody->setGravity(btVector3(0,0,0));
//        player->rigidbody->setDamping(0.98f, 0.18f);
        world->dynamicsWorld->stepSimulation(getDelta());
    }

    handleInput();

    if (world)
    {
        renderEngine->renderWorld(world);
    }

    renderGUI();

    window.updateWindow();
}

void Ethertia::start() {

    running = true;
    window.initWindow();

    Init::initialize();

    renderEngine = new RenderEngine();
    rootGUI = new GuiRoot();
    window.fireWindowSizeEvent();  // effect to the GuiRoot now.

    initThreadChunkLoad();


    player = new EntityPlayer();
    player->setPosition({10, 10, 10});

    Ethertia::loadWorld();

    player->setFlying(true);
    player->switchGamemode(Gamemode::SPECTATOR);

    GuiIngame::INST = new GuiIngame();
    GuiScreenMainMenu::INST = new GuiScreenMainMenu();
    GuiScreenChat::INST = new GuiScreenChat();

    rootGUI->addGui(GuiIngame::INST);
//    rootGUI->addGui(GuiScreenMainMenu::INST);

//    Texture* tex = Loader::loadTexture(Loader::loadPNG(Loader::loadAssets("blocks/stone_.png")));
//    Chunk::tex = tex;

//    EntityCar* car = new EntityCar();
//    world->addEntity(car);
//    car->setPosition({10, 10, -10});


    EntityRaycastCar* raycastCar = new EntityRaycastCar();
//    raycastCar->setPosition({0, 5, -10});
//    world->addEntity(raycastCar);




    brushCursor.size = 2.0;

    EventBus::EVENT_BUS.listen([&, raycastCar](KeyboardEvent* e) {
        if (e->isPressed()) {
            int key = e->getKey();
            if (key == GLFW_KEY_ESCAPE) {
                Gui* g = getRootGUI()->last();
                if (g != GuiIngame::INST) {
                    getRootGUI()->removeGui(g);
                } else {
                    getRootGUI()->addGui(GuiScreenMainMenu::INST);
                }
            } else if (isIngame()) {
                if (key == GLFW_KEY_SLASH) {
                    getRootGUI()->addGui(GuiScreenChat::INST);
                } else if (key == GLFW_KEY_G) {
                    raycastCar->m_vehicle->applyEngineForce(100, 2);
                    raycastCar->m_vehicle->applyEngineForce(100, 3);

                    raycastCar->m_vehicle->setBrake(100, 2);
                    raycastCar->m_vehicle->setBrake(100, 3);


                    raycastCar->m_vehicle->setSteeringValue(0, 0);
                    raycastCar->m_vehicle->setSteeringValue(0, 1);

                    Log::info("Force");
                }
            }
        }
    });
    EventBus::EVENT_BUS.listen([=](MouseButtonEvent* e) {
        if (e->isPressed() && world && isIngame()) {
            static u8 placingBlock = Materials::GRASS;

            int btn = e->getButton();

            BrushCursor& cur = Ethertia::getBrushCursor();
            if (cur.hit) {
                glm::vec3 p = cur.position;
                float n = cur.size;

//                glm::vec3 v = (p - camera.position) * 0.8f;
//                v.y += glm::length(v) * 0.4f;
//                player->applyLinearVelocity(v);

                if (btn == GLFW_MOUSE_BUTTON_1) {
                    for (int dx = floor(-n); dx <= ceil(n); ++dx) {
                        for (int dz = floor(-n); dz <= ceil(n); ++dz) {
                            for (int dy = floor(-n); dy <= ceil(n); ++dy) {
                                glm::vec3 d(dx, dy, dz);

                                MaterialStat& b = world->getBlock(p + d);
                                float f = n - glm::length(d);

                                // b.id = placingBlock;
                                b.density = b.density - Mth::max(0.0f, n - glm::length(d));
                                world->requestRemodel(p+d);
                            }
                        }
                    }
                } else if (btn == GLFW_MOUSE_BUTTON_2) {
                    for (int dx = -n; dx <= n; ++dx) {
                        for (int dz = -n; dz <= n; ++dz) {
                            for (int dy = -n; dy <= n; ++dy) {
                                glm::vec3 d(dx, dy, dz);

                                MaterialStat& b = world->getBlock(p + d);
                                float f = n - glm::length(d);

                                if (!window.isAltKeyDown())  {
                                    b.density = Mth::max(b.density, f);
                                }
                                if (b.density >= 0.0f)
                                    b.id = placingBlock;

                                world->requestRemodel(p+d);
                            }
                        }
                    }
                    world->requestRemodel(p);
                } else if (btn == GLFW_MOUSE_BUTTON_3) {
                    placingBlock = world->getBlock(p).id;
                    Log::info("Picking: {}", (int)placingBlock);
                }
            }

        }
    });








}

void renderGUI()
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


void updateMovement() {
    Window& window = *Ethertia::getWindow();

    static bool sprint = false;

    float speed = 0.5;
    if (window.isKeyDown(GLFW_KEY_LEFT_CONTROL)) sprint = true;
    if (sprint) speed = 3.8;
    float yaw = Ethertia::getCamera()->eulerAngles.y;

    glm::vec3 vel(0);
    if (window.isKeyDown(GLFW_KEY_W)) vel += Mth::angleh(yaw) * speed;
    if (window.isKeyDown(GLFW_KEY_S)) vel += Mth::angleh(yaw + Mth::PI) * speed;
    if (window.isKeyDown(GLFW_KEY_A)) vel += Mth::angleh(yaw + Mth::PI / 2) * speed;
    if (window.isKeyDown(GLFW_KEY_D)) vel += Mth::angleh(yaw - Mth::PI / 2) * speed;

    if (window.isShiftKeyDown())          vel.y -= speed;
    if (window.isKeyDown(GLFW_KEY_SPACE)) vel.y += speed;

    Ethertia::getPlayer()->applyLinearVelocity(vel);

    if (!window.isKeyDown(GLFW_KEY_W)) {
        sprint = false;
    }
}
// not accurate name.
void handleInput()
{
    Camera& camera = *Ethertia::getCamera();
    Window& window = *Ethertia::getWindow();
    Entity* player = Ethertia::getPlayer();
    RenderEngine* renderEngine = Ethertia::getRenderEngine();

    float dt = Ethertia::getDelta();

    if (Ethertia::isIngame()) {
        updateMovement();
        camera.updateMovement(dt, window.getMouseDX(), window.getMouseDY(), window.isKeyDown(GLFW_KEY_Z), window.getDScroll());
    }
    window.setMouseGrabbed(Ethertia::isIngame());
    window.setStickyKeys(!Ethertia::isIngame());
    window.setTitle(("desp. "+std::to_string(1.0/dt)).c_str());

//    player->intpposition = /*Mth::lerp(Ethertia::getTimer()->getPartialTick(), player->prevposition, */player->getPosition();//);
    camera.compute(player->getPosition(), renderEngine->viewMatrix);

    renderEngine->updateViewFrustum();
    renderEngine->updateProjectionMatrix(Ethertia::getAspectRatio());

    if (brushCursor.keepTracking) {
        glm::vec3 p, n;
        if (Ethertia::getWorld()->raycast(camera.position, camera.position + camera.direction * 100.0f, p, n)) {
            brushCursor.hit = true;
            brushCursor.position = p;
        } else {
            brushCursor.hit = false;
            brushCursor.position = glm::vec3(0.0f);
        }
    }

}

void Ethertia::runTick()
{

    if (world) {

        world->tick();
    }
}

void Ethertia::destroy()
{
    delete rootGUI;
    delete world;
    delete renderEngine;

    glfwTerminate();
}

void Ethertia::loadWorld() {
    assert(world == nullptr);

    world = new World();
    world->addEntity(player);
}

void Ethertia::unloadWorld() {


    delete world;
    Log::info("world: ", world);
    world = nullptr;
}

void Ethertia::dispatchCommand(const std::string& cmdline) {
    std::vector<std::string> args = Strings::splitSpaces(cmdline);
    std::string& cmd = args[0];
    EntityPlayer* player = Ethertia::player;

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



bool Ethertia::isIngame() { return getRootGUI()->last() == GuiIngame::INST && !window.isKeyDown(GLFW_KEY_GRAVE_ACCENT); }

float Ethertia::getPreciseTime() { return (float)Window::getPreciseTime(); }
float Ethertia::getAspectRatio() {
    Window* w = getWindow(); if (w->getHeight() == 0) return 0;
    return w->getWidth() / w->getHeight();
}

float Ethertia::getDelta() { return timer.getDelta(); }

BrushCursor& Ethertia::getBrushCursor() { return brushCursor; }

Camera* Ethertia::getCamera() {
    return &renderEngine->mCamera;
}


static void checkChunksModelUpdate(World* world) {
    glm::vec3 vpos = Ethertia::getCamera()->position;

    int i = 0;
    while (++i <= 4) {
        Chunk* chunk = nullptr;
        float minLen = Mth::Inf;
        for (auto& it : world->getLoadedChunks()) {
            Chunk* c = it.second;
            if (c && c->needUpdateModel) {
                float len = glm::length(c->position - vpos);
                if (len < minLen) {
                    minLen = len;
                    chunk = c;
                }
            }
        }
        if (chunk == nullptr)  // nothing to update
            break;
        chunk->needUpdateModel = false;


        VertexBuffer* vbuf = nullptr;

//        vbuf = BlockyChunkMeshGen::genMesh(chunk, world);
//        vbuf->initnorm();

//        vbuf = SurfaceNetsMeshGen::contouring(chunk, world);
        vbuf = MarchingCubesMeshGen::genMesh(chunk);
//        vbuf = BlockyMeshGen::gen(chunk, new VertexBuffer);
        vbuf->initnorm(false);


        if (vbuf) {
            Ethertia::getExecutor()->exec([chunk, vbuf]() {
                delete chunk->proxy->model;

                chunk->proxy->model = Loader::loadModel(vbuf, true);
                chunk->proxy->setMesh(chunk->proxy->model, vbuf->positions.data());

                delete vbuf;
            });
        }
    }
}

static void updateViewDistance(World* world, glm::vec3 p, int n)
{
    glm::vec3 cpos = Chunk::chunkpos(p);

    int num = 0;
    // load chunks
    for (int dx = -n;dx <= n;dx++) {
        for (int dz = -n;dz <= n;dz++) {
            for (int dy = -n;dy <= n;dy++) {
                glm::vec3 p = cpos + glm::vec3(dx, dy, dz) * 16.0f;
                Chunk* c = world->getLoadedChunk(p);
                if (!c) {
                    world->provideChunk(p);
                    for (int i = 0; i < 6; ++i) {
                        world->requestRemodel(p + Mth::QFACES[i] * 16.0f);
                    }
                    if (++num > 10)
                        goto skip;
                }
            }
        }
    }
    skip:

    // unload chunks
    int lim = n*Chunk::SIZE;
    std::vector<glm::vec3> unloads;  // separate iterate / remove
    for (auto itr : world->getLoadedChunks()) {
        glm::vec3 cp = itr.first;
        if (abs(cp.x-cpos.x) > lim || abs(cp.y-cpos.y) > lim || abs(cp.z-cpos.z) > lim) {
            unloads.push_back(cp);
        }
    }
    for (glm::vec3 cp : unloads) {
//        world->unloadChunk(cp);
    }
}

void initThreadChunkLoad() {
    new std::thread([]() {
        while (Ethertia::isRunning())
        {
            World* world = Ethertia::getWorld();
            if (world) {
                {
                    std::lock_guard<std::mutex> guard(world->chunklock);
//                    chunkBuildStat = "ChunkLoad";
//                    updateViewDistance(world, glm::vec3(0), 0);
                    updateViewDistance(world, Ethertia::getCamera()->position, (int)Ethertia::getRenderEngine()->viewDistance);

//                    chunkBuildStat = "ChunkModelUpdate";
                    checkChunksModelUpdate(world);
//                    chunkBuildStat = "None";
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds (10));
        }
    });
}



// todo ls:

// multi layer cube cloud
// cube ambient occlusion
// multi block types.
// block density? for isosurface and even SVO?

// Sky gradient.

// Self-spread chunkload order

// Framebuffer

