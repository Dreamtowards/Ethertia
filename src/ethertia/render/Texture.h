//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_TEXTURE_H
#define ETHERTIA_TEXTURE_H

#include <glad/glad.h>

#include <ethertia/util/BitmapImage.h>

class Texture
{
public:
    GLuint texId;
    int width;
    int height;

    Texture(GLuint texId, int w, int h) : texId(texId), width(w), height(h) {}

    ~Texture() {
        glDeleteTextures(1, &texId);
    }

    GLuint getTextureID() const {
        return texId;
    }

    static BitmapImage* glfGetTexImage(Texture* tex) {
        void* pixels = new char[tex->width * tex->height * 4];
        glBindTexture(GL_TEXTURE_2D, tex->texId);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        return new BitmapImage(tex->width, tex->height, (unsigned int*)pixels);
    }
};

#endif //ETHERTIA_TEXTURE_H
