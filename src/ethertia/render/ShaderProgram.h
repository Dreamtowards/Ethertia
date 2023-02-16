//
// Created by Dreamtowards on 2022/3/30.
//

#ifndef ETHERTIA_SHADER_H
#define ETHERTIA_SHADER_H

#include <stdexcept>
#include <string>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <ethertia/util/Log.h>
#include <ethertia/util/Strings.h>

class ShaderProgram {

    GLuint m_ProgramId = 0;
    std::unordered_map<const char*, GLuint> m_CachedUniformId;

    bool m_Good = false;

public:

    // empty.
    ShaderProgram() {}

    ShaderProgram(const ShaderProgram& cpy) = delete;
    ShaderProgram(ShaderProgram&& mov) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    ShaderProgram& operator=(ShaderProgram&& mov) noexcept {
        m_ProgramId = mov.m_ProgramId;
        m_CachedUniformId = std::move(mov.m_CachedUniformId);
        m_Good = mov.m_Good;

        // prevents glDeleteProgram() deleted the program when rvalue deconstruct.
        mov.m_ProgramId = 0;
        return *this;
    }

    ShaderProgram(const std::string& vsh_src, const std::string& fsh_src, const std::string& gsh_src = "") {
        m_Good = true;

        GLuint vsh = loadShader(GL_VERTEX_SHADER, vsh_src);
        GLuint fsh = loadShader(GL_FRAGMENT_SHADER, fsh_src);
        GLuint gsh = 0;
        if (!gsh_src.empty()) {
            gsh = loadShader(GL_GEOMETRY_SHADER, gsh_src);
        }

        m_ProgramId = glCreateProgram();

        glAttachShader(m_ProgramId, vsh);
        glAttachShader(m_ProgramId, fsh);
        if (gsh) glAttachShader(m_ProgramId, gsh);


        glLinkProgram(m_ProgramId);

        int succ;
        glGetProgramiv(m_ProgramId, GL_LINK_STATUS, &succ);
        if (!succ) {
            char infolog[512];
            glGetProgramInfoLog(m_ProgramId, 512, nullptr, infolog);
            Log::warn("Failed to link the shader program:\n", infolog);
            m_Good = false;
        }

        glDeleteShader(vsh);
        glDeleteShader(fsh);
        if (gsh) glDeleteShader(gsh);
    }

    ~ShaderProgram() {
        glDeleteProgram(m_ProgramId);
    }


    int getProgramId() {
        return m_ProgramId;
    }
    void useProgram() const {
        glUseProgram(m_ProgramId);
    }

    GLuint getUniformLocation(const char* name) {
        GLuint loc = m_CachedUniformId[name];
        if (!loc) {
            return m_CachedUniformId[name] = glGetUniformLocation(m_ProgramId, name);
        }
        return loc;
    }

    void setVector2f(const char* name, glm::vec2 v) {
        glUniform2f(getUniformLocation(name), v.x, v.y);
    }
    void setVector3f(const char* name, glm::vec3 v) {
        glUniform3f(getUniformLocation(name), v.x, v.y, v.z);
    }
    void setVector4f(const char* name, glm::vec4 v) {
        glUniform4f(getUniformLocation(name), v.x, v.y, v.z, v.w);
    }

    void setMatrix4f(const char* name, glm::mat4 m) {
        glUniformMatrix4fv(getUniformLocation(name), 1, false, &m[0][0]);
    }

    void setInt(const char* name, int i) {
        glUniform1i(getUniformLocation(name), i);
    }
    void setFloat(const char* name, float f) {
        glUniform1f(getUniformLocation(name), f);
    }

    void setViewProjection(bool view = true);
    void setMVP(const glm::mat4& matModel) {
        setMatrix4f("matModel", matModel);
        setViewProjection();
    }



    // GenArrayNames("chars[{}]", 128);
    static const char** _GenArrayNames(const std::string& namep, int n) {
        const char** arr = new const char*[n];
        for (int i = 0; i < n; ++i) {
            std::string s = Strings::fmt(namep, i);

            size_t size = s.length() + 1;  // +1 for \0
            char* data = new char[size];
            std::memcpy(data, s.c_str(), size);
            arr[i] = data;
        }
        return arr;
    }

//    static const char** lazyArrayNames(const std::string& namep, int size) {
//        static std::unordered_map<std::string, const char**> _cache;
//
//        auto& v = _cache[namep];
//        if (v == nullptr) {
//            v = _GenArrayNames(namep, size);
//        }
//        return v;
//    }


private:
    GLuint loadShader(GLuint shadertype, const std::string& src) {
        GLuint s = glCreateShader(shadertype);
        const char* cstr = src.c_str();
        glShaderSource(s, 1, &cstr, nullptr);
        glCompileShader(s);

        int succ;
        glGetShaderiv(s, GL_COMPILE_STATUS, &succ);
        if (!succ) {
            char infolog[512];
            glGetShaderInfoLog(s, 512, nullptr, infolog);
            Log::warn("Failed to compile the shader:\n", infolog);
            m_Good = false;
        }
        return s;
    }
};


#endif //ETHERTIA_SHADER_H
