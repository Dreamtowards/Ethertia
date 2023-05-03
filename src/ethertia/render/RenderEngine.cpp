//
// Created by Dreamtowards on 2022/8/22.
//

#include "RenderEngine.h"


#include <ethertia/world/World.h>
//#include <ethertia/render/deferred/GeometryRenderer.h>
//#include <ethertia/render/deferred/ComposeRenderer.h>
//#include <ethertia/render/sky/SkyGradientRenderer.h>
//#include <ethertia/render/sky/SkyboxRenderer.h>
//#include <ethertia/render/gui/GuiRenderer.h>
//#include <ethertia/render/gui/FontRenderer.h>
//#include <ethertia/render/debug/DebugRenderer.h>
//#include <ethertia/render/particle/ParticleRenderer.h>
//#include <ethertia/render/ssao/SSAORenderer.h>
//#include <ethertia/render/shadow/ShadowRenderer.h>
//#include <ethertia/render/anim/AnimRenderer.h>
//#include <ethertia/render/RenderCommand.h>

#include <ethertia/render/Window.h>
#include <ethertia/entity/EntityDroppedItem.h>
#include <ethertia/init/ItemTextures.h>
#include <ethertia/entity/player/EntityPlayer.h>
#include <ethertia/init/Settings.h>
#include <ethertia/init/DebugStat.h>


// RenderEngine is static/singleton. shouldn't instantiate.
// Don't use OOP except it's necessary.

#include <ethertia/imgui/Imgui.h>
//
//void RenderEngine::init()
//{
//    BENCHMARK_TIMER;
//    Log::info("RenderEngine initializing.\1");
//
//    vkx::Init(Ethertia::getWindow().m_WindowHandle, true);
//
//
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
//    std::cout << "]";
//
//    {
//        BitmapImage img(1, 1, new uint32_t[1]{(uint32_t)~0});
//        TEX_WHITE = Loader::loadImage(img);
//
//        TEX_UVMAP = Loader::loadImage("misc/uvmap.png");
//    }
//
//
////    RenderCommand::CheckError("End of RenderEngine Init");
//
//}

void RenderEngine::deinit()
{
    // todo: deinit renderers.
}



static const int MAX_INFLIGHT_FRAMES = 2;
static int g_CurrentInflightFrame = 0;

static std::vector<VkCommandBuffer> g_InflightCommandBuffers;

static std::vector<VkSemaphore> g_SemaphoreImageAcquired;  // for each InflightFrame   ImageAcquired, RenderComplete
static std::vector<VkSemaphore> g_SemaphoreRenderComplete;
static std::vector<VkFence>     g_InflightFence;


static VkSwapchainKHR        g_SwapchainKHR = nullptr;
static std::vector<VkImage>  g_SwapchainImages;  // auto clean by vk swapchain
static std::vector<VkImageView> g_SwapchainImageViews;
static VkExtent2D            g_SwapchainExtent = {};
static VkFormat              g_SwapchainImageFormat = {};
static std::vector<VkFramebuffer> g_SwapchainFramebuffers;  // for each Swapchain Image.

static VkRenderPass g_RenderPass;

static vkx::Image* g_DepthImage = new vkx::Image(0,0,0,0,0);


static VkDescriptorSetLayout g_DescriptorSetLayout = nullptr;
static VkPipelineLayout g_PipelineLayout = nullptr;
static VkPipeline g_GraphicsPipeline = nullptr;

static std::vector<vkx::UniformBuffer*> g_UniformBuffers;  // for each InflightFrame
static std::vector<VkDescriptorSet> g_DescriptorSets;

static bool g_RecreateSwapchainRequested = false;



#define DECL_unif alignas(16)

struct UniformBufferObject
{
    DECL_unif glm::mat4 model;
    DECL_unif glm::mat4 view;
    DECL_unif glm::mat4 proj;
};



static void CreateCommandBuffers()
{
    g_InflightCommandBuffers.resize(MAX_INFLIGHT_FRAMES);

    vl::AllocateCommandBuffers(vkx::ctx().Device, vkx::ctx().CommandPool,
                               g_InflightCommandBuffers.size(), g_InflightCommandBuffers.data(),
                               VK_COMMAND_BUFFER_LEVEL_PRIMARY);
}



