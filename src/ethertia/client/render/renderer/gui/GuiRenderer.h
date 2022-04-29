//
// Created by Dreamtowards on 2022/4/28.
//

#ifndef ETHERTIA_GUIRENDERER_H
#define ETHERTIA_GUIRENDERER_H

#include <ethertia/client/gui/Gui.h>
#include <ethertia/client/render/shader/ShaderProgram.h>
#include <ethertia/client/Loader.h>

class GuiRenderer
{
    ShaderProgram shader{Loader::loadAssetsStr("shaders/gui/gui.vsh"),
                         Loader::loadAssetsStr("shaders/gui/gui.fsh")};

public:
    inline static Model* M_RECT;  // RB,RT,LB,LT. TRIANGLE_STRIP.

    GuiRenderer()
    {
        // init M_RECT.
        float RECT_POS[] = {2,-2, 2, 0, 0,-2, 0, 0};
        float RECT_UV[]  = {1, 0, 1, 1, 0, 0, 0, 1};
        M_RECT = Loader::loadModel(4, {
                {2, RECT_POS},
                {2, RECT_UV}
        });
    }

    void render(float x, float y, float w, float h, glm::vec4 color, Texture* tex)
    {
        if (tex) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex->getTextureID());
        }
        float ww = Gui::maxWidth();  // window width.
        float wh = Gui::maxHeight();

        shader.useProgram();

        shader.setVector2f("offset", Mth::ndc(x, y, ww, wh));
        shader.setVector2f("scale", glm::vec2(w/ww, h/wh));

        shader.setVector4f("color", color);

        glBindVertexArray(M_RECT->vaoId);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
    }

};

#endif //ETHERTIA_GUIRENDERER_H
