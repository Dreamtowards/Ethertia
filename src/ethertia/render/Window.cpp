//
// Created by Dreamtowards on 2023/2/13.
//

#include <ethertia/render/Window.h>

#include <glad/glad.h>


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}


void Window::init()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        throw std::runtime_error("failed to init glfw.");

    if (!glfwVulkanSupported())
        throw std::runtime_error("GLFW: Vulkan not supported.");

}

void Window::deinit()
{
    glfwTerminate();
}



static void setup_glfw_callbacks(GLFWwindow* _win);


Window::Window(int _w, int _h, const char* _title) : m_Width(_w), m_Height(_h)
{
    BENCHMARK_TIMER;

//    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  // disable OpenGL

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);  // Required on Mac OSX.
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);

    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

    m_WindowHandle = glfwCreateWindow(_w, _h, _title, nullptr, nullptr);
    if (!m_WindowHandle) {
        const char* err_str;
        int err = glfwGetError(&err_str);
        throw std::runtime_error(Strings::fmt("failed to init glfw window: {}. ({})", err, err_str));
    }

    glfwMakeContextCurrent(m_WindowHandle);
    glfwSwapInterval(0);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Failed to init GLAD.");

//    Log::info("Init GL_{}, {} | GLFW {}.\1",
//              glGetString(GL_VERSION), glGetString(GL_RENDERER), //glGetString(GL_VENDOR),
//              glfwGetVersionString());

    centralize();

    glfwSetWindowUserPointer(m_WindowHandle, this);

    setup_glfw_callbacks(m_WindowHandle);

    glfwGetFramebufferSize(m_WindowHandle, &m_FramebufferWidth, &m_FramebufferHeight);

}

Window::~Window()
{
    glfwDestroyWindow(m_WindowHandle);
}



bool Window::isCloseRequested() {
    return glfwWindowShouldClose(m_WindowHandle);
}

bool Window::isFramebufferResized() {
    return m_IsFramebufferResized;
}







void Window::centralize()
{
    const GLFWvidmode* vmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int w, h;
    glfwGetWindowSize(m_WindowHandle, &w, &h);
    glfwSetWindowPos(m_WindowHandle, (vmode->width - w) / 2, (vmode->height - h) / 2);
}

double Window::getPreciseTime()
{
    return glfwGetTime();
}




void Window::fullscreen(GLFWmonitor* monitor)
{
    m_Fullscreen = true;

    const GLFWvidmode* vmode = glfwGetVideoMode(monitor);
    glfwSetWindowMonitor(m_WindowHandle, monitor, 0, 0, vmode->width, vmode->height, 0);
}

void Window::restoreFullscreen(int _w, int _h)
{
    m_Fullscreen = false;

    glfwSetWindowMonitor(m_WindowHandle, nullptr, 0, 0, _w, _h, 0);
    centralize();
}

bool Window::isFullscreen() const {
    return m_Fullscreen;
}

void Window::toggleFullscreen()
{
    if (isFullscreen()) {
        restoreFullscreen();
    } else {
        fullscreen();
    }
}









#define UNPACK_win Window* win = (Window*)glfwGetWindowUserPointer(_win)

static void onFramebufferSize(GLFWwindow* _win, int wid, int hei)
{
    UNPACK_win;
    win->m_FramebufferWidth  = wid;
    win->m_FramebufferHeight = hei;

    win->m_IsFramebufferResized = true;
}

static void onWindowSize(GLFWwindow* _win, int wid, int hei)
{
    UNPACK_win;
    win->m_Width  = wid;
    win->m_Height = hei;
}




// CNS: 这里有2个不准确的 DX，就是在切换 glfwSetInputMode(...DISABLE/NORMAL) 的时候
// 当case 1. 禁用变成启用后时 鼠标位置突然从 禁用时的最后位置(启用前的位置) 变成了上次最后启用时的位置 这是由于内部状态特例改变 而造成的没有意义的DX
//           但由于这种情况 通常只在禁用时检测DX 所以启用后产生的巨大DX 通常影响不到 因为自从启用 就已经不检测DX了
// 当case 2. 启用变成禁用后 内部会设置状态 产生巨大DX 由于禁用时会监听DX 所以会Bang的一下一个巨大DX 由最后启用状态位置 变成最后禁用状态位置的DX
//
static void onCursorPos(GLFWwindow* _win, double xpos, double ypos)
{
    UNPACK_win;
    float x = (float)xpos;
    float y = (float)ypos;
    // for fix bug:
    // Extreme DX caused by glfwSetInputMode(GLFW_CURSOR_DISABLE/NORMAL), glfw internal will set CursorPos when
    // switching Cursor's Disable/Normal. so we just ignore the first DX after that switch.
    if (win->m_GrabbedChanged) {
        win->m_GrabbedChanged = false;
        win->m_MouseX = x;
        win->m_MouseY = y;
        return;
    }
    win->m_MouseDX = x - win->m_MouseX;
    win->m_MouseDY = y - win->m_MouseY;
    win->m_MouseX = x;
    win->m_MouseY = y;
}


static void onScroll(GLFWwindow* _win, double xoffset, double yoffset)
{
    UNPACK_win;
    win->m_ScrollDX = (float)xoffset;
    win->m_ScrollDY = (float)yoffset;
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


static void setup_glfw_callbacks(GLFWwindow* _win)
{
    glfwSetWindowSizeCallback(_win, onWindowSize);
    glfwSetFramebufferSizeCallback(_win, onFramebufferSize);

    glfwSetCursorPosCallback(_win, onCursorPos);
    glfwSetScrollCallback(_win, onScroll);
//    glfwSetMouseButtonCallback(_win, onMouseButton);
//    glfwSetKeyCallback(_win, onKeyboardKey);
//    glfwSetCharCallback(_win, onCharInput);
}