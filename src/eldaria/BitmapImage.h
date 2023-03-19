//
// Created by Dreamtowards on 2023/3/19.
//

#ifndef ETHERTIA_BITMAPIMAGE_H
#define ETHERTIA_BITMAPIMAGE_H

// RGBA 4 Channels.

#include <string>

class BitmapImage
{
public:




    // uint32[] of R8G8B8A8.
    void* m_Pixels = nullptr;

    int m_Width = 0;
    int m_Height = 0;

    // e.g. sometimes m_Pixels are allocated from stb_image, then there should have a stbi_image_free()
    void(*m_PixelsFreeFunc)(void* p) = nullptr;

    // just a tag for debug.
    std::string m_Filename;
};

#endif //ETHERTIA_BITMAPIMAGE_H
