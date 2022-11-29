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
        BitmapImage* image = nullptr;
        glm::vec2 pos;
        glm::vec2 size;

        ~Region() {
            delete image;
        }
    };

    std::vector<Region*> atlas;
    Texture* atlasTexture = nullptr;

    ~TextureAtlas() {
        for (Region* r : atlas) {
            delete r;
        }
        delete atlasTexture;
    }

public:
    // add before bake/buildAtlas
    Region* addAtlas(BitmapImage* image)
    {
        assert(atlasTexture == nullptr);  // can add only when Atlas unbaked or invalidated.
        Region* r = new Region();
        r->image = image;

        atlas.push_back(r);
        return r;
    }

    Region* addBakedAtlas(glm::vec2 _pos, glm::vec2 _size)
    {
        Region* r = new Region();
        r->pos  = _pos;
        r->size = _size;

        atlas.push_back(r);
        return r;
    }

    void buildAtlas() {
        assert(atlasTexture == nullptr);

        int totalWidth = 0;
        int maxHeight = 0;
        for (auto* frag : atlas) {
            totalWidth += frag->image->getWidth();
            maxHeight = Mth::max(maxHeight, frag->image->getHeight());
        }

        BitmapImage* atlasImage = new BitmapImage(totalWidth, maxHeight);

        int dx = 0;
        for (Region* r : atlas) {
            BitmapImage* img = r->image;
            int wid = img->getWidth();
            int hei = img->getHeight();
            atlasImage->setPixels(dx, maxHeight - hei, img);

            r->pos  = glm::vec2((float)dx  / totalWidth, 0);
            r->size = glm::vec2((float)wid / totalWidth, (float)hei / maxHeight);

            dx += wid;
        }

        atlasTexture = Loader::loadTexture(atlasImage);
        delete atlasImage;
    }

};

#endif //ETHERTIA_TEXTUREATLAS_H
