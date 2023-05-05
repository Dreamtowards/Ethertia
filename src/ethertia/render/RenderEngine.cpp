//
// Created by Dreamtowards on 2022/8/22.
//

#include "RenderEngine.h"


#include <ethertia/render/Window.h>
#include <ethertia/init/ItemTextures.h>
#include <ethertia/init/Settings.h>
#include <ethertia/entity/EntityDroppedItem.h>
#include <ethertia/entity/player/EntityPlayer.h>
#include <ethertia/init/DebugStat.h>


// RenderEngine is static/singleton. shouldn't instantiate.
// Don't use OOP except it's necessary.

#include <ethertia/imgui/Imgui.h>

//    std::cout << " renderers[";
////    GuiRenderer::init();        std::cout << "gui, ";
////    FontRenderer::init();       std::cout << "font, ";
////    GeometryRenderer::init();   std::cout << "geometry ";
////    ComposeRenderer::init();    std::cout << "deferred ";
////    SkyboxRenderer::init();     std::cout << "skybox ";
////    ParticleRenderer::init();   std::cout << "particle ";
//    //SkyGradientRenderer::init();
//
////    SSAORenderer::init();
////    ShadowRenderer::init();
//


namespace RendererGbuffer
{
    VkPipeline g_Pipeline;
    VkRenderPass g_RenderPass;

    VkDescriptorSetLayout g_DescriptorSetLayout = nullptr;
    VkPipelineLayout g_PipelineLayout = nullptr;

};



static int g_CurrentInflightFrame = 0;

static VkCommandBuffer g_InflightCommandBuffers[vkx::INFLIGHT_FRAMES];

static VkSemaphore g_SemaphoreImageAcquired[vkx::INFLIGHT_FRAMES];
static VkSemaphore g_SemaphoreRenderComplete[vkx::INFLIGHT_FRAMES];
static VkFence     g_InflightFence[vkx::INFLIGHT_FRAMES];


static VkDescriptorSetLayout g_DescriptorSetLayout = nullptr;
static VkPipelineLayout g_PipelineLayout = nullptr;

static vkx::UniformBuffer* g_UniformBuffers[vkx::INFLIGHT_FRAMES];
static VkDescriptorSet g_DescriptorSets[vkx::INFLIGHT_FRAMES];




#define DECL_unif alignas(16)

struct UniformBufferObject
{
    DECL_unif glm::mat4 model;
    DECL_unif glm::mat4 view;
    DECL_unif glm::mat4 proj;
};



static void CreateCommandBuffers()
{
    vl::AllocateCommandBuffers(vkx::ctx().Device, vkx::ctx().CommandPool,
                               std::size(g_InflightCommandBuffers), g_InflightCommandBuffers,
                               VK_COMMAND_BUFFER_LEVEL_PRIMARY);
}



static void CreateSyncObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkDevice device = vkx::ctx().Device;
    for (int i = 0; i < vkx::INFLIGHT_FRAMES; ++i)
    {
        VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &g_SemaphoreImageAcquired[i]));
        VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &g_SemaphoreRenderComplete[i]));
        VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &g_InflightFence[i]));
    }
}












static void CreateDescriptorSetLayout()
{
    g_DescriptorSetLayout = vl::CreateDescriptorSetLayout(vkx::ctx().Device, {
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
    });
}


static void CreateUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    for (int i = 0; i < vkx::INFLIGHT_FRAMES; ++i)
    {
        g_UniformBuffers[i] = new vkx::UniformBuffer(bufferSize);
    }
}

static void CreateDescriptorSets()
{
    VkDevice device = vkx::ctx().Device;
    std::vector<VkDescriptorSetLayout> layouts(vkx::INFLIGHT_FRAMES, g_DescriptorSetLayout);

    vl::AllocateDescriptorSets(device, vkx::ctx().DescriptorPool, vkx::INFLIGHT_FRAMES, layouts.data(), g_DescriptorSets);

    for (int i = 0; i < vkx::INFLIGHT_FRAMES; ++i)
    {
        vkx::DescriptorWrites writes{g_DescriptorSets[i]};

        writes.UniformBuffer(g_UniformBuffers[i]->buffer(), sizeof(UniformBufferObject));
        writes.CombinedImageSampler(RenderEngine::TEX_UVMAP->m_ImageView, vkx::ctx().ImageSampler);

        writes.WriteDescriptorSets(device);
    }
}



