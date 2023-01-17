//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_BITMAPIMAGE_H
#define ETHERTIA_BITMAPIMAGE_H

#include <ethertia/util/Colors.h>

#include <stb/stb_image_resize.h>

class BitmapImage
{
    std::uint32_t* pixels;
    int width;
    int height;

public:
    BitmapImage(int w, int h, std::uint32_t* pxs) {
        resize(w, h, pxs);
    }
    BitmapImage(int w, int h) {
        resize(w, h, new uint32_t[w*h]);
    }
    ~BitmapImage() {
        delete pixels;
    }

    BitmapImage(const BitmapImage& cpy) = delete;
    BitmapImage(const BitmapImage&& cpy) = delete;

    void resize(int w, int h, std::uint32_t* pxs) {
        width = w;
        height = h;
        pixels = pxs;
    }

    static void resizeTo(const BitmapImage& src, BitmapImage& dst)
    {
        int succ =
        stbir_resize_uint8((const unsigned char*)src.pixels, src.getWidth(), src.getHeight(), 0,
                           (      unsigned char*)dst.pixels, dst.getWidth(), dst.getHeight(), 0, 4);
        assert(succ);
    }

    const char* pixel_rgba(int x, int y) const {
        return (const char*)&pixels[y*width+x];
    }
    char* pixel_rgba_intl(int x, int y) {
        return (char*)&pixels[y*width+x];
    }

    std::uint32_t getPixel(int x, int y) const {
        return pixels[y*width+x];
    }
    void setPixel(int x, int y, std::uint32_t rgba) {
        pixels[y*width+x] = rgba;
    }

    // spec_channel: only one channel: -1: no. 0123 -> RGBA.
    void setPixels(int x, int y, const BitmapImage& img, int spec_channel = -1) {
        assert(x+img.getWidth() <= getWidth() && y+img.getHeight() <= getHeight());
        for (int dx = 0; dx < img.getWidth(); ++dx) {
            for (int dy = 0; dy < img.getHeight(); ++dy) {
                if (spec_channel != -1) {
                    char* pix = pixel_rgba_intl(x+dx, y+dy);
                    pix[spec_channel] = img.pixel_rgba(dx,dy)[spec_channel];
                } else {
                    setPixel(x+dx, y+dy, img.getPixel(dx, dy));
                }
            }
        }
    }
    void fillPixels(std::uint32_t rgba) {
        std::fill(pixels, pixels + width*height, rgba);
    }
    void fillAlpha(float a) {
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                char* pix = pixel_rgba_intl(x,y);

                pix[3] = (char)(a * 255.0f);
            }
        }
    }

    std::uint32_t* getPixelsInternal() {
        return pixels;
    }
    std::uint32_t* getPixels() const {
        return pixels;
    }
    void getVerticalFlippedPixels(std::uint32_t* dst) const {  // for OpenGL internal
        for (int y = 0; y < height; ++y) {
            uint32_t bas = (height-1-y) * width;
            for (int x = 0; x < width; ++x) {
                dst[y*width+x] = pixels[bas+x];
            }
        }
    }

    int getWidth()  const { return width; }
    int getHeight() const { return height; }

};

#endif //ETHERTIA_BITMAPIMAGE_H
