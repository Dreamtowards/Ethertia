//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_BITMAPIMAGE_H
#define ETHERTIA_BITMAPIMAGE_H

class BitmapImage
{
    uint* pixels;
    uint width;
    uint height;

public:
    BitmapImage(uint w, uint h, uint* pxs) {
        resize(w, h, pxs);
    }
    BitmapImage(uint w, uint h) {
        resize(w, h, new uint[w*h]);
    }

    void resize(uint w, uint h, uint* pxs) {
        width = w;
        height = h;
        pixels = pxs;
    }

    uint getPixel(uint x, uint y) {
        return pixels[y*width+x];
    }
    void setPixel(uint x, uint y, uint rgba) {
        pixels[y*width+x] = rgba;
    }
    void setPixels(uint x, uint y, BitmapImage* img) {
        // todo check size.
        for (int dx = 0; dx < img->getWidth(); ++dx) {
            for (int dy = 0; dy < img->getHeight(); ++dy) {
                setPixel(x+dx, y+dy, img->getPixel(dx, dy));
            }
        }
    }
    uint* getPixels() {
        return pixels;
    }
    void getVerticalFlippedPixels(uint* dst) {  // for OpenGL internal
        for (int y = 0; y < height; ++y) {
            uint bas = (height-1-y) * width;
            for (int x = 0; x < width; ++x) {
                dst[y*width+x] = pixels[bas+x];
            }
        }
    }

    uint getWidth() { return width; }
    uint getHeight() { return height; }

    ~BitmapImage() {
        delete pixels;
    }
};

#endif //ETHERTIA_BITMAPIMAGE_H
