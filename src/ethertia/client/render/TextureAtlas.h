//
// Created by Dreamtowards on 2022/4/25.
//

#ifndef ETHERTIA_TEXTUREATLAS_H
#define ETHERTIA_TEXTUREATLAS_H

#include <vector>
#include <glm/vec2.hpp>
#include <ethertia/client/Loader.h>
#include <ethertia/client/render/Texture.h>
#include <ethertia/util/BitmapImage.h>
#include <ethertia/util/Mth.h>


class TextureAtlas
{
    class AtlasFragment {
        BitmapImage* image;
        glm::vec2 offset;
        glm::vec2 scale;

        ~AtlasFragment() {
            delete image;
        }
    };

    std::vector<AtlasFragment*> atlas;
    Texture* atlasTexture;

    ~TextureAtlas() {
        for (auto* frag : atlas) {
            delete frag;
        }
        delete atlasTexture;
    }

public:
    AtlasFragment* addAtlas(BitmapImage* image)
    {
        AtlasFragment* frag = new AtlasFragment();
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
        for (AtlasFragment* frag : atlas) {
            atlasImage.setPixels(dx, 0, frag->image);
            dx += frag->image->getWidth();
        }

        atlasTexture = Loader::loadTexture(atlasImage);
    }

};

#endif //ETHERTIA_TEXTUREATLAS_H
