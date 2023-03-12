//
// Created by Dreamtowards on 2022/8/22.
//

#include "RenderEngine.h"
#include "GlState.h"


#include <ethertia/world/World.h>
#include <ethertia/render/deferred/GeometryRenderer.h>
#include <ethertia/render/deferred/ComposeRenderer.h>
#include <ethertia/render/sky/SkyGradientRenderer.h>
#include <ethertia/render/sky/SkyboxRenderer.h>
#include <ethertia/render/gui/GuiRenderer.h>
#include <ethertia/render/gui/FontRenderer.h>
#include <ethertia/render/particle/ParticleRenderer.h>
#include <ethertia/render/ssao/SSAORenderer.h>
#include <ethertia/render/debug/DebugRenderer.h>
#include <ethertia/render/shadow/ShadowRenderer.h>
#include <ethertia/render/anim/AnimRenderer.h>

#include <ethertia/render/Window.h>
#include <ethertia/entity/EntityDroppedItem.h>
#include <ethertia/init/ItemTextures.h>
#include <ethertia/entity/player/EntityPlayer.h>
#include <ethertia/init/Settings.h>

// RenderEngine is static/singleton. shouldn't instantiate.
// Don't use OOP except it's necessary.


void RenderEngine::init()
{
    BenchmarkTimer _tm;
    Log::info("RenderEngine initializing.\1");




    std::cout << " renderers[";
    GuiRenderer::init();        std::cout << "gui, ";
    FontRenderer::init();       std::cout << "font, ";
    GeometryRenderer::init();     std::cout << "geometry";
    ComposeRenderer::init();
    SkyboxRenderer::init();
    ParticleRenderer::init();
    //SkyGradientRenderer::init();

    SSAORenderer::init();
    ShadowRenderer::init();

    std::cout << "]";




    RenderEngine::checkGlError("End of RenderEngine Init");

}

void RenderEngine::deinit()
{
    // todo: deinit renderers.
}


void RenderEngine::framePrepare()
{
    Camera& cam = Ethertia::getCamera();

    cam.update(Ethertia::isIngame());

    if (Ethertia::isIngame())
    {
        cam.position = Ethertia::getPlayer()->getPosition();
    }

    RenderEngine::clearRenderBuffer();
}


void RenderEngine::renderWorldGeometry(World* world) {

    // Geometry of Deferred Rendering

    PROFILE("Geo");
    auto _ap = GeometryRenderer::fboGbuffer->bindFramebuffer_ap();

    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glDisable(GL_BLEND);  // Blending is inhabited in Deferred Rendering.


    Settings::dbgEntitiesRendered = 0;
    for (Entity* entity : world->getEntities()) {
        if (!entity || !entity->m_Model) continue;
        if (entity->m_Model->vertexCount == 0) // most are Empty Chunks.
            continue;

        // Frustum Culling
        if (!Ethertia::getCamera().testFrustum(entity->getAABB())) {
            continue;
        }

        if (entity == (void*)Ethertia::getPlayer() && Ethertia::getCamera().len == 0)
            continue;


        entity->onRender();


        ++Settings::dbgEntitiesRendered;

        // Debug: draw Norm/Border
        if (dbg_EntityGeo) {
            DebugRenderer::Inst().renderDebugGeo(entity->m_Model, entity->getPosition(), entity->getRotation());
        }
        // Debug: draw Entity that Crosshair Hits.
        if (RenderEngine::dbg_HitPointEntityGeo && entity == Ethertia::getHitCursor().hitEntity) {
            DebugRenderer::Inst().renderDebugGeo(entity->m_Model, entity->getPosition(), entity->getRotation(),
                           Ethertia::getHitCursor().brushSize, Ethertia::getHitCursor().position);
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

    SkyboxRenderer::renderWorldSkybox(daytime);

    ParticleRenderer::renderSunMoonTex(daytime);

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
        sunLight.position = Ethertia::getCamera().position + (-RenderEngine::SunlightDir(daytime) * 100.0f);


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

    Texture** gbufferTexs = GeometryRenderer::fboGbuffer->texColor;
    ComposeRenderer::renderCompose(gbufferTexs[0], gbufferTexs[1], gbufferTexs[2],
                                   SSAORenderer::fboSSAO->texColor[0],
                                   ShadowRenderer::fboDepthMap->texDepth, ShadowRenderer::matShadowSpace,
                                   renderLights);

    GlState::blendMode(GlState::ALPHA);

    for (Entity* e : world->getEntities())
    {
        // todo: EntityDroppedItem :: onRenderCompose();
        if (EntityDroppedItem* eDroppedItem = dynamic_cast<EntityDroppedItem*>(e))
        {
            const ItemStack& stack = eDroppedItem->m_DroppedItem;

            float i = Item::REGISTRY.getOrderId(stack.item());
            float n = Item::REGISTRY.size();

            ParticleRenderer::render(ItemTextures::ITEM_ATLAS, eDroppedItem->getPosition(), 0.3f,
                                       {i/n, 0},
                                       {1/n, 1});

            if (stack.amount() > 1)
            {
                // tmp draw count.
                Gui::drawWorldpoint(eDroppedItem->getPosition(), [&](glm::vec2 p) {
                    Gui::drawString(p.x, p.y, std::to_string(stack.amount()));
                });
            }
        }
    }

    glEnable(GL_DEPTH_TEST);

    ParticleRenderer::renderAll();

}


void RenderEngine::renderWorld(World* world)
{
    assert(world);
    if (Settings::dbg_PauseWorldRender) return;

    RenderEngine::checkGlError("Begin World Render");

    ParticleRenderer::updateAll(Ethertia::getDelta());

    // Heavy
    renderWorldGeometry(world);


    if (Settings::g_ShadowMapping)
    ShadowRenderer::renderDepthMap(world->m_Entities, SunlightDir(world->getDayTime()));

    if (Settings::g_SSAO)
    SSAORenderer::renderSSAO(GeometryRenderer::fboGbuffer->texColor[0], GeometryRenderer::fboGbuffer->texColor[1]);


    renderWorldCompose(world);


    Gui::drawRect(0, 0, Gui::maxWidth(), Gui::maxHeight(), ComposeRenderer::fboCompose->texColor[0]);


//    static Model* model = Loader::loadModel(100, {
//        {3, pos},
//        {2, uv},
//        {3, norm},
//        {3, joint_ids},
//        {3, joint_weights}});
//    AnimRenderer::Inst()->render();


    RenderEngine::checkGlError("End World Render");
}


void RenderEngine::drawLine(glm::vec3 pos, glm::vec3 dir, glm::vec4 color, bool viewMat, bool boxOutline) {
    DebugRenderer::Inst().drawLine(pos, dir, color, viewMat, boxOutline);
}

void RenderEngine:: drawLineBox(glm::vec3 min, glm::vec3 size, glm::vec4 color) {
    DebugRenderer::Inst().drawLine(min, size, color, true, true);
}

void RenderEngine::drawLineBox(const AABB& aabb, glm::vec4 color) {
    RenderEngine::drawLineBox(aabb.min, aabb.max-aabb.min, color);
}



//    Get World Ray from Screen Pixel.
//        Window* _wd = Ethertia::getWindow();
//        RenderEngine* _rde = Ethertia::getRenderEngine();
//        glm::vec3 ray = Mth::worldRay(_wd->getMouseX(), _wd->getMouseY(), _wd->getWidth(), _wd->getHeight(), _rde->projectionMatrix, _rde->viewMatrix);
//
//        Log::info("ray {}", glm::to_string(ray));