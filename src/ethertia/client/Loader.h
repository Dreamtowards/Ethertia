//
// Created by Dreamtowards on 2022/3/30.
//

#ifndef ETHERTIA_LOADER_H
#define ETHERTIA_LOADER_H

#include <initializer_list>
#include <utility>
#include <glad/glad.h>
#include <fstream>
#include <ethertia/client/render/Texture.h>
#include <ethertia/util/BitmapImage.h>
#include <ethertia/client/render/VertexBuffer.h>
#include <ethertia/client/render/Model.h>


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

    static Model* loadModel(int vcount, const std::vector<std::pair<float*, int>>& vdats) {
        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        int i = 0;
        for (auto vd : vdats) {
            float* vdat = vd.first;
            int vlen = vd.second;

            GLuint vbo;
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vlen*vcount, vdat, GL_STATIC_DRAW);

            glVertexAttribPointer(i, vlen, GL_FLOAT, false, 0, nullptr);
            glEnableVertexAttribArray(i);
            i++;
        }

        return new Model(vao, vcount);
    }
    static Model* loadModel(VertexBuffer* vbuf) {
        std::vector<std::pair<float*, int>> ls;
        ls.emplace_back(&vbuf->positions[0], 3);
        ls.emplace_back(&vbuf->textureCoords[0], 2);
        ls.emplace_back(&vbuf->normals[0], 3);

        return loadModel(vbuf->vertexCount(), ls);
    }

    static Texture* loadTexture(BitmapImage* img) {
        uint texId;
        glGenTextures(1, &texId);
        Texture* tex = new Texture(texId, img->getWidth(), img->getHeight());



        return tex;
    }

};

#endif //ETHERTIA_LOADER_H
