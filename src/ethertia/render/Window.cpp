//
// Created by Dreamtowards on 2023/2/13.
//

#include "Window.h"

#include <format>

#include <ethertia/util/Log.h>
#include <ethertia/util/BenchmarkTimer.h>


static GLFWwindow* g_GlfwWindow = nullptr;

static void _SetupGlfwCallbacks();

static void _GlfwErrorCallback(int error, const char* description)
{
    Log::warn("GLFW Error: {}: {}", error, description);
}


static glm::vec2 g_FramebufferSize;
static glm::vec2 g_WindowSize;
static glm::vec2 g_MousePos;
static glm::vec2 g_MouseDelta;
static glm::vec2 g_MouseWheel;

static bool g_MouseGrabbed = false;
static bool g_Patch_GrabbedChanged = false;  // bug patch


void Window::Init(int _w, int _h, const char* _title)
{
    BENCHMARK_TIMER;

    glfwSetErrorCallback(_GlfwErrorCallback);
    if (!glfwInit())
        throw std::runtime_error("failed to init glfw.");

    Log::info("GLFW {}", glfwGetVersionString());


    if (!glfwVulkanSupported())
        Log::warn("GLFW Error: Vulkan Not Supported.");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // disable OpenGL


    _w = std::max(1, _w); _h = std::max(1, _h);  // couldn't create 0x0 window.
    g_GlfwWindow = glfwCreateWindow(_w, _h, _title, nullptr, nullptr);
    if (!g_GlfwWindow) {
        const char* err_str;
        int err = glfwGetError(&err_str);
        throw std::runtime_error(std::format("Failed to init glfw window: {}. ({})", err, err_str));
    }


    // ADJUST

    Window::Centralize();

    _SetupGlfwCallbacks();


    {   // fixed: g_WindowSize stay 0 if no window size changes. (WindowSize callback wouldn't been call on create window.)
        int w, h;
        glfwGetWindowSize(g_GlfwWindow, &w, &h);
        g_WindowSize = {w, h};
    }

    Log::info("Window initialized.\1");
}

void Window::Destroy()
{
    glfwDestroyWindow(g_GlfwWindow);

    glfwTerminate();
}

double Window::PreciseTime() {
    return glfwGetTime();
}

GLFWwindow* Window::Handle() {
    return g_GlfwWindow;
}



bool Window::isCloseRequested() {
    return glfwWindowShouldClose(g_GlfwWindow);
}

static bool g_IsFramebufferResized = false;  // fb-resized may additionally caused by monitor-change, system-scale-change.

bool Window::isFramebufferResized() {
    return g_IsFramebufferResized;
}

void Window::Centralize() {
    const GLFWvidmode* vmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int w, h;
    glfwGetWindowSize(g_GlfwWindow, &w, &h);
    glfwSetWindowPos(g_GlfwWindow, (vmode->width - w) / 2, (vmode->height - h) / 2);
}



static bool g_IsFullscreen = false;


void Window::SetFullscreen(GLFWmonitor* monitor)
{
    g_IsFullscreen = true;

    const GLFWvidmode* vmode = glfwGetVideoMode(monitor);
    glfwSetWindowMonitor(g_GlfwWindow, monitor, 0, 0, vmode->width, vmode->height, 0);
}

void Window::UnsetFullscreen(int w, int h)
{
    g_IsFullscreen = false;

    glfwSetWindowMonitor(g_GlfwWindow, nullptr, 0, 0, w, h, 0);
    Window::Centralize();
}

bool Window::isFullscreen() {
    return g_IsFullscreen;
}

void Window::ToggleFullscreen()
{
    if (isFullscreen()) {
        UnsetFullscreen();
    } else {
        SetFullscreen();
    }
}






void Window::PollEvents()
{
    // Reset FrameStates
    g_MouseDelta = {0, 0};
    g_MouseWheel = {0, 0};
    g_IsFramebufferResized = false;

    // PollEvents
    glfwPollEvents();
}

glm::vec2 Window::MousePos() {
    return g_MousePos;
}
glm::vec2 Window::MouseDelta() {
    return g_MouseDelta;
}
glm::vec2 Window::MouseWheel() {
    return g_MouseWheel;
}

