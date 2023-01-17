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

    BitmapImage(const BitmapImage& cpy) {
        assert(false && "Implicit copy is disabled due to big consume.");
    }

    void resize(int w, int h, std::uint32_t* pxs) {
        width = w;
        height = h;
        pixels = pxs;
    }

    static void resizeTo(const BitmapImage& src, BitmapImage& dst)
    {
        stbir_resize_uint8((const unsigned char*)src.pixels, src.getWidth(), src.getHeight(), 0,
                           (      unsigned char*)dst.pixels, dst.getWidth(), dst.getHeight(), 0, 4);
    }

    std::uint32_t getPixel(int x, int y) const {
        return pixels[y*width+x];
    }
    void setPixel(int x, int y, std::uint32_t rgba) {
        pixels[y*width+x] = rgba;
    }
    void setPixels(int x, int y, const BitmapImage& img) {
        // todo check size.
        for (int dx = 0; dx < img.getWidth(); ++dx) {
            for (int dy = 0; dy < img.getHeight(); ++dy) {
                setPixel(x+dx, y+dy, img.getPixel(dx, dy));
            }
        }
    }
    void fillPixels(std::uint32_t rgba) {
        std::fill(pixels, pixels + width*height, rgba);
    }
    void fillAlpha(float a) {
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                char* pix = (char*)&pixels[y*width+x];

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