static void CreateSyncObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    g_SemaphoreImageAcquired.resize(MAX_INFLIGHT_FRAMES);
    g_SemaphoreRenderComplete.resize(MAX_INFLIGHT_FRAMES);
    g_InflightFence.resize(MAX_INFLIGHT_FRAMES);

    VkDevice device = vkx::ctx().Device;
    for (int i = 0; i < MAX_INFLIGHT_FRAMES; ++i)
    {
        VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &g_SemaphoreImageAcquired[i]));
        VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &g_SemaphoreRenderComplete[i]));
        VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &g_InflightFence[i]));
    }
}



static void CreateDepthTexture()
{
    vkx::CreateDepthImage(g_SwapchainExtent.width, g_SwapchainExtent.height, g_DepthImage);
}



static void CreateFramebuffers()
{
    g_SwapchainFramebuffers.resize(g_SwapchainImageViews.size());

    for (size_t i = 0; i < g_SwapchainImageViews.size(); i++)
    {
        std::array<VkImageView, 2> attachments = { g_SwapchainImageViews[i], g_DepthImage->m_ImageView };

        g_SwapchainFramebuffers[i] = vl::CreateFramebuffer(vkx::ctx().Device,
                                                           g_SwapchainExtent.width, g_SwapchainExtent.height,
                                                           g_RenderPass, attachments.size(), attachments.data());
    }
}


static void CreateSwapchain()
{
    vkx::CreateSwapchain(vkx::ctx().Device, vkx::ctx().PhysDevice, vkx::ctx().SurfaceKHR, Ethertia::getWindow().m_WindowHandle,
                         g_SwapchainKHR,
                         g_SwapchainExtent, g_SwapchainImageFormat,
                         g_SwapchainImages, g_SwapchainImageViews);
}

static void DestroySwapchain()
{
    VkDevice device = vkx::ctx().Device;

    for (auto fb : g_SwapchainFramebuffers) {
        vkDestroyFramebuffer(device, fb, nullptr);
    }
    for (auto imageview : g_SwapchainImageViews) {
        vkDestroyImageView(device, imageview, nullptr);
    }
    vkDestroySwapchainKHR(device, g_SwapchainKHR, nullptr);
}

static void RecreateSwapchain()
{
    vkDeviceWaitIdle(vkx::ctx().Device);
    DestroySwapchain();
    Log::info("RecreateSwapchain");

    CreateSwapchain();
    CreateDepthTexture();
    CreateFramebuffers();
}


