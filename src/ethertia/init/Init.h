//
// Created by Dreamtowards on 2022/4/29.
//

#ifndef ETHERTIA_INIT_H
#define ETHERTIA_INIT_H

#include "MaterialTextures.h"

class Init
{
public:

    static void initialize()
    {
        MaterialTextures::init();

        // init Texture::UNIT.
        BitmapImage img(1, 1, new u32[1]{(u32)~0});
        Texture::UNIT = Loader::loadTexture(&img);

        Log::info("Textures initialized.");
    }
};

#endif //ETHERTIA_INIT_H
