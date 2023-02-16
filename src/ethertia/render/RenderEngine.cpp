//
// Created by Dreamtowards on 2022/8/22.
//

#include "RenderEngine.h"
#include "GlState.h"


#include <ethertia/world/World.h>
#include <ethertia/render/renderer/EntityRenderer.h>
#include <ethertia/render/sky/SkyGradientRenderer.h>
#include <ethertia/render/sky/SkyboxRenderer.h>
#include <ethertia/render/gui/GuiRenderer.h>
#include <ethertia/render/gui/FontRenderer.h>
#include <ethertia/render/particle/ParticleRenderer.h>
#include <ethertia/render/compose/ComposeRenderer.h>
#include <ethertia/render/ssao/SSAORenderer.h>
#include <ethertia/render/debug/DebugRenderer.h>
#include <ethertia/render/shadow/ShadowRenderer.h>

#include <ethertia/render/Window.h>
#include <ethertia/entity/EntityDroppedItem.h>
#include <ethertia/init/ItemTextures.h>

// RenderEngine is static/singleton. shouldn't instantiate.
// Don't use OOP except it's necessary.


RenderEngine::RenderEngine()
{
    BenchmarkTimer _tm;
    Log::info("RenderEngine initializing.\1");


    float qual = 0.7;
    fboGbuffer = Framebuffer::GenFramebuffer((int)(1280 * qual), (int)(720 * qual), [](Framebuffer* fbo)
    {
        fbo->attachColorTexture(0, GL_RGBA32F, GL_RGBA, GL_FLOAT);      // Positions, Depth, f16 *3
        fbo->attachColorTexture(1, GL_RGB32F, GL_RGB, GL_FLOAT);        // Normals,          f16 *3
        fbo->attachColorTexture(2, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE); // Albedo,           u8 *3
        fbo->setupMRT({0, 1, 2});

        fbo->attachDepthStencilRenderbuffer();
    });


    std::cout << " renderers[";
    guiRenderer = new GuiRenderer();      std::cout << "gui, ";
    fontRenderer = new FontRenderer();    std::cout << "font, ";
    entityRenderer = new EntityRenderer();std::cout << "entity";
    m_SkyboxRenderer = new SkyboxRenderer();
    m_ParticleRenderer = new ParticleRenderer();
    m_SkyGradientRenderer = new SkyGradientRenderer();

    SSAORenderer::init();
    ComposeRenderer::init();

    ShadowRenderer::init();

    std::cout << "]";




    RenderEngine::checkGlError("End of RenderEngine Init");

}

RenderEngine::~RenderEngine() {

    delete guiRenderer;
    delete fontRenderer;
    delete entityRenderer;
    delete m_SkyboxRenderer;
    delete m_ParticleRenderer;
    delete m_SkyGradientRenderer;
}


void RenderEngine::renderWorldGeometry(World* world) {

    // Geometry of Deferred Rendering

    PROFILE("Geo");
    auto _ap = fboGbuffer->bindFramebuffer_ap();

    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glDisable(GL_BLEND);  // Blending is inhabited in Deferred Rendering.


    dbg_NumEntityRendered = 0;
    for (Entity* entity : world->getEntities()) {
        if (!entity || !entity->m_Model) continue;
        if (entity->m_Model->vertexCount == 0) // most are Empty Chunks.
            continue;

        // Frustum Culling
        if (!m_ViewFrustum.intersects(entity->getAABB())) {
            continue;
        }

        if (entity == (void*)Ethertia::getPlayer() && Ethertia::getCamera()->len == 0)
            continue;


        entity->onRender();


        ++dbg_NumEntityRendered;

        // Debug: draw Norm/Border
        if (dbg_EntityGeo) {
            DebugRenderer::Inst().renderDebugGeo(entity->m_Model, entity->getPosition(), entity->getRotation());
        }
        // Debug: draw Entity that Crosshair Hits.
        if (RenderEngine::dbg_HitPointEntityGeo && entity == Ethertia::getCursor().hitEntity) {
            DebugRenderer::Inst().renderDebugGeo(entity->m_Model, entity->getPosition(), entity->getRotation(),
                           Ethertia::getCursor().brushSize, Ethertia::getCursor().position);
        }
        // Debug: draw Every Entity AABB.
        if (dbg_RenderedEntityAABB) {
            AABB aabb = entity->getAABB();
            RenderEngine::drawLineBox(aabb.min, aabb.max-aabb.min, Colors::RED);
        }
    }

    glEnable(GL_BLEND);

}