static void CreateRenderPass()
{
    VkAttachmentDescription attachmentsDesc[] = {
            vkh::c_AttachmentDescription(g_SwapchainImageFormat, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR),
            vkh::c_AttachmentDescription(vkx::findDepthFormat(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) // .storeOp: VK_ATTACHMENT_STORE_OP_DONT_CARE
    };

    VkAttachmentReference colorAttachmentRef = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    VkAttachmentReference depthAttachmentRef = {1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcAccessMask = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    g_RenderPass = vl::CreateRenderPass(vkx::ctx().Device,
                         {attachmentsDesc, std::size(attachmentsDesc)},
                         {&subpass, 1},
                         {&dependency, 1});
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
    g_UniformBuffers.resize(MAX_INFLIGHT_FRAMES);

    for (int i = 0; i < MAX_INFLIGHT_FRAMES; ++i)
    {
        g_UniformBuffers[i] = new vkx::UniformBuffer(bufferSize);
    }
}

static void CreateDescriptorSets()
{
    VkDevice device = vkx::ctx().Device;
    std::vector<VkDescriptorSetLayout> layouts(MAX_INFLIGHT_FRAMES, g_DescriptorSetLayout);

    g_DescriptorSets.resize(MAX_INFLIGHT_FRAMES);
    vl::AllocateDescriptorSets(device, vkx::ctx().DescriptorPool, MAX_INFLIGHT_FRAMES, layouts.data(), g_DescriptorSets.data());

    for (int i = 0; i < MAX_INFLIGHT_FRAMES; ++i)
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

    CreateSwapchain();

    {
        BitmapImage img(1, 1, new uint32_t[1]{(uint32_t)~0});
        TEX_WHITE = Loader::loadImage(img);

        TEX_UVMAP = Loader::loadImage("misc/uvmap.png");
    }


    CreateRenderPass();     // depend: Swapchain format
    vkx::ctx()._RenderPass = g_RenderPass;

    CreateDepthTexture();
    CreateFramebuffers();   // depend: DepthTexture, RenderPass

    CreateDescriptorSetLayout();

    g_PipelineLayout = vl::CreatePipelineLayout(vkx::ctx().Device, 1, &g_DescriptorSetLayout);

    CreateUniformBuffers();
    CreateDescriptorSets();

    {

//        VkPipelineShaderStageCreateInfo shaderStages[2];
//        vkh::LoadShaderStages_H(shaderStages, "shaders-vk/spv/def_gbuffer/{}.spv");
//
//        VkVertexInputBindingDescription vertexInputBindingDescription;
//        std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescription;
//
//        std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
//
//        VkPipelineColorBlendAttachmentState colorBlendAttachment = vkh::c_PipelineColorBlendAttachmentState();
//
//        VkGraphicsPipelineCreateInfo pipelineInfo =
//                vl::IGraphicsPipeline(
//                        {shaderStages, 2},
//                        vl::IPipelineVertexInputState(
//                                { VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT }, 0,
//                                &vertexInputBindingDescription, &vertexInputAttributeDescription),
//                        vl::IPipelineInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST),
//                        nullptr,
//                        vl::IPipelineViewportState(),
//                        vl::IPipelineRasterizationState(),
//                        vl::IPipelineMultisampleState(),
//                        vl::IPipelineDepthStencilState(),
//                        vl::IPipelineColorBlendState({&colorBlendAttachment, 1}),
//                        vl::IPipelineDynamicState({dynamicStates.data(), (int)dynamicStates.size()}),
//                        g_PipelineLayout,
//                        g_RenderPass);
//
//        vl::CreateGraphicsPipelines(vkx::ctx().Device, VK_NULL_HANDLE, 1, &g_GraphicsPipeline,
//                   &pipelineInfo);

        g_GraphicsPipeline =
        vkx::CreateGraphicsPipeline(
                {
                    Loader::loadAssets("shaders-vk/spv/def_gbuffer/vert.spv"),
                    Loader::loadAssets("shaders-vk/spv/def_gbuffer/frag.spv")
                },
                { VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT },
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR},
                g_PipelineLayout,
                g_RenderPass);
    }


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
    cmd.CmdBeginRenderPass(g_RenderPass, framebuffer_MainRenderPass, g_SwapchainExtent, 2, clearValues);

    cmd.CmdBindGraphicsPipeline(g_GraphicsPipeline);
////    cmd.CmdBindGraphicsPipeline(g_Deferred_Compose.m_Pipeline);
    cmd.CmdSetViewport(g_SwapchainExtent);
    cmd.CmdSetScissor(g_SwapchainExtent);
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
    ubo.proj = glm::perspective(glm::radians(45.0f), g_SwapchainExtent.width / (float) g_SwapchainExtent.height, 0.1f, 10.0f);

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
    vkAcquireNextImageKHR(device, g_SwapchainKHR, UINT64_MAX, g_SemaphoreImageAcquired[frameIdx], nullptr, &imageIdx);
    vkResetFences(device, 1, &g_InflightFence[frameIdx]);
    vkResetCommandBuffer(cmdbuf, 0);

    UpdateUniformBuffer(frameIdx);

    RecordCommandBuffer(cmdbuf, g_SwapchainFramebuffers[imageIdx]);


    vl::QueueSubmit(vkx::ctx().GraphicsQueue, cmdbuf,
                    g_SemaphoreImageAcquired[frameIdx], g_SemaphoreRenderComplete[frameIdx],
                    g_InflightFence[frameIdx]);

    vl::QueuePresentKHR(vkx::ctx().PresentQueue,
                        1, &g_SemaphoreRenderComplete[frameIdx],
                        1, &g_SwapchainKHR, &imageIdx);



    if (g_RecreateSwapchainRequested) {
        g_RecreateSwapchainRequested = false;
        RecreateSwapchain();
    }

//         vkQueueWaitIdle(g_PresentQueue);  // BigWaste on GPU.

    g_CurrentInflightFrame = (g_CurrentInflightFrame + 1) % MAX_INFLIGHT_FRAMES;
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


