//
// Created by Dreamtowards on 2023/5/23.
//

#include "BitmapImage.h"


BitmapImage::BitmapImage(int w, int h)
{
    m_Width = w;
    m_Height = h;
    m_Pixels = new uint32_t[w*h];
}

BitmapImage::BitmapImage(int w, int h, void* pxs)
{
    m_Width = w;
    m_Height = h;
    m_Pixels = (uint32_t*)pxs;
}

BitmapImage::~BitmapImage()
{
    delete m_Pixels;
}



void BitmapImage::resize(const BitmapImage &src, BitmapImage &dst, bool nearestSample)
{
    assert(nearestSample == false);
    int succ =
            stbir_resize_uint8((const unsigned char*)src.m_Pixels, src.width(), src.height(), 0,
                               (      unsigned char*)dst.m_Pixels, dst.width(), dst.height(), 0, 4);
    assert(succ);
}


void BitmapImage::CopyPixels(int srcX, int srcY, const BitmapImage& srcImg,
                             int dstX, int dstY, BitmapImage& dstImg,
                             int specChannel)
{
    assert(srcX <= srcImg.width() && srcY <= srcImg.height() && "Out of copy src image sample range");
    assert(dstX+srcImg.width() <= dstImg.width() && dstY+srcImg.height() <= dstImg.height() && "Out of copy dest range");

    for (int dx = 0; dx < srcImg.width(); ++dx)
    {
        for (int dy = 0; dy < srcImg.height(); ++dy)
        {
            if (specChannel != -1) // copy only the channel.
            {
                dstImg.pixel_(dstX+dx, dstY+dy)[specChannel] =
                srcImg.pixel_(srcX+dx, srcY+dy)[specChannel];
            }
            else
            {
                dstImg.pixel(dstX+dx, dstY+dy) =
                srcImg.pixel(srcX+dx, srcY+dy);
            }
        }
    }
}



void BitmapImage::fill(std::uint32_t rgba)
{
    std::fill(m_Pixels, m_Pixels + m_Width*m_Height, rgba);
}

void BitmapImage::fillAlpha(float a)
{
    assert(a >= 0.0f && a <= 1.0f);

    for (int x = 0; x < m_Width; ++x) {
        for (int y = 0; y < m_Height; ++y) {
            pixel_(x,y)[3] = (char)(a * 255.0f);
        }
    }
}