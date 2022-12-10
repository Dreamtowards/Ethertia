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
class Scheduler;         // #include <ethertia/util/concurrent/Scheduler.h>

class World;             // #include <ethertia/world/World.h>
class EntityPlayer;      // #include <ethertia/entity/player/EntityPlayer.h>
class GuiRoot;           // #include <ethertia/client/gui/GuiRoot.h>

// BrushCursor
class BrushCursor {
public:
    bool keepTracking = true;
    bool hit;
    glm::vec3 position;

    float brushSize = 0.0;

    int brushType;
    int brushMaterial;

#define BRUSH_SPHERE 1
#define BRUSH_CUBE   2
};

class Ethertia
{
    inline static bool          m_Running      = false;
    inline static RenderEngine* m_RenderEngine = nullptr;
    inline static World*        m_World        = nullptr;
    inline static EntityPlayer* m_Player       = nullptr;
    inline static GuiRoot*      m_RootGUI      = nullptr;

    static Window    m_Window;
    static Timer     m_Timer;
    static Scheduler m_Scheduler;
    static BrushCursor m_Cursor;


    Ethertia() { throw std::logic_error("No instance"); };

public:

    static void run()
    {
        start();

        while (m_Running)
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

    static void shutdown() { m_Running = false; }
    static bool isRunning() { return m_Running; }
    static bool isIngame();
    static float getPreciseTime();
    static float getAspectRatio();
    static float getDelta();

    static RenderEngine* getRenderEngine() { return m_RenderEngine; }
    static Window* getWindow() { return &m_Window; }
    static Camera* getCamera();
    static Scheduler* getScheduler() { return &m_Scheduler; }
    static World* getWorld() { return m_World; }
    static GuiRoot* getRootGUI() { return m_RootGUI; }
    static EntityPlayer* getPlayer() { return m_Player; }
    static Timer* getTimer() { return &m_Timer; }
    static BrushCursor& getBrushCursor() { return m_Cursor; }


};

#endif //ETHERTIA_ETHERTIA_H
