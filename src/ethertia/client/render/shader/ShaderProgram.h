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

#include <ethertia/util/QuickTypes.h>

class ShaderProgram {

    uint program;
    std::unordered_map<const char*, uint> cachedUniformId;

public:

    ShaderProgram(const std::string& svsh, const std::string& sfsh) {

        uint vsh = loadShader(GL_VERTEX_SHADER, svsh);
        uint fsh = loadShader(GL_FRAGMENT_SHADER, sfsh);

        program = glCreateProgram();

        glAttachShader(program, vsh);
        glAttachShader(program, fsh);
        glLinkProgram(program);

        int succ;
        glGetProgramiv(program, GL_LINK_STATUS, &succ);
        if (!succ) {
            char infolog[512];
            glGetProgramInfoLog(program, 512, nullptr, infolog);
            throw std::logic_error(std::strstr("Failed to link the shader program:\n", infolog));
        }

        glDeleteShader(vsh);
        glDeleteShader(fsh);
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




private:
    static uint loadShader(GLuint shadertype, const std::string& src) {
        uint s = glCreateShader(shadertype);
        const char* cstr = src.c_str();
        glShaderSource(s, 1, &cstr, nullptr);
        glCompileShader(s);

        int succ;
        glGetShaderiv(s, GL_COMPILE_STATUS, &succ);
        if (!succ) {
            char infolog[512];
            glGetShaderInfoLog(s, 512, nullptr, infolog);
            throw std::logic_error(std::strstr("Failed to compile the shader:\n", infolog));
        }
        return s;
    }
};


#endif //ETHERTIA_SHADER_H
