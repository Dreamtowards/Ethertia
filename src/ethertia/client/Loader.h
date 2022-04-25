//
// Created by Dreamtowards on 2022/3/30.
//

#ifndef ETHERTIA_LOADER_H
#define ETHERTIA_LOADER_H

#include <initializer_list>
#include <utility>
#include <glad/glad.h>
#include <fstream>


class Loader {

public:

    static std::ifstream loadAssets(const std::string& p)
    {
        std::string abspath = "../src/assets/" + p;
        std::ifstream infile(abspath);
        if (!infile.is_open())
            throw std::runtime_error("Failed open file. "+abspath);
        return infile;
    }

    static std::string loadAssetsStr(const std::string& p)
    {
        std::ifstream s = loadAssets(p);
        return std::string(std::istreambuf_iterator<char>(s),
                std::istreambuf_iterator<char>());
    }

    static GLuint loadModel(int vcount, std::initializer_list<std::pair<float*, int>> vdats) {
        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        int i = 0;
        for (auto vd : vdats) {
            float* vts = vd.first;
            int vsz = vd.second;

            GLuint vbo;
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vsz * vcount, vts, GL_STATIC_DRAW);

            glVertexAttribPointer(i, vsz, GL_FLOAT, false, 0, nullptr);
            glEnableVertexAttribArray(i);
            i++;
        }

        return vao;
    }

};

#endif //ETHERTIA_LOADER_H
