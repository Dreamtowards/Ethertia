//
// Created by Dreamtowards on 2022/4/28.
//

#ifndef ETHERTIA_GUIRENDERER_H
#define ETHERTIA_GUIRENDERER_H

#include <ethertia/gui/Gui.h>
#include <ethertia/render/shader/ShaderProgram.h>
#include <ethertia/util/Loader.h>

class GuiRenderer
{
    ShaderProgram shader{Loader::loadAssetsStr("shaders/gui/gui.vsh"),
                         Loader::loadAssetsStr("shaders/gui/gui.fsh")};

public:
    inline static Model* M_RECT;  // RB,RT,LB,LT. TRIANGLE_STRIP.

    inline static Texture* TEX_WHITE;  // 1x1 pixel, RGBA=1 white tex.

    GuiRenderer()
    {
        // init M_RECT.
        float RECT_POS[] = {2,-2, 2, 0, 0,-2, 0, 0};
        float RECT_UV[]  = {1, 0, 1, 1, 0, 0, 0, 1};
        M_RECT = Loader::loadModel(4, {
                {2, RECT_POS},
                {2, RECT_UV}
        });

        BitmapImage img(1, 1, new u32[1]{(u32)~0});
        TEX_WHITE = Loader::loadTexture(&img);
    }

    void render(float x, float y, float w, float h, glm::vec4 color, Texture* tex =nullptr, float round =0, float border =FLT_MAX, int chnMode =0)
    {
        if (!tex)
            tex = TEX_WHITE;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex->getTextureID());

        float ww = Gui::maxWidth();  // window width.
        float wh = Gui::maxHeight();

        shader.useProgram();

        shader.setVector2f("offset", Mth::ndc(x, y, ww, wh));
        shader.setVector2f("scale", glm::vec2(w/ww, h/wh));

        shader.setVector2f("pixel_size", glm::vec2(w, h));
        shader.setFloat("border", border);
        shader.setFloat("round", round);

        shader.setVector4f("color", color);

        shader.setInt("channelMode", chnMode);

        glBindVertexArray(M_RECT->vaoId);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

};

#endif //ETHERTIA_GUIRENDERER_H
