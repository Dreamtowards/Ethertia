//
// Created by Dreamtowards on 2022/4/28.
//

#ifndef ETHERTIA_FONTRENDERER_H
#define ETHERTIA_FONTRENDERER_H

#include <ethertia/client/gui/Gui.h>
#include <ethertia/client/render/shader/ShaderProgram.h>
#include <ethertia/client/render/renderer/gui/GuiRenderer.h>
#include <ethertia/client/render/renderer/Renderer.h>
#include <ethertia/client/Loader.h>
#include <ethertia/util/Mth.h>

class FontRenderer
{
    ShaderProgram shader{Loader::loadAssetsStr("shaders/gui/font.vsh"),
                         Loader::loadAssetsStr("shaders/gui/font.fsh")};

    Texture* texAscii = nullptr;

    float glyphWidths[256];

    const char** UNIFORM_CHARS = Renderer::_GenArrayNames("chars[%i]", 128);
    const char** UNIFORM_OFFSET = Renderer::_GenArrayNames("offset[%i]", 128);
    const char** UNIFORM_SCALE = Renderer::_GenArrayNames("scale[%i]", 128);

    const char** UNIFORM_GLYPH_WIDTHS = Renderer::_GenArrayNames("glyphWidths[%i]", 256);
public:
    FontRenderer() {

        {
            auto m = Loader::loadAssets("font/unicode_page_0.png");
            texAscii = Loader::loadTexture(Loader::loadPNG(m.first, m.second));
            delete m.first;
        }

        {
            auto m = Loader::loadAssets("font/glyph_widths.bin");
            ubyte* wglyph = (ubyte*)m.first;
            for (int i = 0; i < 256; ++i) {
                glyphWidths[i] = (float)wglyph[i] / 255.0f;
            }
            delete m.first;
        }

        shader.useProgram();
        for (int i = 0; i < 256; ++i) {
            shader.setFloat(UNIFORM_GLYPH_WIDTHS[i], glyphWidths[i]);
        }
    }

    void renderString(float x, float y, const std::string& str, glm::vec4 color, float fontHeight)
    {
        uint len = str.length();
        float ww = Gui::maxWidth();
        float wh = Gui::maxHeight();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texAscii->getTextureID());

        shader.useProgram();

        for (int i = 0; i < len; ++i) {
            ubyte ch = str.at(i);
            shader.setInt(UNIFORM_CHARS[i], ch);
            shader.setVector2f(UNIFORM_OFFSET[i], Mth::ndc(x,y,ww,wh));
            x += fontHeight*glyphWidths[ch];
        }
        shader.setVector2f("scale", glm::vec2(fontHeight/ww, fontHeight/wh));
        shader.setVector4f("color", color);

        glBindVertexArray(GuiRenderer::M_RECT->vaoId);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, GuiRenderer::M_RECT->vertexCount, len);
    }
};

#endif //ETHERTIA_FONTRENDERER_H
