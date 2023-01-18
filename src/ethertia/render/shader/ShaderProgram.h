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

    GLuint program;
    std::unordered_map<const char*, GLuint> cachedUniformId;

public:

    bool m_Good = true;

    ShaderProgram(const std::string& vsh_src, const std::string& fsh_src, const std::string& gsh_src = "") {

        GLuint vsh = loadShader(GL_VERTEX_SHADER, vsh_src);
        GLuint fsh = loadShader(GL_FRAGMENT_SHADER, fsh_src);
        GLuint gsh = 0;
        if (!gsh_src.empty()) {
            gsh = loadShader(GL_GEOMETRY_SHADER, gsh_src);
        }

        program = glCreateProgram();

        glAttachShader(program, vsh);
        glAttachShader(program, fsh);
        if (gsh) glAttachShader(program, gsh);


        glLinkProgram(program);

        int succ;
        glGetProgramiv(program, GL_LINK_STATUS, &succ);
        if (!succ) {
            char infolog[512];
            glGetProgramInfoLog(program, 512, nullptr, infolog);
            Log::warn("Failed to link the shader program:\n", infolog);
            m_Good = false;
        }

        glDeleteShader(vsh);
        glDeleteShader(fsh);
        if (gsh) glDeleteShader(gsh);
    }

    ~ShaderProgram() {
        glDeleteProgram(program);
    }


    void useProgram() const {
        glUseProgram(program);
    }

    uint getUniformLocation(const char* name) {
        uint loc = cachedUniformId[name];
        if (!loc) {
            return cachedUniformId[name] = glGetUniformLocation(program, name);
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
    void setMVP(const glm::mat4& matModel);



    // GenArrayNames("chars[%i]", 128);
    static const char** _GenArrayNames(const std::string& namep, uint n) {
        const char** arr = new const char*[n];
        u32 baselen = namep.length()+3;  // +2: brackets, +1: \0.
        for (int i = 0; i < n; ++i) {
            char* ch = new char[baselen+3];  // +3 assume idx <= 999.
            sprintf(ch, namep.c_str(), i);
            arr[i] = ch;
        }
        return arr;
    }


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
