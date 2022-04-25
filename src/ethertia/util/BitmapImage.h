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

    uint getWidth() { return width; }
    uint getHeight() { return height; }

    ~BitmapImage() {
        delete pixels;
    }
};

#endif //ETHERTIA_BITMAPIMAGE_H
