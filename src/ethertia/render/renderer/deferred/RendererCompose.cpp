//
// Created by Dreamtowards on 2023/5/11.
//


// Sun (Day) or Moon (Night).  Noon -> vec3(0, -1, 0)
static glm::vec3 SunlightDir(float daytime) {
    glm::vec3 d = Mth::rot_dir(daytime * 2*Mth::PI, glm::vec3(0, 0, 1), glm::vec3(0, -1, 0));
    return d.y < 0 ? -d : d;
}


namespace RendererCompose
{
    VkRenderPass g_RenderPass = nullptr;
    VkFramebuffer g_Framebuffer = nullptr;

    vkx::FramebufferAttachment g_FramebufferAttachmentColor;
    vkx::FramebufferAttachment g_FramebufferAttachmentDepth;
    VkExtent2D g_AttachmentSize = {512, 512};

    VkPipeline g_Pipeline = nullptr;
    VkPipelineLayout g_PipelineLayout = nullptr;

    VkDescriptorSetLayout g_DescriptorSetLayout = nullptr;
    VkDescriptorSet g_DescriptorSet[vkx::INFLIGHT_FRAMES];
    vkx::UniformBuffer* g_UniformBuffers[vkx::INFLIGHT_FRAMES];

    vkx::Image* gTestCubeMap;

    struct Light
    {
        alignas(16) glm::vec3 position;
        alignas(16) glm::vec3 color;
        alignas(16) glm::vec3 attenuation;

        alignas(16) glm::vec3 direction;
        glm::vec2 coneAngle;
    };
    struct UBO
    {
        alignas(16) glm::vec3 CameraPos;

        alignas(16) glm::mat4 invMatView;
        alignas(16) glm::mat4 invMatProj;

        uint32_t lightCount;
        Light lights[64];

    };

