//
// Created by Dreamtowards on 2022/4/28.
//

#ifndef ETHERTIA_GUIRENDERER_H
#define ETHERTIA_GUIRENDERER_H

#include <ethertia/gui/Gui.h>
#include <ethertia/render/ShaderProgram.h>
#include <ethertia/util/Loader.h>

class GuiRenderer
{
    ShaderProgram shader = Loader::loadShaderProgram("shaders/gui/gui.{}");

public:
    inline static Model* M_RECT_RB;  // RB,RT,LB,LT. TRIANGLE_STRIP.

    inline static Texture* TEX_WHITE;  // 1x1 pixel, RGBA=1 white tex.

    GuiRenderer()
    {
        // init M_RECT.
        float RECT_POS[] = {2,-2, 2, 0, 0,-2, 0, 0};
        float RECT_UV[]  = {1, 0, 1, 1, 0, 0, 0, 1};
        M_RECT_RB = Loader::loadModel(4, {
                {2, RECT_POS},
                {2, RECT_UV}
        });

        BitmapImage img(1, 1, new u32[1]{(u32)~0});
        TEX_WHITE = Loader::loadTexture(img);

        // todo: Align(anchor), in_pos -= anchor * 2.0;
        //
        //  Rotation angle, -w -h flip draw.
    }

    void render(float x, float y, float w, float h, glm::vec4 color, Texture* tex =nullptr, glm::vec4 texPosSize = {0,0,1,1}, float round =0, float border =0, int chnMode =0)
    {
        if (!tex)
            tex = TEX_WHITE;
        if (border == 0)
            border = 999999;

        tex->bindTexture2D(0);

        float ww = Gui::maxWidth();  // window width.
        float wh = Gui::maxHeight();

        shader.useProgram();

        shader.setVector4f("vertPosSize",
                           glm::vec4(Mth::ndc(x, y, ww, wh),
                           glm::vec2(w/ww, h/wh)));

        shader.setVector2f("sizePixel", glm::vec2(w, h));
        shader.setFloat("border", border);
        shader.setFloat("round", round);

        shader.setVector4f("color", color);

        shader.setInt("channelMode", chnMode);

        shader.setVector4f("texPosSize", texPosSize);

        glBindVertexArray(M_RECT_RB->vaoId);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

};

#endif //ETHERTIA_GUIRENDERER_H
