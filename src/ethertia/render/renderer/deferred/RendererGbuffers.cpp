//
// Created by Dreamtowards on 2023/5/5.
//

#include <ethertia/init/MaterialTextures.h>

namespace RendererGbuffer
{
    VkRenderPass    g_RenderPass;
    VkFramebuffer   g_Framebuffer;
    VkPipeline      g_Pipeline;

    VkExtent2D g_AttachmentSize = {512, 512};
    vkx::FramebufferAttachment gPosition;
    vkx::FramebufferAttachment gNormal;
    vkx::FramebufferAttachment gAlbedo;
    vkx::FramebufferAttachment gDepth;

    VkDescriptorSetLayout g_DescriptorSetLayout = nullptr;
    VkPipelineLayout g_PipelineLayout = nullptr;

    vkx::UniformBuffer* g_UniformBuffers[vkx::INFLIGHT_FRAMES];
    VkDescriptorSet g_DescriptorSets[vkx::INFLIGHT_FRAMES];


    vkx::VertexBuffer* g_TestModel = nullptr;
    vkx::Image* g_TestImage = nullptr;


    struct UniformBufferObject
    {
        glm::mat4 matProjection;
        glm::mat4 matView;
        // _uniform_align glm::mat4 matModel;
    };

    struct PushConstant_T
    {
        glm::mat4 matModel;
    };

