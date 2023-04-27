//
// Created by Dreamtowards on 2023/2/18.
//

#include "ShaderProgram.h"


static const char* shader_type_name(GLuint shaderType) {
    switch (shaderType) {
        case GL_VERTEX_SHADER: return "vertex";
        case GL_FRAGMENT_SHADER: return "fragment";
        case GL_GEOMETRY_SHADER: return "geometry";
        default: return "Unknown";
    }
}

GLuint loadShader(GLuint shader_type, const char* src, const char* err_iden) {
    GLuint s = glCreateShader(shader_type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);

    int succ;
    glGetShaderiv(s, GL_COMPILE_STATUS, &succ);
    if (!succ) {
        char infolog[512];
        glGetShaderInfoLog(s, 512, nullptr, infolog);
        Log::warn("Failed to compile the {} shader [{}]:\n", shader_type_name(shader_type), err_iden, infolog);
        return 0;
    }
    return s;
}

void ShaderProgram::load(const char *vsh_src, const char *fsh_src, const char *gsh_src)
{
//    if (!m_ProgramId) {
//        m_ProgramId = glCreateProgram();
//    }
    if (m_ProgramId) {
        glDeleteProgram(m_ProgramId);
    }
    m_ProgramId = glCreateProgram();

    m_Good = true;

    GLuint vsh = loadShader(GL_VERTEX_SHADER, vsh_src, m_SourceLocation.c_str());   if (!vsh) m_Good = false;
    GLuint fsh = loadShader(GL_FRAGMENT_SHADER, fsh_src, m_SourceLocation.c_str()); if (!fsh) m_Good = false;
    GLuint gsh = 0;
    if (gsh_src) {
        gsh = loadShader(GL_GEOMETRY_SHADER, gsh_src, m_SourceLocation.c_str());    if (!gsh) m_Good = false;
    }

    glAttachShader(m_ProgramId, vsh);
    glAttachShader(m_ProgramId, fsh);
    if (gsh) glAttachShader(m_ProgramId, gsh);


    glLinkProgram(m_ProgramId);

    int succ;
    glGetProgramiv(m_ProgramId, GL_LINK_STATUS, &succ);
    if (!succ) {
        char infolog[512];
        glGetProgramInfoLog(m_ProgramId, 512, nullptr, infolog);
        Log::warn("Failed to link the shader program [{}]:\n", m_SourceLocation, infolog);
        m_Good = false;
    }

    glDeleteShader(vsh);
    glDeleteShader(fsh);
    if (gsh) glDeleteShader(gsh);
}






#include <ethertia/Ethertia.h>
#include <ethertia/util/Loader.h>

void ShaderProgram::setViewProjection(bool view)
{
    setMatrix4f("matProjection", Ethertia::getCamera().matProjection);

    setMatrix4f("matView", view ? Ethertia::getCamera().matView : glm::mat4(1.0));
}

void ShaderProgram::reload_sources_by_filenames() {
    Loader::loadShaderProgram(this, m_SourceLocation, m_HaveGeomShader);
}