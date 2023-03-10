//
// Created by Dreamtowards on 2023/2/12.
//

#ifndef ETHERTIA_COMPOSERENDERER_H
#define ETHERTIA_COMPOSERENDERER_H

// Deferred Renderer compose stage.
// take certain input: gPositionDepth, gNormal, gAlbedoSpec

class ComposeRenderer
{
public:

    inline static ShaderProgram* shaderCompose = ShaderProgram::decl("shaders/compose/compose.{}");

    // Deferred Rendering Compose FBO
    inline static Framebuffer* fboCompose = nullptr;

    inline static float fogDensity = 0.01f;
    inline static float fogGradient = 1.5f;

    ComposeRenderer() = delete;

    static void initShader() {

        shaderCompose->useProgram();
        shaderCompose->setInt("gPositionDepth", 0);
        shaderCompose->setInt("gNormal", 1);
        shaderCompose->setInt("gAlbedoRoughness", 2);
        shaderCompose->setInt("gAmbientOcclusion", 3);
        shaderCompose->setInt("gShadowMap", 4);  // Shadow Depth Map.

        // shaderCompose.setInt("panoramaMap", 5);
    }

    static void init()
    {
        fboCompose = Framebuffer::GenFramebuffer(1280, 720,[](Framebuffer* fbo)
        {
            fbo->attachColorTexture(0, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
        });


        initShader();

    }


    static void renderCompose(Texture* gPositionDepth, Texture* gNormal, Texture* gAlbedoRoughness,
                              Texture* gAmbientOcclusion,
                              Texture* gShadowMap, glm::mat4 matShadowSpace,
                              const std::vector<Light*>& lights)
    {
        gPositionDepth   ->bindTexture2D(0);
        gNormal          ->bindTexture2D(1);
        gAlbedoRoughness ->bindTexture2D(2);
        gAmbientOcclusion->bindTexture2D(3);
        gShadowMap       ->bindTexture2D(4);


        // g_PanoramaTex = Loader::loadTexture(Loader::loadPNG(Loader::loadAssets("misc/skybox/hdri5.png")));

        shaderCompose->useProgram();

        shaderCompose->setVector3f("CameraPos", Ethertia::getCamera()->actual_pos);
        shaderCompose->setVector3f("CameraDir", Ethertia::getCamera()->direction);

        HitCursor& cur = Ethertia::getHitCursor();
        shaderCompose->setVector3f("cursorPos", cur.cell ? cur.cell_position : glm::vec3{0,0,0});
        shaderCompose->setFloat("cursorSize", cur.brushSize);

        shaderCompose->setFloat("fogGradient", fogGradient);
        shaderCompose->setFloat("fogDensity", fogDensity);
//
//        shaderCompose.setFloat("debugVar0", debugVar0);
        shaderCompose->setFloat("debugVar1", EntityRenderer::debugVar1);
//        shaderCompose.setFloat("debugVar2", debugVar2);

        shaderCompose->setFloat("Time", Ethertia::getPreciseTime());
        shaderCompose->setFloat("DayTime", Ethertia::getWorld()->getDayTime());

        shaderCompose->setMatrix4f("matInvView", glm::inverse(RenderEngine::matView));
        shaderCompose->setMatrix4f("matInvProjection", glm::inverse(RenderEngine::matProjection));

        shaderCompose->setMatrix4f("matView", RenderEngine::matView);
        shaderCompose->setMatrix4f("matProjection", RenderEngine::matProjection);

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



        _DrawScreenQuad();
    }

    static Model* _Model_ScreenQuad() {
        static Model* INST = nullptr;
        if (!INST) {
            // init RECT. def full viewport.
            float _RECT_POS[] = {1,-1, 1,1, -1,-1, -1,1};
            float _RECT_UV[]  = {1,0,  1,1, 0,0,  0,1};
            INST = Loader::loadModel(4, {
                    {2, _RECT_POS},
                    {2, _RECT_UV}
            });
        }
        return INST;
    }

    static void _DrawScreenQuad() {
        Model* m = _Model_ScreenQuad();
        glBindVertexArray(m->vaoId);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, m->vertexCount);
    }
};

#endif //ETHERTIA_COMPOSERENDERER_H