    void init(VkImageView gPosition, VkImageView gNormal, VkImageView gAlbedo)
    {
        BitmapImage imgs[6] = {
                Loader::loadPNG("/Users/dreamtowards/Documents/YouRepository/Ethertia/run/assets/misc/skybox/left.jpg"),
                Loader::loadPNG("/Users/dreamtowards/Documents/YouRepository/Ethertia/run/assets/misc/skybox/right.jpg"),
                Loader::loadPNG("/Users/dreamtowards/Documents/YouRepository/Ethertia/run/assets/misc/skybox/top.jpg"),
                Loader::loadPNG("/Users/dreamtowards/Documents/YouRepository/Ethertia/run/assets/misc/skybox/bottom.jpg"),
                Loader::loadPNG("/Users/dreamtowards/Documents/YouRepository/Ethertia/run/assets/misc/skybox/back.jpg"),
                Loader::loadPNG("/Users/dreamtowards/Documents/YouRepository/Ethertia/run/assets/misc/skybox/front.jpg")
        };
        gTestCubeMap = Loader::loadCubeMap(imgs);

        VkDevice device = vkx::ctx().Device;

        for (int i = 0; i < vkx::INFLIGHT_FRAMES; ++i) {
            g_UniformBuffers[i] = new vkx::UniformBuffer(sizeof(UBO));
        }

        g_DescriptorSetLayout = vl::CreateDescriptorSetLayout(device, {
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT},          // frag UBO
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},  // gPosition
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},  // gNormal
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},  // gAlbedo
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},   // gDRAM
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}   // gDRAM
        });

        g_PipelineLayout =
        vl::CreatePipelineLayout(device, {{g_DescriptorSetLayout}});

        std::vector<VkDescriptorSetLayout> layouts(vkx::INFLIGHT_FRAMES, g_DescriptorSetLayout);
        vl::AllocateDescriptorSets(device, vkx::ctx().DescriptorPool, vkx::INFLIGHT_FRAMES, layouts.data(), g_DescriptorSet);

        for (int i = 0; i < vkx::INFLIGHT_FRAMES; ++i) {
            vkx::DescriptorWrites writes{g_DescriptorSet[i]};

            writes.UniformBuffer(g_UniformBuffers[i]->buffer(), sizeof(UBO));
            writes.CombinedImageSampler(gPosition, vkx::ctx().ImageSampler);
            writes.CombinedImageSampler(gNormal, vkx::ctx().ImageSampler);
            writes.CombinedImageSampler(gAlbedo, vkx::ctx().ImageSampler);
            writes.CombinedImageSampler(gAlbedo, vkx::ctx().ImageSampler);  // todo: DRAM
            writes.CombinedImageSampler(gTestCubeMap->m_ImageView);

            writes.WriteDescriptorSets(device);
        }

        // RenderPass
        g_FramebufferAttachmentColor = vkx::FramebufferAttachment::Create(g_AttachmentSize, VK_FORMAT_R8G8B8A8_UNORM);  // ?SRGB
        g_FramebufferAttachmentDepth = vkx::FramebufferAttachment::Create(g_AttachmentSize, vkx::findDepthFormat(),true);

        g_RenderPass =
        vl::CreateRenderPass(device,{{
                                g_FramebufferAttachmentColor.AttachmentDescription,
                                g_FramebufferAttachmentDepth.AttachmentDescription
                             }},{{
                             vl::IGraphicsSubpass(
                                     {{
                                          {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}
                                     }},
                                     {1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL})
                             }});

        g_Framebuffer =
        vl::CreateFramebuffer(device, g_AttachmentSize,
                              g_RenderPass,{{
                                  g_FramebufferAttachmentColor.Image->m_ImageView,
                                  g_FramebufferAttachmentDepth.Image->m_ImageView
                              }});

        g_Pipeline =
        vkx::CreateGraphicsPipeline({{
                         {Loader::loadAssets("shaders-vk/spv/def_compose/vert.spv"), VK_SHADER_STAGE_VERTEX_BIT},
                         {Loader::loadAssets("shaders-vk/spv/def_compose/frag.spv"), VK_SHADER_STAGE_FRAGMENT_BIT},
                }},
                {},
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                1,
                {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR},
                g_PipelineLayout,
                g_RenderPass);


    }

    void UpdateUniforms()
    {
        Camera& cam = Ethertia::getCamera();

        UBO ubo;
        ubo.CameraPos = cam.position;

        ubo.invMatView = glm::inverse(cam.matView);

        ubo.invMatProj = cam.matProjection;
        ubo.invMatProj[1][1] *= -1;
        ubo.invMatProj = glm::inverse(ubo.invMatProj);

        ubo.lightCount = 1;
//        for (int i = 0; i < 2; ++i) {
//            ubo.lights[i] = {
//                    .position = glm::vec3{10*i, 1, 0} + ubo.CameraPos,
//                    .color = {0.5 * i, 1, 1},
//            };
//        }

        float DayTime = Ethertia::getWorld()->getDayTime();

        float dayBrightness = 1.0 - abs(DayTime-0.5) * 2.0;
        dayBrightness = 0.4 + dayBrightness * 0.6;

        glm::vec3 SunPos = cam.position + SunlightDir(DayTime) * 100.0f;
        glm::vec3 SunColor = Dbg::dbg_WorldSunColor * dayBrightness * Dbg::dbg_WorldSunColorBrightnessMul;

        ubo.lights[0] = {
                .position = SunPos,
                .color = SunColor,
        };

        g_UniformBuffers[vkx::CurrentInflightFrame]->update(&ubo, sizeof(ubo));
    }

    void RecordCommands(VkCommandBuffer cmdbuf)
    {
        UpdateUniforms();

        vkx::CommandBuffer cmd{cmdbuf};
        int frameIdx = vkx::CurrentInflightFrame;

        VkClearValue clearValues[2]{};
        clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        VkExtent2D renderExtent = g_AttachmentSize;
        cmd.CmdBeginRenderPass(g_RenderPass, g_Framebuffer, renderExtent, {clearValues, 2});

        cmd.CmdBindGraphicsPipeline(g_Pipeline);
        cmd.CmdSetViewport(renderExtent);
        cmd.CmdSetScissor(renderExtent);

        cmd.CmdBindDescriptorSets(g_PipelineLayout, &g_DescriptorSet[frameIdx]);
        cmd.CmdDraw(6);

        cmd.CmdEndRenderPass();
    }
}