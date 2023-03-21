//
// Created by Dreamtowards on 2023/3/19.
//

#include "Window.h"
#include "VulkanIntl.h"
#include "imgui/Imgui.h"

#include <ethertia/util/BenchmarkTimer.h>
#include <ethertia/util/Log.h>
#include "DebugStat.h"



static bool     g_Running = false;
static Window*  g_Window = nullptr;

static void Init();
static void Destroy();
static void RunMainLoop();

int main(int argc, char** argv, char** env)
{
    // Username: Win[USERNAME] Mac[USER, LOGNAME]
    // Home:     Win[HOMEPATH, USERPROFILE] Mac[Home]
//    while (*env) {
//        Log::info("Env  {}", *env);
//        ++env;
//    }

    Init();

    while (g_Running)
    {
        RunMainLoop();
    }

    Destroy();
}


static void Init()
{
    g_Running = true;

    Window::Init();
    g_Window = new Window(800, 600, "Test");

        GLFWwindow* _glfwWindow = g_Window->m_WindowHandle;
    VulkanIntl::Init(_glfwWindow);
    Imgui::Init(_glfwWindow);

}

static void Destroy()
{
    vkDeviceWaitIdle(VulkanIntl::vkDevice());

    Imgui::Destroy();

    VulkanIntl::Destroy();

    Window::Destroy();
}


static void RunMainLoop()
{
    if (Dbg::_fps_frame(Window::Time()))
        Log::info("{} fps", Dbg::dbg_FPS);
    Dbg::dbg_PauseImgui = g_Window->isAltKeyDown();


    g_Window->ProcessEvents();

    if (g_Window->isCloseRequested())
        g_Running = false;
    //if (g_Window->isFramebufferResized())
        //VulkanIntl::RequestRecreateSwapchain();

    VulkanIntl::DrawFrame();


    if (!g_Window->isCtrlKeyDown())
        Timer::sleep_for(8);
}
