//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_TEXTURE_H
#define ETHERTIA_TEXTURE_H

//#include <glad/glad.h>

#include <ethertia/util/BitmapImage.h>

class Texture
{
private:
    Texture() = default;
public:
    int texId = 0;
    int width = 0;
    int height = 0;
    int target = 0;

    // load by RenderEngine::init();
    inline static Texture* WHITE = nullptr;  // 1x1 pixel, RGBA=1 white tex.
    inline static Texture* DEBUG = nullptr;  // uvmap.png

//    enum Type
//    {
//        TEX_2D,
//        TEX_CUBE_MAP
//    } type;

    static Texture* GenTexture(int w, int h, int t = GL_TEXTURE_2D);
    ~Texture();

    void BindTexture(int slot = 0);

    BitmapImage* GetTexImage();

    void* pTexId() {
        return (void*)(intptr_t)texId;
    }
};


//#include <glad/glad.h>
//
//#include <ethertia/util/BitmapImage.h>
//
//// Represents an OpenGL Texture. data in GPU.
//class Texture
//{
//public:
//    GLuint texId;
//    int width;
//    int height;
//
//    // load by RenderEngine::init();
//    inline static Texture* WHITE = nullptr;  // 1x1 pixel, RGBA=1 white tex.
//    inline static Texture* DEBUG = nullptr;  // uvmap.png
//
//    Texture(GLuint texId, int w, int h) : texId(texId), width(w), height(h) {}
//
//    ~Texture() {
//        glDeleteTextures(1, &texId);
//    }
//
////    GLuint getTextureID() const {
////        return texId;
////    }
//
//    void* texId_ptr() {
//        return (void*)(intptr_t)texId;
//    }
//
//    void bindTexture2D(int i = 0) {
//        glActiveTexture(GL_TEXTURE0+i);
//        glBindTexture(GL_TEXTURE_2D, texId);
//    }
//
////    void _BindTexture(int i) {
////        bindTexture2D(i);
////    }
//
//    static BitmapImage* glfGetTexImage(Texture* tex) {
//
//        glBindTexture(GL_TEXTURE_2D, tex->texId);
//        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
//        return new BitmapImage(tex->width, tex->height, (unsigned int*)pixels);
//    }
//};
//
#endif //ETHERTIA_TEXTURE_H
