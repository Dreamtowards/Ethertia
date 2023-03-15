//
// Created by Dreamtowards on 2023/2/14.
//

#ifndef ETHERTIA_SHADOWRENDERER_H
#define ETHERTIA_SHADOWRENDERER_H

#include <glm/gtc/matrix_transform.hpp>

#include <ethertia/render/ShaderProgram.h>
#include <ethertia/render/RenderCommand.h>

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

        auto _ap = fboDepthMap->BeginFramebuffer_Scoped();

        RenderCommand::Clear();


        glm::vec3 shadowPos = Ethertia::getCamera().position;
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

            SHADER->setMatrix4f("matModel", Mth::matModel(e->position(), e->getRotation(), glm::vec3(1)));

            e->m_Model->_glDrawArrays();
        }

    }

};

#endif //ETHERTIA_SHADOWRENDERER_H
