//
// Created by Dreamtowards on 2024/8/3.
//


#include "glx.h"
#include <stack>

#include "ethertia/util/Loader.h"
#include <ethertia/render/Window.h>


#pragma region Framebuffer

void Framebuffer::BindMainFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    auto s = Window::FramebufferSize();
    glViewport(0, 0, s.x, s.y);
}

inline static std::stack<Framebuffer*> g_FboStack;

void Framebuffer::BeginFramebuffer(Framebuffer* fbo) {
    fbo->Bind();
    g_FboStack.push(fbo);
}

void Framebuffer::EndFramebuffer() {
    g_FboStack.pop();

    if (g_FboStack.empty()) {  // bind Main Framebuffer
        BindMainFramebuffer();
    } else {
        g_FboStack.top()->Bind();
    }
}


#pragma endregion


void ShaderProgram::Reload() {
    auto vs = Loader::LoadAsset(std::format("{}.vs", m_SourcePath));
    auto fs = Loader::LoadAsset(std::format("{}.fs", m_SourcePath));

    Load((const char*)vs.data(), (const char*)fs.data());
}










void glx::Clear(glm::vec4 color) {
    glClearColor(color.x, color.y, color.z, color.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

GLuint glx::CheckError(std::string_view phase)  {
    GLuint err;
    if ((err = glGetError()) != GL_NO_ERROR) {
        Log::warn("###### GL Error @{} ######", phase);
        Log::warn("ERR: {}", err);
        return err;
    }
    return 0;
}


void APIENTRY _MyDebugOutput(GLenum source,
                            GLenum type,
                            unsigned int id,
                            GLenum severity,
                            GLsizei length,
                            const char *message,
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}

void glx::InitDebugOutput() {
    int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (!(flags & GL_CONTEXT_FLAG_DEBUG_BIT)) {
        Log::warn("Failed to Init OpenGL DebugOutput");
        return;
    }
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(_MyDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
}