void RenderEngine::init()
{
    BENCHMARK_TIMER;
    Log::info("RenderEngine initializing.\1");

    vkx::Init(Ethertia::getWindow().m_WindowHandle, true);


    CreateCommandBuffers();
    CreateSyncObjects();


    CreateDescriptorSetLayout();
    g_PipelineLayout = vl::CreatePipelineLayout(vkx::ctx().Device, 1, &g_DescriptorSetLayout);


    {
        BitmapImage img(1, 1, new uint32_t[1]{(uint32_t)~0});
        TEX_WHITE = Loader::loadImage(img);

        TEX_UVMAP = Loader::loadImage("misc/uvmap.png");
    }

    CreateUniformBuffers();
    CreateDescriptorSets();


//    {
//        g_GraphicsPipeline =
//        vkx::CreateGraphicsPipeline(
//                {
//                        Loader::loadAssets("shaders-vk/spv/def_gbuffer/vert.spv"),
//                        Loader::loadAssets("shaders-vk/spv/def_gbuffer/frag.spv")
//                },
//                { VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT },
//                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
//                1,
//                {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR},
//                g_PipelineLayout,
//                vkx::ctx().MainRenderPass);
//    }




}


void RenderEngine::deinit()
{
    // todo: deinit renderers.
}




static void RecordCommandBuffer(VkCommandBuffer cmdbuf, VkFramebuffer framebuffer_MainRenderPass)
{

    vkx::CommandBuffer cmd{cmdbuf};
    cmd.BeginCommandBuffer(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

    // Deferred :: Gbuffer

    VkClearValue clearValues[4]{};
//    clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};  // gPosition
//    clearValues[1].color = {0.0f, 0.0f, 0.0f, 1.0f};  // gNormal
//    clearValues[2].color = {0.0f, 0.0f, 0.0f, 1.0f};  // gAlbedo
//    clearValues[3].depthStencil = {1.0f, 0};
//    VkExtent2D gbufferExtent = {1024, 1024};
//    cmd.CmdBeginRenderPass(g_Deferred_Gbuffer.m_RenderPass, g_Deferred_Gbuffer.m_Framebuffer, gbufferExtent,
//                           4, clearValues);
//
//    cmd.CmdBindGraphicsPipeline(g_Deferred_Gbuffer.m_Pipeline);
//    cmd.CmdSetViewport(gbufferExtent);
//    cmd.CmdSetScissor(gbufferExtent);
//
//    cmd.CmdBindDescriptorSets(g_PipelineLayout, &g_DescriptorSets[g_CurrentFrameInflight]);
//
//    cmd.CmdBindVertexBuffer(g_TestModel->vtxbuffer());
//    cmd.CmdBindIndexBuffer(g_TestModel->idxbuffer());
//    cmd.CmdDrawIndexed(g_TestModel->vertexCount());
//
//    cmd.CmdEndRenderPass();



    // Deferred :: Compose



    // Main
    clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    cmd.CmdBeginRenderPass(vkx::ctx().MainRenderPass, framebuffer_MainRenderPass, vkx::ctx().SwapchainExtent, 2, clearValues);
    cmd.CmdSetViewport(vkx::ctx().SwapchainExtent);
    cmd.CmdSetScissor(vkx::ctx().SwapchainExtent);

//    cmd.CmdBindGraphicsPipeline(g_GraphicsPipeline);
////    cmd.CmdBindGraphicsPipeline(g_Deferred_Compose.m_Pipeline);
//
//    cmd.CmdBindDescriptorSets(g_PipelineLayout, &g_DescriptorSets[g_CurrentInflightFrame]);
//
//    cmd.CmdDrawIndexed(6);

    {
        PROFILE("GUI");

        Imgui::RenderGUI(cmdbuf);
    }

    cmd.CmdEndRenderPass();

    cmd.EndCommandBuffer();
}

void UpdateUniformBuffer(int frameIdx)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};

    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), vkx::ctx().SwapchainExtent.width / (float) vkx::ctx().SwapchainExtent.height, 0.1f, 10.0f);

    ubo.proj[1][1] *= -1;

    g_UniformBuffers[frameIdx]->memcpy(&ubo, sizeof(ubo));
}

