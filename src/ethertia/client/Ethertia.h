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
#include <ethertia/client/audio/AudioEngine.h>
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

        Init::initialize();

        initThreadChunkLoad();

        GuiIngame::INST = new GuiIngame();
        GuiScreenMainMenu::INST = new GuiScreenMainMenu();

        rootGUI->addGui(GuiIngame::INST);
        rootGUI->addGui(GuiScreenMainMenu::INST);

        EventBus::EVENT_BUS.listen([](KeyboardEvent* e) {
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
                      "dt/ {}, {}fs",
                      glm::to_string(camera.position), glm::length(player->velocity),
                      timer.getDelta(), 1.0/timer.getDelta());
        Gui::drawString(0, 32, ss.str(), Colors::WHITE, 16, 0, false);

//        Gui::drawRect(100, 100, 200, 100, Colors::WHITE, BlockTextures::ATLAS->atlasTexture, 20);
//        Gui::drawRect(100, 100, 10, 10, Colors::GREEN);

//        Gui::drawString(Gui::maxWidth()/2, 110, "Test yo wassaup9\nTest\nOf\nSomeTexts\nWill The Center Texting Works?Test yo wassaup9\nTest\nOf\nSomeTexts\nWill The Center Texting Works?Test yo wassaup9\nTest\nOf\nSomeTexts\nWill The Center Texting Works?Test yo wassaup9\nTest\nOf\nSomeTexts\nWill The Center Texting Works?Test yo wassaup9\nTest\nOf\nSomeTexts\nWill The Center Texting Works?",
//                        Colors::WHITE, 32, 1);

        Gui::drawWorldpoint(player->position, [](glm::vec2 p) {

            Gui::drawRect(p.x, p.y, 4, 4, Colors::RED);
        });

        glEnable(GL_DEPTH_TEST);
    }

    void clientUpdate()
    {
        if (isIngame()) {
            updateMovement();
            camera.update(window, timer.getDelta(), renderEngine->viewMatrix);
        }
        if (!window.isKeyDown(GLFW_KEY_P))
            renderEngine->updateViewFrustum();


        window.setMouseGrabbed(isIngame());
        window.setTitle(("desp. "+std::to_string(1.0/timer.getDelta())).c_str());
        renderEngine->updateProjectionMatrix(window.getWidth()/window.getHeight());
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
    static bool isIngame() { return getRootGUI()->last() == GuiIngame::INST && !getWindow()->isAltKeyDown(); }

    static float getPreciseTime() { return (float)Window::getPreciseTime(); }

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
                updateViewDistance(getWorld(), getCamera()->position, 4);

                checkChunksModelUpdate(getWorld());

                std::this_thread::sleep_for(std::chrono::milliseconds (10));
            }
        });
    }

    static void checkChunksModelUpdate(World* world) {

        for (auto& it : world->getLoadedChunks()) {
            Chunk* chunk = it.second;
            if (chunk->needUpdateModel) {
                chunk->needUpdateModel = false;
                World::tmpDoRebuildModel(chunk, world);
            }
        }

    }

    static void updateViewDistance(World* world, glm::vec3 p, int n)
    {
        glm::vec3 cpos = Chunk::chunkpos(p);

        // load chunks
        for (int dx = -n;dx <= n;dx++) {
            for (int dy = -n;dy <= n;dy++) {
                for (int dz = -n;dz <= n;dz++) {
                    world->provideChunk(cpos + glm::vec3(dx, dy, dz) * 16.0f);
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
        float a = camera.eulerAngles.y;

        if (window.isKeyDown(GLFW_KEY_W)) player->velocity += Mth::angleh(a) * speed;
        if (window.isKeyDown(GLFW_KEY_S)) player->velocity += Mth::angleh(a+Mth::PI) * speed;
        if (window.isKeyDown(GLFW_KEY_A)) player->velocity += Mth::angleh(a+Mth::PI/2) * speed;
        if (window.isKeyDown(GLFW_KEY_D)) player->velocity += Mth::angleh(a-Mth::PI/2) * speed;

        if (window.isShiftKeyDown()) player->velocity.y -= speed;
        if (window.isKeyDown(GLFW_KEY_SPACE)) player->velocity.y += speed;

        if (!window.isKeyDown(GLFW_KEY_W)) {
            sprint = false;
        }

        camera.position = Mth::lerp(timer.getPartialTick(), player->prevposition, player->position);

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
