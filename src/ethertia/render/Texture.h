//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_TEXTURE_H
#define ETHERTIA_TEXTURE_H

#include <glad/glad.h>

#include <ethertia/util/BitmapImage.h>

class Texture
{
    u32 textureID;
    u32 width;
    u32 height;

public:
    inline static Texture* UNIT;  // 1x1 pixel, RGBA=1 white tex.

    Texture(u32 texId, u32 w, u32 h) : textureID(texId), width(w), height(h) {}

    ~Texture() {
        glDeleteTextures(1, &textureID);
    }

    u32 getTextureID() const {
        return textureID;
    }

    static BitmapImage* glfGetTexImage(Texture* tex) {
        void* pixels = new char[tex->width * tex->height * 4];
        glBindTexture(GL_TEXTURE_2D, tex->textureID);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        return new BitmapImage(tex->width, tex->height, (unsigned int*)pixels);
    }
};

#endif //ETHERTIA_TEXTURE_H