void RenderEngine::Render()
{
    VkDevice device = vkx::ctx().Device;
    const int frameIdx = g_CurrentInflightFrame;
    VkCommandBuffer cmdbuf = g_InflightCommandBuffers[frameIdx];

    vkWaitForFences(device, 1, &g_InflightFence[frameIdx], VK_TRUE, UINT64_MAX);

    uint32_t imageIdx;
    vkAcquireNextImageKHR(device, vkx::ctx().SwapchainKHR, UINT64_MAX, g_SemaphoreImageAcquired[frameIdx], nullptr, &imageIdx);
    vkResetFences(device, 1, &g_InflightFence[frameIdx]);
    vkResetCommandBuffer(cmdbuf, 0);

    UpdateUniformBuffer(frameIdx);

    RecordCommandBuffer(cmdbuf, vkx::ctx().SwapchainFramebuffers[imageIdx]);


    vl::QueueSubmit(vkx::ctx().GraphicsQueue, cmdbuf,
                    g_SemaphoreImageAcquired[frameIdx], g_SemaphoreRenderComplete[frameIdx],
                    g_InflightFence[frameIdx]);

    VkResult vkr =
    vl::QueuePresentKHR(vkx::ctx().PresentQueue,
                        1, &g_SemaphoreRenderComplete[frameIdx],
                        1, &vkx::ctx().SwapchainKHR, &imageIdx);

    if (vkr == VK_SUBOPTIMAL_KHR)
    {
        vkx::RecreateSwapchain();
    }

    vkQueueWaitIdle(vkx::ctx().PresentQueue);  // BigWaste on GPU.

    g_CurrentInflightFrame = (g_CurrentInflightFrame + 1) % vkx::INFLIGHT_FRAMES;
}




//static void renderWorldGeometry(World* world)
//{
//
//    // Geometry of Deferred Rendering
//
//    PROFILE("Geo");
//    auto _ap = GeometryRenderer::fboGbuffer->BeginFramebuffer_Scoped();
//
//    RenderCommand::Clear();
//
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);
//
//    glDisable(GL_BLEND);  // Blending is inhabited in Deferred Rendering.
//
//
//    Dbg::dbg_NumEntityRendered = 0;
//    for (Entity* entity : world->getEntities()) {
//        if (!entity || !entity->m_Model) continue;
//        if (entity->m_Model->vertexCount == 0) // most are Empty Chunks.
//            continue;
//
//        // Frustum Culling
//        if (!Ethertia::getCamera().testFrustum(entity->getAABB())) {
//            continue;
//        }
//
//        if (entity == (void*)Ethertia::getPlayer() && Ethertia::getCamera().len == 0)
//            continue;
//
//
//        entity->onRender();
//
//
//        ++Dbg::dbg_NumEntityRendered;
//
//        // Debug: draw Norm/Border
//        if (Dbg::dbg_EntityGeo) {
//            DebugRenderer::Inst().renderDebugGeo(entity->m_Model, entity->position(), entity->getRotation());
//        }
//        // Debug: draw Entity that Crosshair Hits.
//        if (Dbg::dbg_HitPointEntityGeo && entity == Ethertia::getHitCursor().hitEntity) {
//            DebugRenderer::Inst().renderDebugGeo(entity->m_Model, entity->position(), entity->getRotation(),
//                           Ethertia::getHitCursor().brushSize, Ethertia::getHitCursor().position);
//        }
//        // Debug: draw Every Entity AABB.
//        if (Dbg::dbg_RenderedEntityAABB) {
//            AABB aabb = entity->getAABB();
//            Imgui::RenderAABB(aabb, Colors::RED);
//        }
//    }
//
//    glEnable(GL_BLEND);
//
//}


