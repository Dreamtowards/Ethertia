//
// Created by Dreamtowards on 2022/3/30.
//

#ifndef ETHERTIA_LOADER_H
#define ETHERTIA_LOADER_H

#include <initializer_list>
#include <utility>
#include <fstream>
#include <array>

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include <ethertia/client/render/Texture.h>
#include <ethertia/util/BitmapImage.h>
#include <ethertia/client/render/VertexBuffer.h>
#include <ethertia/client/render/Model.h>
#include <ethertia/util/OBJLoader.h>


class Loader {

public:

    static std::pair<char*, u32> loadAssets(const std::string& p)
    {
        std::string abspath = "../src/assets/" + p;
        std::ifstream infile(abspath, std::ios_base::binary);
        if (!infile.is_open())
            throw std::runtime_error("Failed open file. "+abspath);
        infile.seekg(0, std::ios_base::end);
        u32 len = infile.tellg();
        infile.seekg(0, std::ios_base::beg);

        char* buf = new char[len];
        infile.read(buf, len);
        infile.close();

        return std::pair(buf, len);
    }

    static std::string loadAssetsStr(const std::string& p) {
        auto m = loadAssets(p);
        return std::string(m.first, m.second);
    }

    static VertexBuffer* loadOBJ_(const std::string& path) {
        VertexBuffer* vbuf = new VertexBuffer();
        std::stringstream ss(Loader::loadAssetsStr(path));
        OBJLoader::loadOBJ(ss, vbuf);
        return vbuf;
    }


    static BitmapImage* loadPNG(const void* data, u32 len) {
        int width, height, channels;
        void* pixels = stbi_load_from_memory((unsigned char*)data, len, &width, &height, &channels, 4);
        return new BitmapImage(width, height, (unsigned int*)pixels);
    }
    static BitmapImage* loadPNG(std::pair<void*, u32> m) {
        return loadPNG(m.first, m.second);
    }
    static void savePNG(BitmapImage* img, const char* filename) {
        stbi_write_png(filename, img->getWidth(), img->getHeight(), 4, img->getPixels(), 0);
    }

    static Model* loadModel(u32 vcount, const std::vector<std::pair<u32, float*>>& vdats) {
        u32 vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        auto* m = new Model(vao, vcount);

        int i = 0;
        for (auto vd : vdats) {
            int vlen = vd.first;
            float* vdat = vd.second;

            u32 vbo;
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
    static Model* loadModel(VertexBuffer* vbuf, bool materials = false) {
        std::vector<std::pair<u32, float*>> ls;
        ls.emplace_back(3, &vbuf->positions[0]);
        ls.emplace_back(2, &vbuf->textureCoords[0]);
        ls.emplace_back(3, &vbuf->normals[0]);
        if (materials) {
            ls.emplace_back(1, &vbuf->materialIds[0]);
        }

        return loadModel(vbuf->vertexCount(), ls);
    }
    static Model* loadModel(u32 vcount, std::initializer_list<std::pair<u32, float*>> vdats) {
        return loadModel(vcount, std::vector(vdats));
    }

    static Texture* loadTexture(BitmapImage* img) {
        GLuint texId;
        glGenTextures(1, &texId);

        u32 w = img->getWidth();
        u32 h = img->getHeight();
        Texture* tex = new Texture(texId, w, h);

        glBindTexture(GL_TEXTURE_2D, texId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  //GL_LINEAR, GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.2f);
//        if (GL.getCapabilities().GL_EXT_texture_filter_anisotropic) {
//            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0);  // set 0 if use TextureFilterAnisotropic
//            float amount = Math.min(4f, glGetFloat(EXTTextureFilterAnisotropic.GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT));
//            glTexParameterf(target, EXTTextureFilterAnisotropic.GL_TEXTURE_MAX_ANISOTROPY_EXT, amount);
//            LOGGER.info("ENABLED GL_EXT_texture_filter_anisotropic");
//         }

        u32 pixels[w * h];
        img->getVerticalFlippedPixels(pixels);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        // glTexSubImage2D();

        glGenerateMipmap(GL_TEXTURE_2D);

        return tex;
    }

    // imgs order: Right, Left, Top, Bottom, Front, Back.
    static Texture* loadCubeMap(std::vector<BitmapImage*> imgs) {
        assert(imgs.size() == 6);
        int w = imgs[0]->getWidth();
        int h = imgs[0]->getHeight();

        GLuint texId;
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texId);
        Texture* tex = new Texture(texId, w, h);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        for (int i = 0; i < 6; ++i) {
            BitmapImage* img = imgs[i];
            assert(img->getWidth() == w && img->getHeight() == h);

            // flipped y.
            void* pixels = img->getPixels();

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        }

        return tex;
    }

};

#endif //ETHERTIA_LOADER_H
