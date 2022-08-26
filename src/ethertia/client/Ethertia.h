//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_ETHERTIA_H
#define ETHERTIA_ETHERTIA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdexcept>

// __forward_declarations

class RenderEngine;      // #include <ethertia/client/render/RenderEngine.h>
class Window;            // #include <ethertia/client/Window.h>
class Timer;             // #include <ethertia/util/Timer.h>
class Camera;            // #include <ethertia/client/render/Camera.h>
class Executor;          // #include <ethertia/util/concurrent/Executor.h>

class World;             // #include <ethertia/world/World.h>
class Entity;            // #include <ethertia/entity/Entity.h>
class GuiRoot;           // #include <ethertia/client/gui/GuiRoot.h>

class Ethertia
{
    static bool running;

    static RenderEngine* renderEngine;
    static Window window;
    static Timer timer;
    static Camera camera;
    static Executor executor;

    static World* world;
    static Entity* player;
    static GuiRoot* rootGUI;

    Ethertia() { throw std::logic_error("No instance"); };

public:

    static void run()
    {
        start();

        while (running)
        {
            runMainLoop();
        }

        destroy();
    }

    static void start();

    static void runMainLoop();
    static void runTick();

    static void destroy();

    static void loadWorld();
    static void unloadWorld();

    static void dispatchCommand(const std::string& cmd);

    static void shutdown() { running = false; }
    static bool isRunning() { return running; }
    static bool isIngame();
    static float getPreciseTime();
    static float getAspectRatio();
    static float getDelta();

    static RenderEngine* getRenderEngine() { return renderEngine; }
    static Window* getWindow() { return &window; }
    static Camera* getCamera() { return &camera; }
    static Executor* getExecutor() { return &executor; }
    static World* getWorld() { return world; }
    static GuiRoot* getRootGUI() { return rootGUI; }
    static Entity* getPlayer() { return player; }
    static Timer* getTimer() { return &timer; }


};

#endif //ETHERTIA_ETHERTIA_H
