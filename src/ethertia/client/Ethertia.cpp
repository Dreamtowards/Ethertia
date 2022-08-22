

#include "Ethertia.h"

#include <ethertia/client/render/RenderEngine.h>
#include <ethertia/client/Window.h>
#include <ethertia/util/Timer.h>
#include <ethertia/util/concurrent/Executor.h>
#include <ethertia/world/World.h>
#include <ethertia/entity/Entity.h>
#include <ethertia/client/gui/GuiRoot.h>

#include <ethertia/client/render/chunk/ChunkMeshGen.h>
#include <ethertia/client/gui/screen/GuiIngame.h>
#include <ethertia/init/Init.h>
#include <ethertia/client/gui/screen/GuiScreenMainMenu.h>

//#include <ethertia/lang/Elytra.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

int main()
{
    Ethertia::run();

//    et();

    return 0;
}


//#include <ethertia/util/Log.h>
//#include <ethertia/util/Colors.h>
//#include <ethertia/util/testing/BenchmarkTimer.h>
//#include <ethertia/util/OBJLoader.h>
//#include <ethertia/init/BlockTextures.h>
//#include <ethertia/init/Init.h>
//#include <ethertia/client/gui/GuiButton.h>
//#include <ethertia/client/gui/GuiPadding.h>
//#include <ethertia/client/gui/GuiAlign.h>
//#include <ethertia/client/gui/screen/GuiScreenMainMenu.h>
//#include <ethertia/client/gui/screen/GuiIngame.h>
//#include <ethertia/client/render/chunk/ChunkMeshGen.h>



bool Ethertia::running = false;

RenderEngine* Ethertia::renderEngine = nullptr;

Window Ethertia::window{};
Timer Ethertia::timer{};
Camera Ethertia::camera{};
Executor Ethertia::executor{std::this_thread::get_id()};

World* Ethertia::world = nullptr;
Entity* Ethertia::player = new Entity();
GuiRoot* Ethertia::rootGUI = new GuiRoot();


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
    renderEngine = new RenderEngine();

    Init::initialize();

    initThreadChunkLoad();

    GuiIngame::INST = new GuiIngame();
    GuiScreenMainMenu::INST = new GuiScreenMainMenu();

    rootGUI->addGui(GuiIngame::INST);
    rootGUI->addGui(GuiScreenMainMenu::INST);

    EventBus::EVENT_BUS.listen([&](KeyboardEvent* e) {
        if (e->isPressed()) {
            if (e->getKey() == GLFW_KEY_ESCAPE) {
                Gui* g = getRootGUI()->last();
                if (g != GuiIngame::INST) {
                    getRootGUI()->removeGui(g);
                } else {
                    getRootGUI()->addGui(GuiScreenMainMenu::INST);
                }
            }
        }
    });
    EventBus::EVENT_BUS.listen([=](MouseButtonEvent* e) {
        if (e->isPressed() && world) {
            u8 face;
            glm::vec3 pos;
            world->raycast(camera.position, camera.direction, pos, face);
            static u8 placingBlock = Blocks::STONE;

            int btn = e->getButton();
            if (btn == GLFW_MOUSE_BUTTON_1) {
                world->setBlock(pos, 0);
            } else if (btn == GLFW_MOUSE_BUTTON_2) {
                world->setBlock(pos + Mth::QFACES[face], placingBlock);
            } else if (btn == GLFW_MOUSE_BUTTON_3) {
                placingBlock = world->getBlock(pos);
            }
        }
    });



    {
//            std::string s = Loader::loadAssetsStr("entity/stall.obj");
//            std::stringstream ss(s);
//            VertexBuffer vbuf;
//            OBJLoader::loadOBJ(ss, &vbuf);
//
//            Model* mod = Loader::loadModel(&vbuf);
//
//            player->model = mod;
//            BitmapImage* img = Loader::loadPNG(Loader::loadAssets("entity/gravestone/diff.png"));
//            player->diffuseMap = Loader::loadTexture(img);
    }


}

static std::string chunkBuildStat = "Uninitialized";


