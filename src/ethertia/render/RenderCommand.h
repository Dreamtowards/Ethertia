//
// Created by Dreamtowards on 2023/3/13.
//

#ifndef ETHERTIA_RENDERCOMMAND_H
#define ETHERTIA_RENDERCOMMAND_H

#include <glm/vec4.hpp>

#include "Texture.h"
#include "Framebuffer.h"
#include "VertexArrays.h"

struct RenderCommand
{
    // the DrawCalls always async. just submit render-commands.

    static void Clear(glm::vec4 clearcolor = {0,0,0,1});

    static void CheckError(std::string_view phase);

    // -- Draw ------------------------------

    static void DrawArrays(VertexArrays* vao);
    // static void DrawIndexed();

    // Screen Quad. kinda high-level. useful for Post-processing. e.g. SSAO etc.
    // Built-in VAO is [vec2 PosNDC, vec2 TexCoord]
    static void DrawFullQuad();

};

#endif //ETHERTIA_RENDERCOMMAND_H