void RenderEngine::renderWorldCompose(World* world)
{
    // Compose of Deferred Rendering

    PROFILE("Cmp");
    auto _ap = ComposeRenderer::fboCompose->bindFramebuffer_ap();

    glClearColor(0,0,0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float daytime = Ethertia::getWorld()->getDayTime();

    m_SkyboxRenderer->renderWorldSkybox(daytime);

    m_ParticleRenderer->renderSunMoonTex(daytime);

    std::vector<Light*> renderLights;  // lights to be render.
//    {
//        static Light l{};
//        l.position = Ethertia::getCamera()->position;
//        l.color = glm::vec3(0.4, 1, 0.8);
//        l.direction = Ethertia::getCamera()->direction;
//        l.attenuation = {0,0,0};
//        renderLights.push_back(&l);
//    }
    {
        static Light sunLight;

        // CNS 这是不准确的，不能直接取反 否则月亮位置可能会错误
        sunLight.position = Ethertia::getCamera()->position + (-RenderEngine::SunlightDir(daytime) * 100.0f);


        float dayBrightness = 1.0 - abs(daytime-0.5) * 2.0;
        glm::vec3 SunColor = glm::vec3(1.0) * dayBrightness * 2.5f;
        sunLight.color = SunColor;

        sunLight.attenuation = {1, 0, 0};

        renderLights.push_back(&sunLight);
    }
    for (Entity* e : world->m_Entities)
    {
        if (EntityLantern* lat = dynamic_cast<EntityLantern*>(e))
        {
            renderLights.push_back(lat->getLights());
        }

        if (EntityTorch* lat = dynamic_cast<EntityTorch*>(e))
        {
            renderLights.push_back(lat->getLights());

            ParticleRenderer::m_Particles.push_back(lat->genParticles());
        }
    }


    // CNS. 让接下来Alpha!=1.0的地方的颜色 添加到背景颜色中 为了接下来的天空颜色叠加
    // Blend: addictive color when src.alpha != 1.0, for sky background add.
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    ComposeRenderer::renderCompose(fboGbuffer->texColor[0], fboGbuffer->texColor[1], fboGbuffer->texColor[2],
                                   SSAORenderer::fboSSAO->texColor[0],
                                   ShadowRenderer::fboDepthMap->texDepth, ShadowRenderer::matShadowSpace,
                                   renderLights);

    GlState::blendMode(GlState::ALPHA);

    for (Entity* e : world->getEntities())
    {
        if (EntityDroppedItem* droppedItem = dynamic_cast<EntityDroppedItem*>(e))
        {
            const Item& item = *droppedItem->m_DroppedItem.item();

            float i = Item::REGISTRY.getOrderId((Item*)&item);
            float n = Item::REGISTRY.size();

            m_ParticleRenderer->render(ItemTextures::ITEM_ATLAS, droppedItem->getPosition(), 0.3f,
                                       {i/n, 0},
                                       {1/n, 1});
        }
    }

    glEnable(GL_DEPTH_TEST);

    m_ParticleRenderer->renderAll();

}


void RenderEngine::renderWorld(World* world)
{
    assert(world);

    RenderEngine::checkGlError("Begin World Render");

    m_ParticleRenderer->updateAll(Ethertia::getDelta());

    // Heavy
    renderWorldGeometry(world);


    ShadowRenderer::renderDepthMap(world->m_Entities, SunlightDir(world->getDayTime()));

    SSAORenderer::renderSSAO(fboGbuffer->texColor[0], fboGbuffer->texColor[1]);


    renderWorldCompose(world);


    Gui::drawRect(0, 0, Gui::maxWidth(), Gui::maxHeight(), ComposeRenderer::fboCompose->texColor[0]);


    RenderEngine::checkGlError("End World Render");
}


void RenderEngine::drawLine(glm::vec3 pos, glm::vec3 dir, glm::vec4 color, bool viewMat, bool boxOutline) {
    DebugRenderer::Inst().drawLine(pos, dir, color, viewMat, boxOutline);
}

void RenderEngine::drawLineBox(glm::vec3 min, glm::vec3 size, glm::vec4 color) {
    DebugRenderer::Inst().drawLine(min, size, color, true, true);
}



//    Get World Ray from Screen Pixel.
//        Window* _wd = Ethertia::getWindow();
//        RenderEngine* _rde = Ethertia::getRenderEngine();
//        glm::vec3 ray = Mth::worldRay(_wd->getMouseX(), _wd->getMouseY(), _wd->getWidth(), _wd->getHeight(), _rde->projectionMatrix, _rde->viewMatrix);
//
//        Log::info("ray {}", glm::to_string(ray));