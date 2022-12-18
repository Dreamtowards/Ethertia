//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_ETHERTIA_H
#define ETHERTIA_ETHERTIA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <stdexcept>
#include <thread>

#include <ethertia/util/Timer.h>
#include <ethertia/util/concurrent/Scheduler.h>
#include <ethertia/render/Camera.h>
#include <ethertia/init/BrushCursor.h>

// __forward_declarations

class RenderEngine;      // #include <ethertia/client/render/RenderEngine.h>
class World;             // #include <ethertia/world/World.h>
class EntityPlayer;      // #include <ethertia/entity/player/EntityPlayer.h>
class GuiRoot;           // #include <ethertia/client/gui/GuiRoot.h>
class Window;            // #include <ethertia/client/Window.h>


class Ethertia
{
    inline static bool          m_Running      = false;
    inline static RenderEngine* m_RenderEngine = nullptr;
    inline static World*        m_World        = nullptr;
    inline static EntityPlayer* m_Player       = nullptr;
    inline static GuiRoot*      m_RootGUI      = nullptr;
    inline static Window*       m_Window       = nullptr;
    inline static Timer         m_Timer{};
    inline static Scheduler     m_Scheduler{std::this_thread::get_id()};
    inline static BrushCursor   m_Cursor{};


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
    static void renderGUI();

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
    static Window* getWindow() { return m_Window; }
    static World* getWorld() { return m_World; }
    static GuiRoot* getRootGUI() { return m_RootGUI; }
    static EntityPlayer* getPlayer() { return m_Player; }

    static Timer* getTimer() { return &m_Timer; }
    static Scheduler* getScheduler() { return &m_Scheduler; }
    static BrushCursor& getBrushCursor() { return m_Cursor; }
    static Camera* getCamera();


};

#endif //ETHERTIA_ETHERTIA_H
