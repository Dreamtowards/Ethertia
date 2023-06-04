//
// Created by Dreamtowards on 2023/5/23.
//


#include "glc.h"

#include <glad/glad.h>

void test()
{

}

glc::Texture* glc::CreateTexture(GLenum target)
{
    glc::Texture* tex = new glc::Texture();

    glCreateTextures(GL_TEXTURE_2D, 1, &tex->textureId);
    return tex;
}

void glc::TextureParameter(Texture* tex, GLenum pname, GLenum param)
{
    glTextureParameteri(tex->textureId, pname, param);
}


void glc::BindTextureUnit(int unit, glc::Texture* tex)
{
//    glActiveTexture(GL_TEXTURE0 + unit);
//    glBindTexture(tex->target, tex->textureId);

    glBindTextureUnit(unit, tex->textureId);
}

















const char* glc::GetString_DebugMessageEnum(GLenum src_type_serv) {
    switch (src_type_serv) {
        // GL_DEBUG_SOURCE_
        case GL_DEBUG_SOURCE_API: return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW_SYSTEM";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER_COMPILER";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD_PARTY";
        case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
        case GL_DEBUG_SOURCE_OTHER: return "OTHER";
        // GL_DEBUG_TYPE_
        case GL_DEBUG_TYPE_ERROR: return "ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
        case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
        case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
        case GL_DEBUG_TYPE_MARKER: return "MARKER";
        case GL_DEBUG_TYPE_OTHER: return "OTHER";
        // GL_DEBUG_SEVERITY_
        case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
        case GL_DEBUG_SEVERITY_LOW: return "LOW";
        case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
        case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
        default: return "UNKNOWN";
    }
}

struct _IntlDebugMessageUserObj
{
    std::function<void(glc::DebugMessageCallbackArgs)> callback;
};

void callback_gl_debug_message(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
    glc::DebugMessageCallbackArgs args{};
    args.source = source;
    args.type = type;
    args.id = id;
    args.severity = severity;
    args.length = length;
    args.message = message;

    args.source_str = glc::GetString_DebugMessageEnum(source);
    args.type_str = glc::GetString_DebugMessageEnum(type);
    args.severity_str = glc::GetString_DebugMessageEnum(severity);

    ((_IntlDebugMessageUserObj*)user_param)->callback(args);
}

bool glc::DebugMessageCallback(const std::function<void(const glc::DebugMessageCallbackArgs&)>& callback, bool sync)
{
    int contextFlags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &contextFlags);

    if (contextFlags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        if (sync) glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(callback_gl_debug_message, new _IntlDebugMessageUserObj{callback});
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        return true;
    }
    return false;
}