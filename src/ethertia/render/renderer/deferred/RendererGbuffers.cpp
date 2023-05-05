//
// Created by Dreamtowards on 2023/5/5.
//


namespace RendererGbuffer
{
    VkPipeline      g_Pipeline;
    VkRenderPass    g_RenderPass;
    VkFramebuffer   g_Framebuffer;

    struct FramebufferAttachment   // aka RenderTarget
    {
        vkx::Image* Image;
        VkAttachmentDescription AttachmentDescription;

        static FramebufferAttachment Create(int w, int h, VkFormat format, bool depth = false)
        {
            FramebufferAttachment out{};
            out.Image = new vkx::Image(0,0,0,0,0);
            VkDevice device = vkx::ctx().Device;

            vl::CreateImage(device, w, h, &out.Image->m_Image, &out.Image->m_ImageMemory, format,
                            depth ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);

            out.Image->m_ImageView =
                    vl::CreateImageView(device, out.Image->m_Image, format, depth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT);

            out.AttachmentDescription =
                    vl::IAttachmentDescription(format, depth ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            return out;
        }
    };
    FramebufferAttachment gPosition;
    FramebufferAttachment gNormal;
    FramebufferAttachment gAlbedo;
    FramebufferAttachment gDepth;

    VkDescriptorSetLayout g_DescriptorSetLayout = nullptr;
    VkPipelineLayout g_PipelineLayout = nullptr;

    vkx::UniformBuffer* g_UniformBuffers[vkx::INFLIGHT_FRAMES];
    VkDescriptorSet g_DescriptorSets[vkx::INFLIGHT_FRAMES];


    vkx::VertexBuffer* g_TestModel = nullptr;
    vkx::Image* g_TestImage = nullptr;


    struct UniformBufferObject
    {
        _uniform_align glm::mat4 model;
        _uniform_align glm::mat4 view;
        _uniform_align glm::mat4 proj;
    };

    void init()
    {
        VertexData* vtx = Loader::loadOBJ("entity/viking_room/viking_room.obj");
        g_TestModel = Loader::loadVertexBuffer(vtx);

        BitmapImage img = Loader::loadPNG("entity/viking_room/viking_room.png");
        g_TestImage = Loader::loadImage(img);


        VkDevice device = vkx::ctx().Device;

        // Uniform Buffers
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);
        for (int i = 0; i < vkx::INFLIGHT_FRAMES; ++i)
        {
            g_UniformBuffers[i] = new vkx::UniformBuffer(bufferSize);
        }

        g_DescriptorSetLayout = vl::CreateDescriptorSetLayout(device, {
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
        });
        g_PipelineLayout = vl::CreatePipelineLayout(device, 1, &g_DescriptorSetLayout);

        // DescriptorSet

        std::vector<VkDescriptorSetLayout> layouts(vkx::INFLIGHT_FRAMES, g_DescriptorSetLayout);
        vl::AllocateDescriptorSets(device, vkx::ctx().DescriptorPool, vkx::INFLIGHT_FRAMES, layouts.data(), g_DescriptorSets);

        for (int i = 0; i < vkx::INFLIGHT_FRAMES; ++i)
        {
            vkx::DescriptorWrites writes{g_DescriptorSets[i]};

            writes.UniformBuffer(g_UniformBuffers[i]->buffer(), sizeof(UniformBufferObject));
            writes.CombinedImageSampler(g_TestImage->m_ImageView, vkx::ctx().ImageSampler);

            writes.WriteDescriptorSets(device);
        }

        int attach_size = 1024;
        // RenderPass
        {
            VkFormat rgbFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
            gPosition = FramebufferAttachment::Create(attach_size,attach_size, rgbFormat);
            gNormal   = FramebufferAttachment::Create(attach_size,attach_size, rgbFormat);
            gAlbedo   = FramebufferAttachment::Create(attach_size,attach_size, rgbFormat);
            gDepth    = FramebufferAttachment::Create(attach_size,attach_size, vkx::findDepthFormat(), true);

            VkAttachmentReference attachmentRefs[] = {
                    {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},  // gPosition
                    {1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},  // gNormal
                    {2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},  // gAlbedo
                    {3, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}  // gDepth
            };
            VkSubpassDescription subpass = vl::IGraphicsSubpass(
                    {&attachmentRefs[0], 3},
                    &attachmentRefs[3]);

            g_RenderPass =
                    vl::CreateRenderPass(device, {
                                                 gPosition.AttachmentDescription,
                                                 gNormal.AttachmentDescription,
                                                 gAlbedo.AttachmentDescription,
                                                 gDepth.AttachmentDescription
                                         },
                                         {&subpass, 1});
        }


        g_Framebuffer =
        vl::CreateFramebuffer(device, attach_size, attach_size,
                              g_RenderPass,{{
                                                            gPosition.Image->m_ImageView,
                                                            gNormal.Image->m_ImageView,
                                                            gAlbedo.Image->m_ImageView,
                                                            gDepth.Image->m_ImageView}});


        g_Pipeline =
        vkx::CreateGraphicsPipeline({
                                            Loader::loadAssets("shaders-vk/spv/def_gbuffer/vert.spv"),
                                            Loader::loadAssets("shaders-vk/spv/def_gbuffer/frag.spv")
                                    },
                                    {
                                            VK_FORMAT_R32G32B32_SFLOAT,
                                            VK_FORMAT_R32G32_SFLOAT,
                                            VK_FORMAT_R32G32B32_SFLOAT
                                    },
                                    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                    3,
                                    {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR},
                                    g_PipelineLayout,
                                    g_RenderPass);

    }

    void deinit()
    {
        for (int i = 0; i < std::size(g_UniformBuffers); ++i) {
            delete g_UniformBuffers[i];
        }
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

    void RecordCommands(VkCommandBuffer cmdbuf)
    {

        vkx::CommandBuffer cmd{cmdbuf};
        int frameIdx = vkx::CurrentInflightFrame;

        // Deferred :: Gbuffer

        VkClearValue clearValues[4]{};
        clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};  // gPosition
        clearValues[1].color = {0.0f, 0.0f, 0.0f, 1.0f};  // gNormal
        clearValues[2].color = {0.0f, 0.0f, 0.0f, 1.0f};  // gAlbedo
        clearValues[3].depthStencil = {1.0f, 0};
        VkExtent2D renderExtent = {1024, 1024};
        cmd.CmdBeginRenderPass(g_RenderPass, g_Framebuffer, renderExtent,
                               4, clearValues);

        cmd.CmdBindGraphicsPipeline(g_Pipeline);
        cmd.CmdSetViewport(renderExtent);
        cmd.CmdSetScissor(renderExtent);

        cmd.CmdBindDescriptorSets(g_PipelineLayout, &g_DescriptorSets[frameIdx]);

        cmd.CmdBindVertexBuffer(g_TestModel->vtxbuffer());
        cmd.CmdBindIndexBuffer(g_TestModel->idxbuffer());
        cmd.CmdDrawIndexed(g_TestModel->vertexCount());

        cmd.CmdEndRenderPass();



        // Deferred :: Compose

//    cmd.CmdBindGraphicsPipeline(g_Deferred_Compose.m_Pipeline);
//    cmd.CmdBindDescriptorSets(g_PipelineLayout, &g_DescriptorSets[g_CurrentInflightFrame]);
//    cmd.CmdDrawIndexed(6);

    }

};
