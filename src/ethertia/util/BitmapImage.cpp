//
// Created by Dreamtowards on 2023/5/23.
//

#include "BitmapImage.h"

#include <assert.h>
#include <algorithm>

#include <stb_image_resize.h>



BitmapImage::BitmapImage(int w, int h)
{
    m_Width = w;
    m_Height = h;
    m_Pixels = new uint32_t[w * h];
}

BitmapImage::BitmapImage(int w, int h, void* pxs)
{
    m_Width = w;
    m_Height = h;
    m_Pixels = (uint32_t*)pxs;
}

BitmapImage::~BitmapImage()
{
    free(m_Pixels);
}



void BitmapImage::Resize(const BitmapImage& src, BitmapImage& dst, bool nearestSample)
{
    assert(nearestSample == false);
    int succ = stbir_resize_uint8((const unsigned char*)src.m_Pixels, src.width(), src.height(), 0,
                                        (unsigned char*)dst.m_Pixels, dst.width(), dst.height(), 0, 4);
    assert(succ);
}


void BitmapImage::CopyPixels(
    int srcX, int srcY, const BitmapImage& srcImg,
    int dstX, int dstY, BitmapImage& dstImg,
    int cpyWidth, int cpyHeight,
    int specChannel)
{
    if (cpyWidth == -1) cpyWidth = srcImg.width();
    if (cpyHeight == -1) cpyHeight = srcImg.height();
    assert(srcX + cpyWidth <= srcImg.width() && srcY + cpyHeight <= srcImg.height() && "Out of copy src image sample range");
    assert(dstX + cpyWidth <= dstImg.width() && dstY + cpyHeight <= dstImg.height() && "Out of copy dest range");

    for (int dx = 0; dx < cpyWidth; ++dx)
    {
        for (int dy = 0; dy < cpyHeight; ++dy)
        {
            if (specChannel != -1) // copy only the channel.
            {
                dstImg.pixel_channels(dstX + dx, dstY + dy)[specChannel] =
                    srcImg.pixel_channels(srcX + dx, srcY + dy)[specChannel];
            }
            else
            {
                dstImg.pixel(dstX + dx, dstY + dy) =
                    srcImg.pixel(srcX + dx, srcY + dy);
            }
        }
    }
}


int BitmapImage::width() const {
    return m_Width;
}
int BitmapImage::height() const {
    return m_Height;
}


uint32_t* BitmapImage::pixels() {
    return m_Pixels;
}
uint32_t* BitmapImage::pixels() const {
    return m_Pixels;
}

uint32_t& BitmapImage::pixel(int x, int y) const {
    return m_Pixels[y * m_Width + x];
}

char* BitmapImage::pixel_channels(int x, int y) const {
    return (char*)&pixel(x, y);
}

void BitmapImage::fill(uint32_t rgba)
{
    std::fill(m_Pixels, m_Pixels + m_Width * m_Height, rgba);
}

void BitmapImage::fillAlpha(float a)
{
    assert(a >= 0.0f && a <= 1.0f);

    for (int x = 0; x < m_Width; ++x) {
        for (int y = 0; y < m_Height; ++y) {
            pixel_channels(x, y)[3] = (char)(a * 255.0f);
        }
    }
}

void BitmapImage::flipY(uint32_t* dst) const
{
    for (int y = 0; y < m_Height; ++y) {
        uint32_t bas = (m_Height - 1 - y) * m_Width;
        for (int x = 0; x < m_Width; ++x) {
            dst[y * m_Width + x] = m_Pixels[bas + x];
        }
    }
}