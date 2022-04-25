//
// Created by Dreamtowards on 2022/3/30.
//

#ifndef ETHERTIA_SHADER_H
#define ETHERTIA_SHADER_H

#include <stdexcept>
#include <string>
#include "glad/glad.h"

class ShaderProgram {

    int program;

public:

    ShaderProgram(const std::string& svsh, const std::string& sfsh) {

        int vsh = loadShader(GL_VERTEX_SHADER, svsh);
        int fsh = loadShader(GL_FRAGMENT_SHADER, sfsh);

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

    ~ShaderProgram()
    {
        glDeleteProgram(program);
    }

    void useProgram() const {
        glUseProgram(program);
    }

private:
    static int loadShader(GLuint shadertype, const std::string& src) {
        int s = glCreateShader(shadertype);
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
