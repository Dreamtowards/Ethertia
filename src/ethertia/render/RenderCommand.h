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

    static void BindTexture2D(Texture* tex, int slot = 0);
    static BitmapImage* GetTexImage(Texture* tex);

};

#endif //ETHERTIA_RENDERCOMMAND_H
