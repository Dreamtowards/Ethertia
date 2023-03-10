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
    DECL_SHADER(SHADER, "shaders/shadow/shadow.{}");

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
    }

    static void renderDepthMap(const std::vector<Entity*>& entities, glm::vec3 shadowDir)
    {
        assert(glIsEnabled(GL_DEPTH_TEST));

        auto _ap = fboDepthMap->bindFramebuffer_ap();

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


        glm::vec3 shadowPos = RenderEngine::g_Camera.position;
        glm::mat4 matView = glm::lookAt(shadowPos, shadowPos+shadowDir, glm::vec3{0,1,0});

        glm::mat4 matProj = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f,
                                       -1000.0f, 1000.0f);
        matShadowSpace = matProj * matView;

        SHADER->useProgram();
        SHADER->setMatrix4f("matShadowSpace", matShadowSpace);  // ~= matProj * matView in the MVP.

        for (Entity* e : entities)
        {
            if (!e->m_Model)
                continue;

            SHADER->setMatrix4f("matModel", Mth::matModel(e->getPosition(), e->getRotation(), glm::vec3(1)));

            e->m_Model->_glDrawArrays();
        }

    }

};

#endif //ETHERTIA_SHADOWRENDERER_H
