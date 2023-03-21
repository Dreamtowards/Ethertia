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

int main()
{
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
    static int n = 0;
    static float t = 0;
    if (t > 1.0f) {
        Log::info("{} frames in {} sec.", n, t);
        t = 0; n = 0;
    }
    ++n;
    BENCHMARK_TIMER_VAL(&t);

    Dbg::dbg_PauseImgui = g_Window->isAltKeyDown();

    g_Window->ProcessEvents();

    if (g_Window->isCloseRequested()) {
        g_Running = false;
    }

    VulkanIntl::DrawFrame();


    //Timer::sleep_for(4);
}
