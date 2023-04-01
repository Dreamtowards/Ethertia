//
// Created by Dreamtowards on 2023/2/12.
//

#ifndef ETHERTIA_COMPOSERENDERER_H
#define ETHERTIA_COMPOSERENDERER_H

// Deferred Renderer compose stage.
// take certain input: gPositionDepth, gNormal, gAlbedoSpec

#include <ethertia/render/RenderEngine.h>
#include <ethertia/init/DebugStat.h>

class ComposeRenderer
{
public:

    inline static ShaderProgram* shaderCompose = ShaderProgram::decl("shaders/deferred/compose.{}");

    // Deferred Rendering Compose FBO
    inline static Framebuffer* fboCompose = nullptr;

    inline static float fogDensity = 0.01f;
    inline static float fogGradient = 1.5f;

    ComposeRenderer() = delete;


    static void init()
    {
        fboCompose = Framebuffer::GenFramebuffer(1280, 720,[](Framebuffer* fbo)
        {
            fbo->attachColorTexture(0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
        });

        shaderCompose->useProgram();
        shaderCompose->setInt("gPositionDepth", 0);
        shaderCompose->setInt("gNormal", 1);
        shaderCompose->setInt("gAlbedoRoughness", 2);
        shaderCompose->setInt("gAmbientOcclusion", 3);
        shaderCompose->setInt("gShadowMap", 4);  // Shadow Depth Map.

        // shaderCompose.setInt("panoramaMap", 5);

        shaderCompose->m_Uniforms["fogGradient"].bind(&fogGradient);
        shaderCompose->m_Uniforms["fogDensity"].bind(&fogDensity);

    }


    static void renderCompose(Texture* gPositionDepth, Texture* gNormal, Texture* gAlbedoRoughness,
                              Texture* gAmbientOcclusion,
                              Texture* gShadowMap, glm::mat4 matShadowSpace,
                              const std::vector<Light*>& lights)
    {
        // tmp add for debug ReloadShader
        shaderCompose->useProgram();
        shaderCompose->setInt("gPositionDepth", 0);
        shaderCompose->setInt("gNormal", 1);
        shaderCompose->setInt("gAlbedoRoughness", 2);
        shaderCompose->setInt("gAmbientOcclusion", 3);
        shaderCompose->setInt("gShadowMap", 4);  // Shadow Depth Map.


        gPositionDepth   ->BindTexture(0);
        gNormal          ->BindTexture(1);
        gAlbedoRoughness ->BindTexture(2);
        gAmbientOcclusion->BindTexture(3);
        gShadowMap       ->BindTexture(4);


        // g_PanoramaTex = Loader::loadTexture(Loader::loadPNG(Loader::loadAssets("misc/skybox/hdri5.png")));

        shaderCompose->useProgram();

        Camera& cam = Ethertia::getCamera();
        shaderCompose->setVector3f("CameraPos", cam.actual_pos);
        shaderCompose->setVector3f("CameraDir", cam.direction);

        HitCursor& cur = Ethertia::getHitCursor();
        shaderCompose->setVector3f("cursorPos", cur.cell ? cur.cell_position : glm::vec3{0,0,0});
        shaderCompose->setFloat("cursorSize", cur.brushSize);

        shaderCompose->setVector3f("dbg_Color", Dbg::dbg_ShaderDbgColor);
//
//        shaderCompose.setFloat("debugVar0", debugVar0);
//        shaderCompose->setFloat("debugVar1", EntityRenderer::debugVar1);
//        shaderCompose.setFloat("debugVar2", debugVar2);

        shaderCompose->setFloat("Time", Ethertia::getPreciseTime());
        shaderCompose->setFloat("DayTime", Ethertia::getWorld()->getDayTime());

        shaderCompose->setMatrix4f("matInvView", glm::inverse(cam.matView));
        shaderCompose->setMatrix4f("matInvProjection", glm::inverse(cam.matProjection));

        shaderCompose->setViewProjection();

        shaderCompose->setMatrix4f("matShadowSpace", matShadowSpace);

        shaderCompose->setInt("lightsCount", lights.size());
        for (int i = 0; i < std::min(64, (int)lights.size()); ++i)
        {
            Light* light = lights[i];

            static auto u_LPos = ShaderProgram::_GenArrayNames("lights[{}].position", 64);
            shaderCompose->setVector3f(u_LPos[i],light->position);

            static auto u_LColor = ShaderProgram::_GenArrayNames("lights[{}].color", 64);
            shaderCompose->setVector3f(u_LColor[i], light->color);

            static auto u_LAtten = ShaderProgram::_GenArrayNames("lights[{}].attenuation", 64);
            shaderCompose->setVector3f(u_LAtten[i], light->attenuation);

            static auto u_LDir = ShaderProgram::_GenArrayNames("lights[{}].direction", 64);
            shaderCompose->setVector3f(u_LDir[i], light->direction);

            static auto u_LConeAng = ShaderProgram::_GenArrayNames("lights[{}].coneAngle", 64);
            shaderCompose->setVector2f(u_LConeAng[i], glm::vec2(light->coneAngle, 0));
        }


        RenderCommand::DrawFullQuad();
    }

};

#endif //ETHERTIA_COMPOSERENDERER_H
