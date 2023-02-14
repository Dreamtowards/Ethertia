//
// Created by Dreamtowards on 2023/2/14.
//

#ifndef ETHERTIA_SHADOWRENDERER_H
#define ETHERTIA_SHADOWRENDERER_H

#include <glm/gtc/matrix_transform.hpp>

#include <ethertia/render/ShaderProgram.h>

class ShadowRenderer
{
public:

    inline static ShaderProgram shaderShadow;

    inline static glm::mat4 matShadowSpace;

    inline static Framebuffer* fboDepthMap = nullptr;

    static void init()
    {
        fboDepthMap = Framebuffer::GenFramebuffer(1280, 720, [](Framebuffer* fbo)
        {
            // GL_LINEAR?
            fbo->attachDepthTexture();
            fbo->disableDrawBuffers();  // not render any Color Data. just depthMap.
        });

        shaderShadow = Loader::loadShaderProgram("shaders/shadow/shadow.{}");

    }

    static void renderDepthMap(const std::vector<Entity*>& entities)
    {
        assert(glIsEnabled(GL_DEPTH_TEST));

        auto _ap = fboDepthMap->bindFramebuffer_ap();

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


        glm::vec3 shadowDir = glm::normalize(glm::vec3{-1,-1,-1});
        glm::vec3 shadowPos = RenderEngine::g_Camera.position;
        glm::mat4 matView = glm::lookAt(shadowPos, shadowPos+shadowDir, glm::vec3{0,1,0});

        glm::mat4 matProj = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f,
                                       -1000.0f, 1000.0f);
        matShadowSpace = matProj * matView;

        shaderShadow.useProgram();
        shaderShadow.setMatrix4f("matShadowSpace", matShadowSpace);  // ~= matProj * matView in the MVP.

        for (Entity* e : entities)
        {
            if (!e->m_Model)
                continue;

            shaderShadow.setMatrix4f("matModel", Mth::matModel(e->getPosition(), e->getRotation(), glm::vec3(1)));

            e->m_Model->drawArrays();
        }

    }

};

#endif //ETHERTIA_SHADOWRENDERER_H
