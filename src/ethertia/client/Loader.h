//
// Created by Dreamtowards on 2022/3/30.
//

#ifndef ETHERTIA_LOADER_H
#define ETHERTIA_LOADER_H

#include <initializer_list>
#include <utility>
#include <fstream>

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include <ethertia/client/render/Texture.h>
#include <ethertia/util/BitmapImage.h>
#include <ethertia/client/render/VertexBuffer.h>
#include <ethertia/client/render/Model.h>


class Loader {

public:

    static char* loadAssets(const std::string& p, size_t* len_out)
    {
        std::string abspath = "../src/assets/" + p;
        std::ifstream infile(abspath, std::ios_base::binary);
        if (!infile.is_open())
            throw std::runtime_error("Failed open file. "+abspath);
        infile.seekg(0, std::ios_base::end);
        size_t len = infile.tellg();
        infile.seekg(0, std::ios_base::beg);

        char* buf = new char[len];
        infile.read(buf, len);
        infile.close();

        if (len_out) {
            *len_out = len;
        }
        return buf;
    }

    static std::string loadAssetsStr(const std::string& p) {
        size_t len;
        char* dat = loadAssets(p, &len);
        return std::string(dat, len);
    }

    static BitmapImage* loadPNG(const void* data, size_t len) {
        int width, height, channels;
        void* pixels = stbi_load_from_memory((unsigned char*)data, len, &width, &height, &channels, 4);
        return new BitmapImage(width, height, (unsigned int*)pixels);
    }
    static void savePNG(BitmapImage* img, const char* filename) {
        stbi_write_png(filename, img->getWidth(), img->getHeight(), 4, img->getPixels(), 0);
    }

    static Model* loadModel(int vcount, const std::vector<std::pair<int, float*>>& vdats) {
        uint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        auto* m = new Model(vao, vcount);

        int i = 0;
        for (auto vd : vdats) {
            int vlen = vd.first;
            float* vdat = vd.second;

            uint vbo;
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vlen*vcount, vdat, GL_STATIC_DRAW);

            glVertexAttribPointer(i, vlen, GL_FLOAT, false, 0, nullptr);
            glEnableVertexAttribArray(i);
            m->vbos.push_back(vbo);
            i++;
        }
        return m;
    }
    static Model* loadModel(VertexBuffer* vbuf) {
        std::vector<std::pair<int, float*>> ls;
        ls.emplace_back(3, &vbuf->positions[0]);
        ls.emplace_back(2, &vbuf->textureCoords[0]);
        ls.emplace_back(3, &vbuf->normals[0]);

        return loadModel(vbuf->vertexCount(), ls);
    }

    static Texture* loadTexture(BitmapImage* img) {
        uint texID;
        glGenTextures(1, &texID);
        auto* tex = new Texture(texID, img->getWidth(), img->getHeight());

        glBindTexture(GL_TEXTURE_2D, texID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  //GL_LINEAR, GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.2f);

//        if (GL.getCapabilities().GL_EXT_texture_filter_anisotropic) {
//            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0);  // set 0 f use TextureFilterAnisotropic
//
//            float amount = Math.min(4f, glGetFloat(EXTTextureFilterAnisotropic.GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT));
//            glTexParameterf(target, EXTTextureFilterAnisotropic.GL_TEXTURE_MAX_ANISOTROPY_EXT, amount);
//            LOGGER.info("ENABLED GL_EXT_texture_filter_anisotropic");
//         }

        void* pixels = img->getPixels();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->getWidth(), img->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        // glTexSubImage2D();

        glGenerateMipmap(GL_TEXTURE_2D);

        return tex;
    }

};

#endif //ETHERTIA_LOADER_H
