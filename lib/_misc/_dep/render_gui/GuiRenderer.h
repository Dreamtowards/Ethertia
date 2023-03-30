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
    DECL_SHADER(SHADER, "shaders/gui/gui.{}");

public:
    inline static VertexArrays* M_RECT_RB;  // RB,RT,LB,LT. TRIANGLE_STRIP.

    static void init()
    {
        // init M_RECT.
        float RECT_POS[] = {2,-2, 2, 0, 0,-2, 0, 0};
        float RECT_UV[]  = {1, 0, 1, 1, 0, 0, 0, 1};
        M_RECT_RB = Loader::loadModel(4, {
                {2, RECT_POS},
                {2, RECT_UV}
        });


        // todo: Align(anchor), in_pos -= anchor * 2.0;
        //
        //  Rotation angle, -w -h flip draw.
    }

    static void render(float x, float y, float w, float h,
                       glm::vec4 color, Texture* tex =nullptr, glm::vec4 texPosSize = {0,0,1,1},
                       float round =0, float border =0, int chnMode =0)
    {
        if (!tex)
            tex = Texture::WHITE;
        if (border == 0)
            border = 999999;

        tex->BindTexture(0);

        float ww = Gui::maxWidth();  // window width.
        float wh = Gui::maxHeight();

        SHADER->useProgram();

        SHADER->setVector4f("vertPosSize",
                           glm::vec4(Mth::ndc(x, y, ww, wh),
                           glm::vec2(w/ww, h/wh)));

        SHADER->setVector2f("sizePixel", glm::vec2(w, h));
        SHADER->setFloat("border", border);
        SHADER->setFloat("round", round);

        SHADER->setVector4f("color", color);

        SHADER->setInt("channelMode", chnMode);

        SHADER->setVector4f("texPosSize", texPosSize);

        glBindVertexArray(M_RECT_RB->vaoId);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

};

#endif //ETHERTIA_GUIRENDERER_H
