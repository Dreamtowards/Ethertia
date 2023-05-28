//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_BITMAPIMAGE_H
#define ETHERTIA_BITMAPIMAGE_H

#include <ethertia/util/Colors.h>

#include <stb/stb_image_resize.h>
#include <algorithm>

class BitmapImage
{
    uint32_t* m_Pixels;  // pixel: u32 RGBA
    int m_Width;
    int m_Height;

public:
    BitmapImage(int w, int h);
    BitmapImage(int w, int h, void* pxs);  // pixels in. e.g. quick load from stbi etc.
    ~BitmapImage();

    BitmapImage(const BitmapImage& cpy) = delete;
    BitmapImage(const BitmapImage&& cpy) = delete;


    static void resize(const BitmapImage& src,
                       BitmapImage& dst,
                       bool nearestSample = false);  // true=NearestSample, false=STBIR_DEFAULT_FILTER_DOWNSAMPLE(STBIR_FILTER_MITCHELL) STBIR_DEFAULT_FILTER_UPSAMPLE(STBIR_FILTER_CATMULLROM)


    static void CopyPixels(int srcX, int srcY, const BitmapImage& srcImg,
                           int dstX, int dstY, BitmapImage& dstImg,
                           int cpyWidth = -1, int cpyHeight = -1,  // -1: Full srcImg size. width and height.
                           int specChannel = -1);  // -1: u32 RGBA, 0/1/2/3: one of u8 R/G/B/A.


    int width() const { return m_Width; }
    int height() const { return m_Height; }

    std::uint32_t* pixels() { return m_Pixels; }
    std::uint32_t* pixels() const { return m_Pixels; }

    std::uint32_t& pixel(int x, int y) const { return m_Pixels[y*m_Width+x]; }

    // pixel channel
    char* pixel_(int x, int y) const { return (char*)&pixel(x,y); }


    void fill(std::uint32_t rgba);

    void fillAlpha(float a);

    // for OpenGL internal, especially for Screenshot Dump.
    void flipY(std::uint32_t* dst) const;
};

#endif //ETHERTIA_BITMAPIMAGE_H
