//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_BITMAPIMAGE_H
#define ETHERTIA_BITMAPIMAGE_H

#include <ethertia/util/UnifiedTypes.h>

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
        resize(w, h, new u32[w*h]);
    }

    void resize(int w, int h, std::uint32_t* pxs) {
        width = w;
        height = h;
        pixels = pxs;
    }

    std::uint32_t getPixel(int x, int y) {
        return pixels[y*width+x];
    }
    void setPixel(int x, int y, std::uint32_t rgba) {
        pixels[y*width+x] = rgba;
    }
    void setPixels(int x, int y, BitmapImage* img) {
        // todo check size.
        for (int dx = 0; dx < img->getWidth(); ++dx) {
            for (int dy = 0; dy < img->getHeight(); ++dy) {
                setPixel(x+dx, y+dy, img->getPixel(dx, dy));
            }
        }
    }
    std::uint32_t* getPixels() {
        return pixels;
    }
    void getVerticalFlippedPixels(std::uint32_t* dst) {  // for OpenGL internal
        for (int y = 0; y < height; ++y) {
            u32 bas = (height-1-y) * width;
            for (int x = 0; x < width; ++x) {
                dst[y*width+x] = pixels[bas+x];
            }
        }
    }

    int getWidth()  const { return width; }
    int getHeight() const { return height; }

    ~BitmapImage() {
        delete pixels;
    }
};

#endif //ETHERTIA_BITMAPIMAGE_H
