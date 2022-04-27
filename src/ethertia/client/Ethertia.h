//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_ETHERTIA_H
#define ETHERTIA_ETHERTIA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ethertia/client/Window.h>
#include <ethertia/client/render/Camera.h>
#include <ethertia/client/render/RenderEngine.h>
#include <ethertia/util/Timer.h>
#include <ethertia/util/Log.h>
#include <ethertia/util/concurrent/Executor.h>
#include <ethertia/world/World.h>
#include <ethertia/init/BlockTextures.h>


class Ethertia
{
    static Ethertia* INST;

    bool running = false;

    RenderEngine* renderEngine;
    Window window;
    Timer timer;
    Camera camera;
    Executor executor{std::this_thread::get_id()};

    World* world;

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

        BlockTextures::init();
        initThreadChunkLoad();

        Log::info("Initialized.");
    }

    void runMainLoop()
    {
        timer.update(getPreciseTime());

        executor.processTasks();

//        World::updateViewDistance(getWorld(), getCamera()->position, 4);
//        while (timer.polltick())
        {
            runTick();
        }
        if (isIngame()) {
            Camera::update(camera, window);
        }
        window.setMouseGrabbed(isIngame());


        renderEngine->renderWorld(world);

        window.updateWindow();
    }

    void runTick()
    {
        float speed = 0.1;
        if (window.isKeyDown(GLFW_KEY_LEFT_CONTROL)) speed = 1;
        float a = camera.eulerAngles.y;

        if (window.isKeyDown(GLFW_KEY_W)) camera.position += Camera::diff(a) * speed;
        if (window.isKeyDown(GLFW_KEY_S)) camera.position += Camera::diff(a+Mth::PI) * speed;
        if (window.isKeyDown(GLFW_KEY_A)) camera.position += Camera::diff(a+Mth::PI/2) * speed;
        if (window.isKeyDown(GLFW_KEY_D)) camera.position += Camera::diff(a-Mth::PI/2) * speed;

        if (window.isShiftKeyDown()) camera.position.y -= speed;
        if (window.isKeyDown(GLFW_KEY_SPACE)) camera.position.y += speed;



//        world->onTick();
    }

    void destroy()
    {
        delete world;
        delete renderEngine;

        glfwTerminate();
    }


    static void shutdown() { INST->running = false; }
    static bool isRunning() { return INST->running; }

    static float getPreciseTime() { return (float)Window::getPreciseTime(); }

    static Window* getWindow() { return &INST->window; }
    static Camera* getCamera() { return &INST->camera; }
    static Executor* getExecutor() { return &INST->executor; }

    static World* getWorld() { return INST->world; }

    static bool isIngame() { return !getWindow()->isAltKeyDown(); }


    static void initThreadChunkLoad()
    {
        new std::thread([]() {
            while (isRunning())
            {
                World::updateViewDistance(getWorld(), getCamera()->position, 4);

                std::this_thread::sleep_for(std::chrono::milliseconds (100));
            }
        });
    }
};

#endif //ETHERTIA_ETHERTIA_H
