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

void RenderEngine::renderWorld(World* world)
{
    assert(world);


    ParticleRenderer::updateSunMoonPos();

    m_ParticleRenderer->updateAll(Ethertia::getDelta());


    // Geometry of Deferred Rendering
    {
        PROFILE("Geo");
Framebuffer::gPushFramebuffer(gbuffer);

    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glDisable(GL_BLEND);  // Blending is inhabited in Deferred Rendering.
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    dbg_NumEntityRendered = 0;
    for (Entity* entity : world->getEntities()) {
        if (!entity || !entity->m_Model) continue;
        if (entity->m_Model->vertexCount == 0) // most are Empty Chunks.
            continue;

        // Frustum Culling
        if (!m_ViewFrustum.intersects(entity->getAABB())) {
            continue;
        }

        bool isFolige = entity->m_TypeTag == Entity::TypeTag::T_CHUNK_VEGETABLE;
        if (isFolige) {
            if (dbg_NoVegetable) continue;
            glDisable(GL_CULL_FACE);
        }

        if (entity == (void*)Ethertia::getPlayer() && Ethertia::getCamera()->len == 0)
            continue;

        PROFILE("E/"+std::to_string(entity->m_TypeTag));

        entityRenderer->renderGeometryChunk(entity->m_Model, entity->getPosition(), entity->getRotation(), entity->m_DiffuseMap, isFolige ? 0.1 : 0.0);

        if (entity->m_TypeTag == Entity::TypeTag::T_CHUNK_VEGETABLE) {
            glEnable(GL_CULL_FACE);  // setback
        }

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



    // Compose of Deferred Rendering

    {
    PROFILE("Cmp");
    Framebuffer::gPushFramebuffer(dcompose);

    static glm::vec3 SkyColor = Colors::parseHexRGB("87cef4");  // 0.702, 0.812, 0.969  McPulp: 969df7 (0.588, 0.616, 0.969)

    glClearColor(0,0,0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

//        m_SkyGradientRenderer->render();

//        if (Ethertia::getWindow()->isKeyDown(GLFW_KEY_K)) {
//            const char* col = Loader::showInputBox("", "", "66ccff");
//            if (col)
//                SkyColor = Colors::parseHexRGB(col);
//        }


//    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
//        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
        if (!Ethertia::getWindow()->isKeyDown(GLFW_KEY_P)) {
            float time = Ethertia::getPreciseTime();

            if (world->m_DayTime >= 0.25 && world->m_DayTime < 0.75) { // 6AM-6PM
                GlState::blendMode(GlState::ADD);
                m_SkyboxRenderer->render(SkyboxRenderer::Tex_Cloud, {0,1,0}, -time / 60, glm::vec4{0.15});
//                GlState::blendMode(GlState::ALPHA);
//                m_SkyboxRenderer->render(SkyboxRenderer::Tex_Cloud, {0,1,0}, -time / 60, glm::vec4{1});

                GlState::blendMode(GlState::ADD);
                static Texture *Atmos = Loader::loadCubeMap1("misc/sky/cloudbox/atmosphere.png");
                m_SkyboxRenderer->render(Atmos, {0,1,0}, time / 60);

                GlState::blendMode(GlState::ADD);
                static Texture* Horizon = Loader::loadCubeMap1("misc/sky/cloudbox/horizon.png");
                m_SkyboxRenderer->render(Horizon, {0,1,0}, -time/40);
            } else {
                static Texture* Stars = Loader::loadCubeMap1("misc/sky/nightbox/stars.png");
                m_SkyboxRenderer->render(Stars, {0,0,1}, time/60);

                static Texture *Aurora = Loader::loadCubeMap1("misc/sky/nightbox/aurora.png");
                m_SkyboxRenderer->render(Aurora, {-0.3,1,0}, -time / 70);

                static Texture *Galaxies = Loader::loadCubeMap1("misc/sky/nightbox/galaxies.png");
                m_SkyboxRenderer->render(Galaxies, {0.4,0.4,0}, -time / 70);

                static Texture *Depth = Loader::loadCubeMap1("misc/sky/nightbox/depth.png");
                m_SkyboxRenderer->render(Depth, {1,0,0}, -time / 50);
            }

        }

        {
            // render Sun and Moon img.

            GlState::blendMode(GlState::ADD);

            static Texture* TEX_SUN = Loader::loadTexture("misc/sky/sun.png");
            static Texture* TEX_MOON = Loader::loadTexture("misc/sky/moon.png");

            using glm::vec3;
            // -PI/2: DayTime:0 as midnight SunPos instead of Morning.
            // glm::rotate(glm::mat4(1), angle, glm::vec3(0, 0, 1)) * glm::vec4(1, 0, 0, 1.0);
            vec3 relSunPos = Mth::anglez(world->m_DayTime * 2*Mth::PI - 0.5f*Mth::PI) * 300.0f;

            vec3 CamPos = Ethertia::getCamera()->position;
            vec3 SunPos = CamPos + relSunPos;
            vec3 MoonPos = CamPos - relSunPos;

            m_ParticleRenderer->render(TEX_SUN, SunPos, 180.0f);
            m_ParticleRenderer->render(TEX_MOON, MoonPos, 180.0f);

        }

        // CNS. 让接下来Alpha!=1.0的地方的颜色 添加到背景颜色中 为了接下来的天空颜色叠加
        // use Screen Blend: addictive color.
        GlState::blendMode(GlState::SCREEN);  // glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        entityRenderer->renderCompose(gbuffer->texColor[0], gbuffer->texColor[1], gbuffer->texColor[2]);

        for (Entity* e : world->getEntities())
        {
            if (EntityDroppedItem* droppedItem = dynamic_cast<EntityDroppedItem*>(e))
            {
                const Item& item = *droppedItem->m_DroppedItem.item();

                m_ParticleRenderer->render(ItemTextures::ITEM_ATLAS, droppedItem->getPosition(), 0.3f);
            }
        }

Framebuffer::gPopFramebuffer();
    }

    // Result.

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