//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_TEXTURE_H
#define ETHERTIA_TEXTURE_H

class Texture
{
    unsigned int textureID;
    unsigned int width;
    unsigned int height;

public:
    Texture(int texId, int w, int h) : textureID(texId), width(w), height(h) {}
};

#endif //ETHERTIA_TEXTURE_H
