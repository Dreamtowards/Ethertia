//
// Created by Dreamtowards on 2023/2/14.
//

#ifndef ETHERTIA_SHADOWRENDERER_H
#define ETHERTIA_SHADOWRENDERER_H

#include <ethertia/render/shader/ShaderProgram.h>

class ShadowRenderer
{
public:

    ShaderProgram shaderShadow = Loader::loadShaderProgram("shaders/shadow/shadow.{}");

    glm::mat4 matShadowSpace;

    Framebuffer* fboDepthMap = nullptr;

    ShadowRenderer()
    {

        fboDepthMap = Framebuffer::glfGenFramebuffer(1280, 720);


    }

    void renderDepthMap()
    {

        shaderShadow.useProgram();



    }

};

#endif //ETHERTIA_SHADOWRENDERER_H
