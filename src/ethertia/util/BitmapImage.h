//
// Created by Dreamtowards on 2022/4/25.
//

#pragma once

#include <stdint.h>


class BitmapImage
{
    uint32_t* m_Pixels;  // pixel: u32 RGBA, channel=u8
    int m_Width;
    int m_Height;

public:
    BitmapImage(int w, int h);
    BitmapImage(int w, int h, void* pxs);  // pixels in. e.g. quick load from stbi etc.
    ~BitmapImage();

    BitmapImage(const BitmapImage& cpy) = delete;
    BitmapImage(const BitmapImage&& cpy) = delete;


    static void Resize(
        const BitmapImage& src,
        BitmapImage& dst,
        bool nearestSample = false);  // true=NearestSample, false=STBIR_DEFAULT_FILTER_DOWNSAMPLE(STBIR_FILTER_MITCHELL) STBIR_DEFAULT_FILTER_UPSAMPLE(STBIR_FILTER_CATMULLROM)


    static void CopyPixels(
        int srcX, int srcY, const BitmapImage& srcImg,
        int dstX, int dstY, BitmapImage& dstImg,
        int cpyWidth = -1, int cpyHeight = -1,  // -1: Full srcImg size. width and height.
        int specChannel = -1);  // -1: u32 RGBA, 0/1/2/3: one of u8 R/G/B/A.


    int width() const;
    int height() const;

    uint32_t* pixels();
    uint32_t* pixels() const;

    uint32_t& pixel(int x, int y) const;

    // pixel_channels(x, y)[0-3 for RGBA]
    char* pixel_channels(int x, int y) const;


    void fill(uint32_t rgba);

    void fillAlpha(float a);

    // for OpenGL internal, especially for Screenshot Dump.
    void flipY(uint32_t* dst) const;
};

