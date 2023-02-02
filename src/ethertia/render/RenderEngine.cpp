//
// Created by Dreamtowards on 2022/8/22.
//

#include "RenderEngine.h"
#include "GlState.h"


#include <ethertia/world/World.h>
#include <ethertia/render/renderer/EntityRenderer.h>
#include <ethertia/render/renderer/SkyGradientRenderer.h>
#include <ethertia/render/renderer/SkyboxRenderer.h>
#include <ethertia/render/renderer/gui/GuiRenderer.h>
#include <ethertia/render/renderer/gui/FontRenderer.h>
#include <ethertia/render/renderer/ParticleRenderer.h>
#include <ethertia/render/Window.h>
#include <ethertia/entity/EntityDroppedItem.h>
#include <ethertia/init/ItemTextures.h>


RenderEngine::RenderEngine()
{
    BenchmarkTimer _tm;
    Log::info("RenderEngine initializing.\1");

    std::cout << " renderers[";
    guiRenderer = new GuiRenderer();      std::cout << "gui, ";
    fontRenderer = new FontRenderer();    std::cout << "font, ";
    entityRenderer = new EntityRenderer();std::cout << "entity";
    m_SkyboxRenderer = new SkyboxRenderer();
    m_ParticleRenderer = new ParticleRenderer();
    m_SkyGradientRenderer = new SkyGradientRenderer();
    std::cout << "]";

    float qual = 0.6;
    gbuffer = Framebuffer::glfGenFramebuffer((int)(1280 * qual), (int)(720 * qual));
    Framebuffer::gPushFramebuffer(gbuffer);
        gbuffer->attachColorTexture(0, GL_RGBA32F, GL_RGBA, GL_FLOAT);      // Positions, Depth, f16 *3
        gbuffer->attachColorTexture(1, GL_RGB32F, GL_RGB, GL_FLOAT);        // Normals,          f16 *3
        gbuffer->attachColorTexture(2, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE); // Albedo,           u8 *3
        gbuffer->setupMRT({0, 1, 2});

        gbuffer->attachDepthStencilRenderbuffer();
        gbuffer->checkFramebufferStatus();
    Framebuffer::gPopFramebuffer();

    dcompose = Framebuffer::glfGenFramebuffer(gbuffer->width, gbuffer->height);
    Framebuffer::gPushFramebuffer(dcompose);
        dcompose->attachColorTexture(0, GL_RGBA);
        dcompose->checkFramebufferStatus();
    Framebuffer::gPopFramebuffer();

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
    Framebuffer::gPushFramebuffer(gbuffer);

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

//        bool noFaceCulling = EntityMesh::isFaceCulling(entity);
//        if (noFaceCulling) {
//            if (dbg_NoVegetable) continue;
//            glDisable(GL_CULL_FACE);
//        }

        if (entity == (void*)Ethertia::getPlayer() && Ethertia::getCamera()->len == 0)
            continue;

        //PROFILE("E/"+std::to_string(entity->m_TypeTag));

        entity->onRender();
//
//        entityRenderer->renderGeometryChunk(entity->m_Model, entity->getPosition(), entity->getRotation(),
//                                            entity->m_DiffuseMap, noFaceCulling ? 0.1 : 0.0);
//
//        if (noFaceCulling) {
//            glEnable(GL_CULL_FACE);  // setback
//        }

        ++dbg_NumEntityRendered;
        if (dbg_EntityGeo) {
            renderDebugGeo(entity->m_Model, entity->getPosition(), entity->getRotation());
        }
        if (RenderEngine::dbg_HitPointEntityGeo && entity == Ethertia::getBrushCursor().hitEntity) {
            renderDebugGeo(entity->m_Model, entity->getPosition(), entity->getRotation(),
                           Ethertia::getBrushCursor().brushSize, Ethertia::getBrushCursor().position);
        }
        if (dbg_RenderedEntityAABB) {
            AABB aabb = entity->getAABB();
            renderLineBox(aabb.min, aabb.max-aabb.min, Colors::RED);
        }
    }


    glEnable(GL_BLEND);

    Framebuffer::gPopFramebuffer();
}


void RenderEngine::renderWorldCompose(World* world)
{
    // Compose of Deferred Rendering

    PROFILE("Cmp");
    Framebuffer::gPushFramebuffer(dcompose);

    glClearColor(0,0,0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    m_SkyboxRenderer->renderWorldSkybox(world->m_DayTime);

    m_ParticleRenderer->renderSunMoonTex(world->m_DayTime);

    std::vector<Light*> renderLights;  // lights to be render.
//    {
//        static Light l{};
//        l.position = Ethertia::getCamera()->position;
//        l.color = glm::vec3(0.4, 1, 0.8);
//        l.direction = Ethertia::getCamera()->direction;
//        l.attenuation = {0,0,0};
//        renderLights.push_back(&l);
//    }
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

    entityRenderer->renderCompose(gbuffer->texColor[0], gbuffer->texColor[1], gbuffer->texColor[2], renderLights);

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

    Framebuffer::gPopFramebuffer();
}

void RenderEngine::renderWorld(World* world)
{
    assert(world);

    m_ParticleRenderer->updateAll(Ethertia::getDelta());

    // Heavy
    renderWorldGeometry(world);


    renderWorldCompose(world);


    Gui::drawRect(0, 0, Gui::maxWidth(), Gui::maxHeight(), dcompose->texColor[0]);


    RenderEngine::checkGlError("End World Render");
}

void RenderEngine::checkGlError(std::string_view phase) {
    GLuint err = glGetError();
    if (err) {
        Log::warn("###### GL Error @{} ######", phase);
        Log::warn("ERR: {}", err);
    }
}


//    Get World Ray from Screen Pixel.
//        Window* _wd = Ethertia::getWindow();
//        RenderEngine* _rde = Ethertia::getRenderEngine();
//        glm::vec3 ray = Mth::worldRay(_wd->getMouseX(), _wd->getMouseY(), _wd->getWidth(), _wd->getHeight(), _rde->projectionMatrix, _rde->viewMatrix);
//
//        Log::info("ray {}", glm::to_string(ray));