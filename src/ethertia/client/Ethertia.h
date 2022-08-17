//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_ETHERTIA_H
#define ETHERTIA_ETHERTIA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ethertia/client/render/RenderEngine.h>
#include <ethertia/client/render/Camera.h>
#include <ethertia/client/Window.h>
//#include <ethertia/client/audio/AudioEngine.h>
#include <ethertia/client/gui/GuiRoot.h>
#include <ethertia/util/Timer.h>
#include <ethertia/util/concurrent/Executor.h>
#include <ethertia/util/Log.h>
#include <ethertia/util/Colors.h>
#include <ethertia/init/BlockTextures.h>
#include <ethertia/init/Init.h>
#include <ethertia/world/World.h>
#include <ethertia/entity/Entity.h>
#include <ethertia/client/gui/GuiButton.h>
#include <ethertia/client/gui/GuiPadding.h>
#include <ethertia/client/gui/GuiAlign.h>
#include <ethertia/client/gui/screen/GuiScreenMainMenu.h>
#include <ethertia/client/gui/screen/GuiIngame.h>
#include <ethertia/client/render/chunk/ChunkMeshGen.h>
#include <ethertia/util/testing/BenchmarkTimer.h>


class Ethertia
{
    inline static Ethertia* INST;

    bool running = false;

    RenderEngine* renderEngine = nullptr;
    // AudioEngine audioEngine;
    Window window{};
    Timer timer{};
    Camera camera{};
    Executor executor{std::this_thread::get_id()};

    World* world = nullptr;
    Entity* player = new Entity();
    GuiRoot* rootGUI = new GuiRoot();

public:

    void run()
    {
        start();

        while (running)
        {
            runMainLoop();
        }

        destroy();
    }

    void start()
    {
        INST = this;
        running = true;
        window.initWindow();
        renderEngine = new RenderEngine();
        world = new World();
        world->addEntity(player);
        player->position = glm::vec3(-10, 10, -10);

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
            if (e->isPressed()) {
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
    }

    void runMainLoop()
    {
        timer.update(getPreciseTime());

        executor.processTasks();

        while (timer.polltick())
        {
            runTick();
        }

        clientUpdate();

        {
            renderEngine->renderWorld(world);

            renderGUI();
        }

        window.updateWindow();
    }

    inline static std::string chunkBuildStat = "Uninitialized";

    void renderGUI()
    {
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
                      timer.getDelta(), 1.0/timer.getDelta(),
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

    void clientUpdate()
    {
        if (isIngame()) {
            updateMovement();
            camera.updateMovement(window, timer.getDelta());
        }

        player->intpposition = Mth::lerp(timer.getPartialTick(), player->prevposition, player->position);
        camera.compute(player->intpposition, renderEngine->viewMatrix);

        if (!window.isKeyDown(GLFW_KEY_P))
            renderEngine->updateViewFrustum();

        window.setMouseGrabbed(isIngame());
        window.setTitle(("desp. "+std::to_string(1.0/timer.getDelta())).c_str());

        renderEngine->updateProjectionMatrix(getAspectRatio());
    }

    void runTick()
    {

        world->tick();
    }

    void destroy()
    {
        delete rootGUI;
        delete world;
        delete renderEngine;

        glfwTerminate();
    }


    static void shutdown() { INST->running = false; }
    static bool isRunning() { return INST->running; }
    static bool isIngame() { return getRootGUI()->last() == GuiIngame::INST; }

    static float getPreciseTime() { return (float)Window::getPreciseTime(); }
    static float getAspectRatio() {
        Window* w = getWindow(); if (w->getHeight() == 0) return 0;
        return w->getWidth() / w->getHeight();
    }

    static RenderEngine* getRenderEngine() { return INST->renderEngine; }
    static Window* getWindow() { return &INST->window; }
    static Camera* getCamera() { return &INST->camera; }
    static Executor* getExecutor() { return &INST->executor; }
    static World* getWorld() { return INST->world; }
    static GuiRoot* getRootGUI() { return INST->rootGUI; }



    static void initThreadChunkLoad() {
        new std::thread([]() {
            while (isRunning())
            {
                chunkBuildStat = "ChunkLoad";
                updateViewDistance(getWorld(), getCamera()->position, (int)RenderEngine::viewDistance);

                chunkBuildStat = "ChunkModelUpdate";
                checkChunksModelUpdate(getWorld());
                chunkBuildStat = "None";

                std::this_thread::sleep_for(std::chrono::milliseconds (10));
            }
        });
    }

    static void checkChunksModelUpdate(World* world) {
        glm::vec3 vpos = getCamera()->position;

        int i = 0;
        while (true) {
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
            if (chunk == nullptr)
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

        // load chunks
        for (int dx = -n;dx <= n;dx++) {
            for (int dz = -n;dz <= n;dz++) {
                for (int dy = -n;dy <= n;dy++) {
                    glm::vec3 p = cpos + glm::vec3(dx, dy, dz) * 16.0f;
                    Chunk* c = world->getLoadedChunk(p);
                    if (!c) {
                        world->provideChunk(p);
                    }
                }
            }
        }

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

    void updateMovement() {
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
};

// todo ls:

// multi layer cube cloud
// cube ambient occlusion
// multi block types.
// block density? for isosurface and even SVO?

// Sky gradient.

// Self-spread chunkload order

// opt Sliders ViewDist,FOV,Colors..

// Framebuffer

// Basic Collision Detection
// Entities

// Trees TerrGen


#endif //ETHERTIA_ETHERTIA_H
