//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_BITMAPIMAGE_H
#define ETHERTIA_BITMAPIMAGE_H

#include <ethertia/util/UnifiedTypes.h>

class BitmapImage
{
    u32* pixels;
    u32 width;
    u32 height;

public:
    BitmapImage(u32 w, u32 h, u32* pxs) {
        resize(w, h, pxs);
    }
    BitmapImage(u32 w, u32 h) {
        resize(w, h, new u32[w*h]);
    }

    void resize(u32 w, u32 h, u32* pxs) {
        width = w;
        height = h;
        pixels = pxs;
    }

    u32 getPixel(u32 x, u32 y) {
        return pixels[y*width+x];
    }
    void setPixel(u32 x, u32 y, u32 rgba) {
        pixels[y*width+x] = rgba;
    }
    void setPixels(u32 x, u32 y, BitmapImage* img) {
        // todo check size.
        for (int dx = 0; dx < img->getWidth(); ++dx) {
            for (int dy = 0; dy < img->getHeight(); ++dy) {
                setPixel(x+dx, y+dy, img->getPixel(dx, dy));
            }
        }
    }
    u32* getPixels() {
        return pixels;
    }
    void getVerticalFlippedPixels(u32* dst) {  // for OpenGL internal
        for (int y = 0; y < height; ++y) {
            u32 bas = (height-1-y) * width;
            for (int x = 0; x < width; ++x) {
                dst[y*width+x] = pixels[bas+x];
            }
        }
    }

    u32 getWidth() { return width; }
    u32 getHeight() { return height; }

    ~BitmapImage() {
        delete pixels;
    }
};

#endif //ETHERTIA_BITMAPIMAGE_H
