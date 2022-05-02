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
#include <ethertia/client/gui/GuiButton.h>
#include <ethertia/client/gui/GuiColumn.h>
#include <ethertia/client/gui/GuiRow.h>
#include <ethertia/client/gui/GuiPadding.h>
#include <ethertia/client/gui/GuiAlign.h>
#include <ethertia/client/gui/screen/GuiScreenMainMenu.h>


class Ethertia
{
    inline static Ethertia* INST;

    bool running = false;

    RenderEngine* renderEngine;
    // AudioEngine audioEngine;
    Window window;
    Timer timer;
    Camera camera;
    Executor executor{std::this_thread::get_id()};

    World* world;
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

        Init::initialize();

        initThreadChunkLoad();

        rootGUI->addGui(new GuiScreenMainMenu());
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

//        Gui::drawRect(100, 100, 200, 100, Colors::WHITE, nullptr, 2, 4);
//        Gui::drawRect(100, 100, 10, 10, Colors::GREEN);

//        Gui::drawString(Gui::maxWidth()/2, 110, "Test yo wassaup9\nTest\nOf\nSomeTexts\nWill The Center Texting Works?Test yo wassaup9\nTest\nOf\nSomeTexts\nWill The Center Texting Works?Test yo wassaup9\nTest\nOf\nSomeTexts\nWill The Center Texting Works?Test yo wassaup9\nTest\nOf\nSomeTexts\nWill The Center Texting Works?Test yo wassaup9\nTest\nOf\nSomeTexts\nWill The Center Texting Works?",
//                        Colors::WHITE, 32, 1);

        glEnable(GL_DEPTH_TEST);
    }

    void clientUpdate()
    {
        if (isIngame()) {
            camera.update(window);
            updateMovement();
        }
//        window.setMouseGrabbed(isIngame());
        window.setTitle(("desp. "+std::to_string(1.0/timer.getDelta())).c_str());
        renderEngine->updateProjectionMatrix(window.getWidth()/window.getHeight());
    }

    void runTick()
    {


         // world->onTick();
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
    static bool isIngame() { return !getWindow()->isAltKeyDown(); }

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
                updateViewDistance(getWorld(), getCamera()->position, 2);

                std::this_thread::sleep_for(std::chrono::milliseconds (10));
            }
        });
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
        float speed = 0.1;
        if (window.isKeyDown(GLFW_KEY_F)) speed = 1;
        float a = camera.eulerAngles.y;

        if (window.isKeyDown(GLFW_KEY_W)) camera.position += Camera::diff(a) * speed;
        if (window.isKeyDown(GLFW_KEY_S)) camera.position += Camera::diff(a+Mth::PI) * speed;
        if (window.isKeyDown(GLFW_KEY_A)) camera.position += Camera::diff(a+Mth::PI/2) * speed;
        if (window.isKeyDown(GLFW_KEY_D)) camera.position += Camera::diff(a-Mth::PI/2) * speed;

        if (window.isShiftKeyDown()) camera.position.y -= speed;
        if (window.isKeyDown(GLFW_KEY_SPACE)) camera.position.y += speed;

    }
};

// todo ls:
// text/font rendering
// gui

// multi layer cube cloud
// cube ambient occlusion
// multi block types.
// block density? for isosurface and even SVO?

// Fog.


#endif //ETHERTIA_ETHERTIA_H