void renderGUI()
{
    GuiRoot* rootGUI = Ethertia::getRootGUI();
    Window& window = *Ethertia::getWindow();
    Entity* player = Ethertia::getPlayer();
    Camera& camera = *Ethertia::getCamera();
    float dt = Ethertia::getDelta();

    rootGUI->onLayout();

    rootGUI->updateHovers(window.getMousePos());

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    rootGUI->onDraw();

    std::stringstream ss;
    Log::log(ss,
             "camp: {}, pvel: {}\n"
             "dt/ {}, {}fs\n"
             "ChunkStat: {}",
             glm::to_string(camera.position), glm::length(player->velocity),
             dt, 1.0/dt,
             chunkBuildStat);
    Gui::drawString(0, 32, ss.str(), Colors::WHITE, 16, 0, false);

//        Gui::drawRect(100, 100, 200, 100, Colors::WHITE, BlockTextures::ATLAS->atlasTexture, 20);
//        Gui::drawRect(100, 100, 10, 10, Colors::GREEN);

//        Gui::drawString(Gui::maxWidth()/2, 110, "Test yo wassaup9\nTest\nOf\nSomeTexts\nWill The Center Texting Works?Test yo wassaup9\nTest\nOf\nSomeTexts\nWill The Center Texting Works?Test yo wassaup9\nTest\nOf\nSomeTexts\nWill The Center Texting Works?Test yo wassaup9\nTest\nOf\nSomeTexts\nWill The Center Texting Works?Test yo wassaup9\nTest\nOf\nSomeTexts\nWill The Center Texting Works?",
//                        Colors::WHITE, 32, 1);

    Gui::drawWorldpoint(player->intpposition, [](glm::vec2 p) {

        Gui::drawRect(p.x, p.y, 4, 4, Colors::RED);
    });

    Gui::drawRect(Gui::maxWidth()/2, Gui::maxHeight()/2,
                  4, 4, Colors::WHITE);

    glEnable(GL_DEPTH_TEST);
}


void updateMovement() {
    Window& window = *Ethertia::getWindow();
    Camera& camera = *Ethertia::getCamera();
    Entity* player = Ethertia::getPlayer();

    static bool sprint = false;

    float speed = 0.8;
    if (window.isKeyDown(GLFW_KEY_LEFT_CONTROL)) sprint = true;
    if (sprint) speed = 1.8;
    float yaw = camera.eulerAngles.y;

    if (window.isKeyDown(GLFW_KEY_W)) player->velocity += Mth::angleh(yaw) * speed;
    if (window.isKeyDown(GLFW_KEY_S)) player->velocity += Mth::angleh(yaw + Mth::PI) * speed;
    if (window.isKeyDown(GLFW_KEY_A)) player->velocity += Mth::angleh(yaw + Mth::PI / 2) * speed;
    if (window.isKeyDown(GLFW_KEY_D)) player->velocity += Mth::angleh(yaw - Mth::PI / 2) * speed;

    if (window.isShiftKeyDown()) player->velocity.y -= speed;
    if (window.isKeyDown(GLFW_KEY_SPACE)) player->velocity.y += speed;

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
        camera.updateMovement(window, dt);
    }

    player->intpposition = Mth::lerp(Ethertia::getTimer()->getPartialTick(), player->prevposition, player->position);
    camera.compute(player->intpposition, renderEngine->viewMatrix);

    if (!window.isKeyDown(GLFW_KEY_P))
        renderEngine->updateViewFrustum();

    window.setMouseGrabbed(Ethertia::isIngame());
    window.setTitle(("desp. "+std::to_string(1.0/dt)).c_str());

    renderEngine->updateProjectionMatrix(Ethertia::getAspectRatio());
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
    player->position = glm::vec3(-10, 10, -10);
}

void Ethertia::unloadWorld() {


    delete world;
    Log::info("world: ", world);
    world = nullptr;
}



bool Ethertia::isIngame() { return getRootGUI()->last() == GuiIngame::INST; }

float Ethertia::getPreciseTime() { return (float)Window::getPreciseTime(); }
float Ethertia::getAspectRatio() {
    Window* w = getWindow(); if (w->getHeight() == 0) return 0;
    return w->getWidth() / w->getHeight();
}

float Ethertia::getDelta() { return timer.getDelta(); }






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
        // World::tmpDoRebuildModel(chunk, world);

        VertexBuffer* vbuf = ChunkMeshGen::genMesh(chunk, world);
        if (vbuf) {
            Ethertia::getExecutor()->exec([chunk, vbuf]() {
                delete chunk->model;
                chunk->model = Loader::loadModel(vbuf);
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
        world->unloadChunk(cp);
    }
}

void initThreadChunkLoad() {
    new std::thread([]() {
        while (Ethertia::isRunning())
        {
            World* world = Ethertia::getWorld();
            if (world) {
                chunkBuildStat = "ChunkLoad";
                updateViewDistance(world, Ethertia::getCamera()->position, (int)Ethertia::getRenderEngine()->viewDistance);

                chunkBuildStat = "ChunkModelUpdate";
                checkChunksModelUpdate(world);
                chunkBuildStat = "None";
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

// opt Sliders ViewDist,FOV,Colors..

// Framebuffer

