//
// Created by Dreamtowards on 2023/2/13.
//

#include <ethertia/render/Window.h>



Window::Window(int _w, int _h, const char *_title) : width(_w), height(_h)
{
    BenchmarkTimer _tm;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);  // OSX Required.
    // glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);

    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

    m_WindowHandle = glfwCreateWindow(width, height, _title, nullptr, nullptr);
    if (!m_WindowHandle) {
        const char* err_str;
        int err = glfwGetError(&err_str);
        throw std::runtime_error(Strings::fmt("Failed to init GLFW window. Err {}. ({})", err, err_str));
    }

    centralize();

    glfwMakeContextCurrent(m_WindowHandle);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Failed to init GLAD.");

    Log::info("Init GL_{}, {} | GLFW {}.\1",
              glGetString(GL_VERSION), glGetString(GL_RENDERER), //glGetString(GL_VENDOR),
              glfwGetVersionString());

    glfwSetWindowUserPointer(m_WindowHandle, this);

    glfwSetWindowCloseCallback(m_WindowHandle, onWindowClose);
    glfwSetWindowSizeCallback(m_WindowHandle, onWindowSize);
    glfwSetFramebufferSizeCallback(m_WindowHandle, onFramebufferSize);
    glfwSetDropCallback(m_WindowHandle, onWindowDropPath);
    glfwSetWindowFocusCallback(m_WindowHandle, onWindowFocus);

    glfwSetCursorPosCallback(m_WindowHandle, onCursorPos);
    glfwSetMouseButtonCallback(m_WindowHandle, onMouseButton);
    glfwSetScrollCallback(m_WindowHandle, onScroll);
    glfwSetKeyCallback(m_WindowHandle, onKeyboardKey);
    glfwSetCharCallback(m_WindowHandle, onCharInput);

    glfwGetFramebufferSize(m_WindowHandle, &framebufferWidth, &framebufferHeight);

}