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
    BitmapImage(void* pixels, int w, int h, const std::string& _filename = "");
    ~BitmapImage();


    // uint32[] of R8G8B8A8. this will be free() at dtor.
    void* m_Pixels = nullptr;

    int m_Width = 0;
    int m_Height = 0;


    std::string m_Filename;  // optional. debug tag.
};

#endif //ETHERTIA_BITMAPIMAGE_H