//static void renderWorldCompose(World* world)
//{
//    // Compose of Deferred Rendering
//
//    PROFILE("Cmp");
//    auto _ap = ComposeRenderer::fboCompose->BeginFramebuffer_Scoped();
//
//    glClearColor(0,0,0, 1);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
////    glEnable(GL_FRAMEBUFFER_SRGB);  // built-in Gamma Correction
//
//    float daytime = Ethertia::getWorld()->getDayTime();
//
//    SkyboxRenderer::renderWorldSkybox(daytime);
//
//    ParticleRenderer::renderSunMoonTex(daytime);
//
//    std::vector<Light*> renderLights;  // lights to be render.
////    {
////        static Light l{};
////        l.position = Ethertia::getCamera()->position;
////        l.color = glm::vec3(0.4, 1, 0.8);
////        l.direction = Ethertia::getCamera()->direction;
////        l.attenuation = {0,0,0};
////        renderLights.push_back(&l);
////    }
//    {
//        static Light sunLight;
//
//        // CNS 这是不准确的，不能直接取反 否则月亮位置可能会错误
//        sunLight.position = Ethertia::getCamera().position + (-RenderEngine::SunlightDir(daytime) * 100.0f);
//
//
//        float dayBrightness = 1.0 - abs(daytime-0.5) * 2.0;
//        dayBrightness = 0.4 + dayBrightness * 0.6;
//        glm::vec3 SunColor = Dbg::dbg_WorldSunColor * dayBrightness * Dbg::dbg_WorldSunColorBrightnessMul;
//        sunLight.color = SunColor;
//
//        sunLight.attenuation = {1, 0, 0};
//
//        renderLights.push_back(&sunLight);
//    }
//    //fixme: Do it later
////    for (Entity* e : world->m_Entities)
////    {
////        if (EntityLantern* lat = dynamic_cast<EntityLantern*>(e))
////        {
////            renderLights.push_back(lat->getLights());
////        }
////
////        if (EntityTorch* lat = dynamic_cast<EntityTorch*>(e))
////        {
////            renderLights.push_back(lat->getLights());
////
////            ParticleRenderer::m_Particles.push_back(lat->genParticles());
////        }
////    }
//
//
//    // CNS. 让接下来Alpha!=1.0的地方的颜色 添加到背景颜色中 为了接下来的天空颜色叠加
//    // Blend: addictive color when src.alpha != 1.0, for sky background add.
//    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
//
//    Texture** gbufferTexs = GeometryRenderer::fboGbuffer->texColor;
//    ComposeRenderer::renderCompose(gbufferTexs[0], gbufferTexs[1], gbufferTexs[2],
//                                   SSAORenderer::fboSSAO->texColor[0],
//                                   ShadowRenderer::fboDepthMap->texDepth, ShadowRenderer::matShadowSpace,
//                                   renderLights);
//
//    GlState::blendMode(GlState::ALPHA);
//
//    for (Entity* e : world->getEntities())
//    {
//        // todo: EntityDroppedItem :: onRenderCompose();
//        if (EntityDroppedItem* eDroppedItem = dynamic_cast<EntityDroppedItem*>(e))
//        {
//            const ItemStack& stack = eDroppedItem->m_DroppedItem;
//
//            float i = Item::REGISTRY.getOrderId(stack.item());
//            float n = Item::REGISTRY.size();
//
//            ParticleRenderer::render(ItemTextures::ITEM_ATLAS, eDroppedItem->position(), 0.3f,
//                                       {i/n, 0},
//                                       {1/n, 1});
//
//            if (stack.amount() > 1)
//            {
//                // fixme: tmp draw count.
////                Gui::drawWorldpoint(eDroppedItem->position(), [&](glm::vec2 p) {
////                    Gui::drawString(p.x, p.y, std::to_string(stack.amount()));
////                });
//            }
//        }
//    }
//
//    glEnable(GL_DEPTH_TEST);
//
//    ParticleRenderer::renderAll();
//
//}




// void RenderEngine::RenderWorld()
//    GeometryRenderer::PrepareFrame();
//
//    if (Dbg::dbg_PauseWorldRender) return;
//
//    RenderCommand::CheckError("Begin World Render");
//
//    ParticleRenderer::updateAll(Ethertia::getDelta());
//
//    // Heavy
//    renderWorldGeometry(world);
//
//
//    float CurrTime = Ethertia::getPreciseTime();
//    static float s_LastTimeRenderShadowDepthMap = 0;
//    if ((s_LastTimeRenderShadowDepthMap + Settings::gInterval_ShadowDepthMap) <= CurrTime)
//    {
//        PROFILE("ShadowDepthMap");
//
//        s_LastTimeRenderShadowDepthMap = CurrTime;
//        ShadowRenderer::renderDepthMap(world->m_Entities, SunlightDir(world->getDayTime()), !Settings::g_ShadowMapping);
//    }
//
//    {
//        PROFILE("SSAO");
//
//        SSAORenderer::renderSSAO(GeometryRenderer::fboGbuffer->texColor[0], GeometryRenderer::fboGbuffer->texColor[1], !Settings::g_SSAO);
//    }
//
//
//    renderWorldCompose(world);
//
//
////    static Model* model = Loader::loadModel(100, {
////        {3, pos},
////        {2, uv},
////        {3, norm},
////        {3, joint_ids},
////        {3, joint_weights}});
////    AnimRenderer::Inst()->render();
//
//
//    RenderCommand::CheckError("End World Render");
//}


