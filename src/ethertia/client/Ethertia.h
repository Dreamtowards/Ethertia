//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_ETHERTIA_H
#define ETHERTIA_ETHERTIA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ethertia/client/render/RenderEngine.h>
#include <ethertia/util/Log.h>
#include <ethertia/world/World.h>
#include <ethertia/util/Timer.h>

#include "Window.h"

class Ethertia
{
    static Ethertia* INST;


    bool running = false;

    RenderEngine* renderEngine;
    Window window;
    Timer timer;

    World* world;

public:
    int viewDistanceChunks;

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
    }

    void runMainLoop()
    {
        timer.update(getPreciseTime());

        while (timer.polltick())
        {
            runTick();
        }

        glClearColor(0, 1, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        renderEngine->renderWorld(world);


        window.updateWindow();
    }

    void runTick()
    {

        world->onTick();
    }

    void destroy()
    {

        delete world;

        delete renderEngine;

        glfwTerminate();
    }


    static void shutdown() { INST->running = false; }

    static Window* getWindow() { return &INST->window; }

    static float getPreciseTime() { return (float)Window::getPreciseTime(); }

};

#endif //ETHERTIA_ETHERTIA_H
