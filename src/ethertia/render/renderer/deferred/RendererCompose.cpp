//
// Created by Dreamtowards on 2023/5/11.
//




namespace RendererCompose
{
    VkRenderPass g_RenderPass = nullptr;
    VkFramebuffer g_Framebuffer = nullptr;
    VkPipeline g_Pipeline = nullptr;
    VkPipelineLayout g_PipelineLayout = nullptr;

    VkDescriptorSetLayout g_DescriptorSetLayout = nullptr;
    VkDescriptorSet g_DescriptorSet[vkx::INFLIGHT_FRAMES];
    vkx::UniformBuffer* g_UniformBuffers[vkx::INFLIGHT_FRAMES];

    vkx::FramebufferAttachment g_FramebufferAttachmentColor;
    vkx::FramebufferAttachment g_FramebufferAttachmentDepth;

    VkExtent2D g_AttachmentSize = {512, 512};

    struct UBO
    {

    };

    void init(VkImageView gPosition, VkImageView gNormal, VkImageView gAlbedo)
    {
        VkDevice device = vkx::ctx().Device;

        for (int i = 0; i < vkx::INFLIGHT_FRAMES; ++i) {
            g_UniformBuffers[i] = new vkx::UniformBuffer(sizeof(UBO));
        }

        g_DescriptorSetLayout = vl::CreateDescriptorSetLayout(device, {
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},            // frag UBO
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},  // gPosition
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},  // gNormal
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},  // gAlbedo
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
        vkx::CreateGraphicsPipeline(
                {{
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

    void RecordCommands(VkCommandBuffer cmdbuf)
    {
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