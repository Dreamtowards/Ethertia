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
    static constexpr float GAP_CHAR_PERC = 0.02;
    static constexpr float GAP_LINE_PERC = 0.02;
    static constexpr uint BATCH_CHARS_LIMIT = 128;  // actually is size. ie. max+1
    static constexpr uint GLYPH_LIMIT = 256;

    ShaderProgram shader{Loader::loadAssetsStr("shaders/gui/font.vsh"),
                         Loader::loadAssetsStr("shaders/gui/font.fsh")};

    Texture* texAscii = nullptr;

    float glyphWidths[GLYPH_LIMIT];
    const char** UNIFORM_GLYPH_WIDTHS = Renderer::_GenArrayNames("glyphWidths[%i]", GLYPH_LIMIT);

    const char** UNIFORM_CHARS = Renderer::_GenArrayNames("chars[%i]", BATCH_CHARS_LIMIT);
    const char** UNIFORM_OFFSET = Renderer::_GenArrayNames("offset[%i]", BATCH_CHARS_LIMIT);
    // const char** UNIFORM_SCALE = Renderer::_GenArrayNames("scale[%i]", 128);
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
            for (int i = 0; i < GLYPH_LIMIT; ++i) {
                glyphWidths[i] = (float)wglyph[i] / 255.0f;
            }
            delete m.first;
        }

        shader.useProgram();
        for (int i = 0; i < GLYPH_LIMIT; ++i) {
            shader.setFloat(UNIFORM_GLYPH_WIDTHS[i], glyphWidths[i]);
        }
    }

    void renderString(float x, float y, const std::string& str, glm::vec4 color, float textHeight, float align)
    {
        uint len = str.length();
        float ww = Gui::maxWidth();
        float wh = Gui::maxHeight();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texAscii->getTextureID());

        shader.useProgram();

        shader.setVector2f("scale", glm::vec2(textHeight / ww, textHeight / wh));
        shader.setVector4f("color", color);

        float chX = x - lineWidth(str, 0, textHeight) * align;
        float chY = y;
        uint chIdx = 0;
        for (int strIdx = 0; strIdx < str.length(); ++strIdx) {  // index-access is not necessary. str.index != rendering-char-idx. we skip "\n" chars.
            uint ch = str.at(strIdx);
            if (ch == '\n') {
                chX = x - lineWidth(str, strIdx+1, textHeight) * align;
                chY += textHeight * (1.0f+GAP_LINE_PERC);
            } else {
                shader.setInt(UNIFORM_CHARS[chIdx], ch);
                shader.setVector2f(UNIFORM_OFFSET[chIdx], Mth::ndc(chX,chY,ww,wh));
                chX += charFullWidth(ch, textHeight);
                ++chIdx;
            }
            // Render. if Reach Batched-Rendering-Limit or just End of String.
            if (chIdx == BATCH_CHARS_LIMIT || strIdx == str.length()-1) {
                chIdx = 0;
                glBindVertexArray(GuiRenderer::M_RECT->vaoId);
                glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, GuiRenderer::M_RECT->vertexCount, len);
            }
        }
    }

    float charFullWidth(uint ch, float textHeight) {
        return glyphWidths[ch]*textHeight * (1.0f+GAP_CHAR_PERC);
    }

    float lineWidth(const std::string& str, uint from, float textHeight) {
        float w = 0;
        for (int i = from; i < str.length(); ++i) {
            uint ch = str.at(i);
            if (ch == '\n')
                break;
            w += charFullWidth(ch, textHeight);
        }
        return w;
    }

    glm::vec2 calculateTextBound(const std::string& str, float textHeight) {
        float lineHeight = textHeight * (1.0f+GAP_LINE_PERC);
        float maxX = 0;
        float x = 0;
        float y = 0;
        for (uint ch : str) {
            if (ch == '\n') {
                maxX = Mth::max(maxX, x);
                x = 0;
                y += lineHeight;
            } else {
                x += charFullWidth(ch, textHeight);
            }
        }
        return glm::vec2(maxX, y+lineHeight);
    }
};

#endif //ETHERTIA_FONTRENDERER_H