    void init()
    {
        VertexData* vtx = Loader::loadOBJ("entity/viking_room/viking_room.obj");
        g_TestModel = Loader::loadVertexData(vtx);

        BitmapImage img = Loader::loadPNG("entity/viking_room/viking_room.png");
        g_TestImage = Loader::loadTexture(img);


        VkDevice device = vkx::ctx().Device;

        // Uniform Buffers
        for (int i = 0; i < vkx::INFLIGHT_FRAMES; ++i) {
            g_UniformBuffers[i] = new vkx::UniformBuffer(sizeof(UniformBufferObject));
        }

        g_DescriptorSetLayout = vl::CreateDescriptorSetLayout(device, {
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},            // vert UBO
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},  // frag diffuseMap
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},  // frag normMap
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}   // frag dramMap (Disp, Rough, AO, Metal)
        });
        g_PipelineLayout =
        vl::CreatePipelineLayout(device,
                 {{g_DescriptorSetLayout}},
                 {{vl::IPushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushConstant_T))}});

        // DescriptorSet

        std::vector<VkDescriptorSetLayout> layouts(vkx::INFLIGHT_FRAMES, g_DescriptorSetLayout);
        vl::AllocateDescriptorSets(device, vkx::ctx().DescriptorPool, vkx::INFLIGHT_FRAMES, layouts.data(), g_DescriptorSets);

        for (int i = 0; i < vkx::INFLIGHT_FRAMES; ++i)
        {
            vkx::DescriptorWrites writes{g_DescriptorSets[i]};

            writes.UniformBuffer(g_UniformBuffers[i]->buffer(), sizeof(UniformBufferObject));
            writes.CombinedImageSampler(MaterialTextures::ATLAS_DIFFUSE->m_ImageView, vkx::ctx().ImageSampler);
            writes.CombinedImageSampler(MaterialTextures::ATLAS_NORM->m_ImageView, vkx::ctx().ImageSampler);
            writes.CombinedImageSampler(MaterialTextures::ATLAS_DRAM->m_ImageView, vkx::ctx().ImageSampler);

            writes.WriteDescriptorSets(device);
        }

        // RenderPass
        {
            VkFormat rgbFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
            gPosition = vkx::FramebufferAttachment::Create(g_AttachmentSize, rgbFormat);
            gNormal   = vkx::FramebufferAttachment::Create(g_AttachmentSize, rgbFormat);
            gAlbedo   = vkx::FramebufferAttachment::Create(g_AttachmentSize, rgbFormat);
            gDepth    = vkx::FramebufferAttachment::Create(g_AttachmentSize, vkx::findDepthFormat(), true);

            g_RenderPass =
            vl::CreateRenderPass(device, {{
                                         gPosition.AttachmentDescription,
                                         gNormal.AttachmentDescription,
                                         gAlbedo.AttachmentDescription,
                                         gDepth.AttachmentDescription
                                 }},{{vl::IGraphicsSubpass(
                                         {{
                                                  {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},  // gPosition
                                                  {1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},  // gNormal
                                                  {2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},  // gAlbedo
                                         }},
                                         {3, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL}  // gDepth
                                 )}});
        }

        g_Framebuffer =
        vl::CreateFramebuffer(device, g_AttachmentSize,
                              g_RenderPass,{{
                                                            gPosition.Image->m_ImageView,
                                                            gNormal.Image->m_ImageView,
                                                            gAlbedo.Image->m_ImageView,
                                                            gDepth.Image->m_ImageView}});

        g_Pipeline =
        vkx::CreateGraphicsPipeline(
                {{
                         {Loader::loadAssets("shaders-vk/spv/def_gbuffer/vert.spv"), VK_SHADER_STAGE_VERTEX_BIT},
                         {Loader::loadAssets("shaders-vk/spv/def_gbuffer/frag.spv"), VK_SHADER_STAGE_FRAGMENT_BIT},
                         {Loader::loadAssets("shaders-vk/spv/def_gbuffer/tesc.spv"), VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT},
                         {Loader::loadAssets("shaders-vk/spv/def_gbuffer/tese.spv"), VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT}
                 }},
                {
                        VK_FORMAT_R32G32B32_SFLOAT,
                        VK_FORMAT_R32G32_SFLOAT,
                        VK_FORMAT_R32G32B32_SFLOAT
                },
                VK_PRIMITIVE_TOPOLOGY_PATCH_LIST,
                3,
                {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR},
                g_PipelineLayout,
                g_RenderPass,
                3);

    }

    void deinit()
    {
        for (int i = 0; i < std::size(g_UniformBuffers); ++i) {
            delete g_UniformBuffers[i];
        }

        delete gPosition.Image;
        delete gNormal.Image;
        delete gAlbedo.Image;
        delete gDepth.Image;

        delete g_TestImage;
        delete g_TestModel;

    }


    void UpdateUniformBuffer(int frameIdx)
    {
        Camera& cam = Ethertia::getCamera();

        UniformBufferObject ubo{};

        ubo.matProjection = cam.matProjection;
        ubo.matView = cam.matView;

        // GLM was originally designed for OpenGL, where the Y coordinate of the clip coordinates is inverted.
        // The easiest way to compensate for that is to flip the sign on the scaling factor of the Y axis in
        // the projection matrix. If you don't do this, then the image will be rendered upside down.
        ubo.matProjection[1][1] *= -1;

        g_UniformBuffers[frameIdx]->update(&ubo, sizeof(ubo));
    }

    void RecordCommands(VkCommandBuffer cmdbuf, const std::vector<Entity*>& entities)
    {
        UpdateUniformBuffer(vkx::CurrentInflightFrame);

        vkx::CommandBuffer cmd{cmdbuf};
        int frameIdx = vkx::CurrentInflightFrame;

        // Deferred :: Gbuffer

        VkClearValue clearValues[4]{};
        clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};  // gPosition
        clearValues[1].color = {0.0f, 0.0f, 0.0f, 1.0f};  // gNormal
        clearValues[2].color = {0.0f, 0.0f, 0.0f, 1.0f};  // gAlbedo
        clearValues[3].depthStencil = {1.0f, 0};
        VkExtent2D renderExtent = g_AttachmentSize;
        cmd.CmdBeginRenderPass(g_RenderPass, g_Framebuffer, renderExtent, {clearValues, 4});

        cmd.CmdBindGraphicsPipeline(g_Pipeline);
        cmd.CmdSetViewport(renderExtent);
        cmd.CmdSetScissor(renderExtent);

        cmd.CmdBindDescriptorSets(g_PipelineLayout, &g_DescriptorSets[frameIdx]);

        PushConstant_T pushConstant{};

        Dbg::dbg_NumEntityRendered = 0;
        for (Entity* entity : entities)
        {
            vkx::VertexBuffer* vtxbuf = entity->m_Model;

            if (vtxbuf == nullptr) continue;
            assert(vtxbuf->vertexCount() > 0);
            // Frustum Culling
            if (!Ethertia::getCamera().testFrustum(entity->getAABB()))
                continue;
            if (entity == (void*)Ethertia::getPlayer() && Ethertia::getCamera().len == 0)
                continue;


            pushConstant.matModel = Mth::matModel(entity->position());//, entity->getRotation(), {1,1,1});
            cmd.CmdPushConstants(g_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, &pushConstant, sizeof(pushConstant));

            cmd.CmdBindVertexBuffer(vtxbuf->vtxbuffer());
            if (vtxbuf->isIndexed()) {
                cmd.CmdBindIndexBuffer(vtxbuf->idxbuffer());
                cmd.CmdDrawIndexed(vtxbuf->vertexCount());
            } else {
                cmd.CmdDraw(vtxbuf->vertexCount());
            }


            ++Dbg::dbg_NumEntityRendered;
            // Debug: draw Every Entity AABB.
            if (Dbg::dbg_RenderedEntityAABB) {
                Imgui::RenderAABB(entity->getAABB(), Colors::RED);
            }
        }

        cmd.CmdEndRenderPass();
    }

};