void Window::SetMouseGrabbed(bool grabbed)
{
    if (g_MouseGrabbed != grabbed)
    {
        g_MouseGrabbed = grabbed;
        g_Patch_GrabbedChanged = true;

        glfwSetInputMode(g_GlfwWindow, GLFW_CURSOR, g_MouseGrabbed ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
}

void Window::SetStickyKeys(bool s) {
    glfwSetInputMode(g_GlfwWindow, GLFW_STICKY_KEYS, s ? GLFW_TRUE : GLFW_FALSE);
}

glm::vec2 Window::Size() {
    return g_WindowSize;
}

void Window::SetTitle(const char* s) {
    glfwSetWindowTitle(g_GlfwWindow, s);
}

bool Window::isKeyDown(int key) {
    return glfwGetKey(g_GlfwWindow, key) == GLFW_PRESS;
}
bool Window::isMouseDown(int button) {
    return glfwGetMouseButton(g_GlfwWindow, button) == GLFW_PRESS;
}


const char* Window::GetClipboard() {
    return glfwGetClipboardString(g_GlfwWindow);
}

void Window::SetClipboard(const char* str) {
    glfwSetClipboardString(g_GlfwWindow, str);
}


//#define UNPACK_win Window* win = (Window*)glfwGetWindowUserPointer(_win)

static void GlfwCallback_FramebufferSize(GLFWwindow* _win, int wid, int hei)
{
    g_FramebufferSize = {wid, hei};
    g_IsFramebufferResized = true;
}

static void GlfwCallback_WindowSize(GLFWwindow* _win, int wid, int hei)
{
    g_WindowSize = {wid, hei};
}


// CNS: 这里有2个不准确的 DX，就是在切换 glfwSetInputMode(...DISABLE/NORMAL) 的时候
// 当case 1. 禁用变成启用后时 鼠标位置突然从 禁用时的最后位置(启用前的位置) 变成了上次最后启用时的位置 这是由于内部状态特例改变 而造成的没有意义的DX
//           但由于这种情况 通常只在禁用时检测DX 所以启用后产生的巨大DX 通常影响不到 因为自从启用 就已经不检测DX了
// 当case 2. 启用变成禁用后 内部会设置状态 产生巨大DX 由于禁用时会监听DX 所以会Bang的一下一个巨大DX 由最后启用状态位置 变成最后禁用状态位置的DX
//
static void GlfwCallback_CursorPos(GLFWwindow* _win, double xpos, double ypos)
{
    glm::vec2 pos = {xpos, ypos};
    // for fix bug:
    // Extreme DX caused by glfwSetInputMode(GLFW_CURSOR_DISABLE/NORMAL), glfw internal will set CursorPos when
    // switching Cursor's Disable/Normal. so we just ignore the first DX after that switch.
    if (g_Patch_GrabbedChanged) {
        g_Patch_GrabbedChanged = false;
        g_MousePos = pos;
        return;
    }
    g_MouseDelta = pos - g_MousePos;
    g_MousePos = pos;
}


static void GlfwCallback_Scroll(GLFWwindow* _win, double xoffset, double yoffset)
{
    g_MouseWheel = {xoffset, yoffset};
}

//static void onKeyboardKey(GLFWwindow* _win, int key, int scancode, int action, int mods) {
//    UNPACK_win;
//
//    KeyboardEvent e(key, action==GLFW_PRESS );
//    win->eventbus().post(&e);
//}
//
//static void onCharInput(GLFWwindow* _win, unsigned int codepoint) {
//    UNPACK_win;
//
//    CharInputEvent e(codepoint);
//    win->eventbus().post(&e);
//}


static void _SetupGlfwCallbacks()
{
    GLFWwindow* win = g_GlfwWindow;
    glfwSetWindowSizeCallback(win, GlfwCallback_WindowSize);
    glfwSetFramebufferSizeCallback(win, GlfwCallback_FramebufferSize);

    glfwSetCursorPosCallback(win, GlfwCallback_CursorPos);
    glfwSetScrollCallback(win, GlfwCallback_Scroll);
//    glfwSetMouseButtonCallback(_win, onMouseButton);
//    glfwSetKeyCallback(_win, onKeyboardKey);
//    glfwSetCharCallback(_win, onCharInput);
}