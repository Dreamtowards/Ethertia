//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_ETHERTIA_H
#define ETHERTIA_ETHERTIA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

#include <stdexcept>

// __forward_declarations

class RenderEngine;      // #include <ethertia/client/render/RenderEngine.h>
class Window;            // #include <ethertia/client/Window.h>
class Timer;             // #include <ethertia/util/Timer.h>
class Camera;            // #include <ethertia/client/render/Camera.h>
class Executor;          // #include <ethertia/util/concurrent/Executor.h>

class World;             // #include <ethertia/world/World.h>
class EntityPlayer;      // #include <ethertia/entity/player/EntityPlayer.h>
class GuiRoot;           // #include <ethertia/client/gui/GuiRoot.h>

// BrushCursor
class BrushCursor {
public:
    bool keepTracking;
    bool hit;
    glm::vec3 position;
    float size;
    int type;

#define BRUSH_SPHERE 1
#define BRUSH_CUBE   2
};

class Ethertia
{
    static bool running;

    static RenderEngine* renderEngine;
    static Window window;
    static Timer timer;
    static Executor executor;

    static World* world;
    static EntityPlayer* player;
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

    static BrushCursor& getBrushCursor();

    static void shutdown() { running = false; }
    static bool isRunning() { return running; }
    static bool isIngame();
    static float getPreciseTime();
    static float getAspectRatio();
    static float getDelta();

    static RenderEngine* getRenderEngine() { return renderEngine; }
    static Window* getWindow() { return &window; }
    static Camera* getCamera();
    static Executor* getExecutor() { return &executor; }
    static World* getWorld() { return world; }
    static GuiRoot* getRootGUI() { return rootGUI; }
    static EntityPlayer* getPlayer() { return player; }
    static Timer* getTimer() { return &timer; }


};

#endif //ETHERTIA_ETHERTIA_H
