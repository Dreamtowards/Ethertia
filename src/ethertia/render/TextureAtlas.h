//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_TEXTUREATLAS_H
#define ETHERTIA_TEXTUREATLAS_H

#include <vector>
#include <glm/vec2.hpp>

#include <ethertia/render/Texture.h>
#include <ethertia/util/Loader.h>
#include <ethertia/util/BitmapImage.h>
#include <ethertia/util/Mth.h>


class TextureAtlas
{
public:
    class Region {
    public:
        BitmapImage* image;
        glm::vec2 pos;
        glm::vec2 size;

        ~Region() {
            delete image;
        }
    };

    std::vector<Region*> atlas;
    Texture* atlasTexture;

    ~TextureAtlas() {
        for (auto* r : atlas) {
            delete r;
        }
        delete atlasTexture;
    }

public:
    Region* addAtlas(BitmapImage* image)
    {
        Region* frag = new Region();
        frag->image = image;

        atlas.push_back(frag);
        return frag;
    }

    void buildAtlas() {
        uint totalWidth = 0;
        uint maxHeight = 0;
        for (auto* frag : atlas) {
            totalWidth += frag->image->getWidth();
            maxHeight = Mth::max(maxHeight, frag->image->getHeight());
        }

        BitmapImage atlasImage(totalWidth, maxHeight);

        uint dx = 0;
        for (Region* r : atlas) {
            BitmapImage* img = r->image;
            float wid = img->getWidth();
            float hei = img->getHeight();
            atlasImage.setPixels(dx, maxHeight - hei, img);

            r->pos = glm::vec2((float)dx / totalWidth, 0);
            r->size = glm::vec2(wid / totalWidth, hei / maxHeight);

            dx += wid;
        }

        atlasTexture = Loader::loadTexture(&atlasImage);
    }

};

#endif //ETHERTIA_TEXTUREATLAS_H
