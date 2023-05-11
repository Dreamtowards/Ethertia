//
// Created by Dreamtowards on 2023/5/5.
//

#include <ethertia/init/MaterialTextures.h>

namespace RendererGbuffer
{
    VkRenderPass    g_RenderPass;
    VkFramebuffer   g_Framebuffer;
    VkPipeline      g_Pipeline;

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
        _uniform_align glm::mat4 matProjection;
        _uniform_align glm::mat4 matView;
        // _uniform_align glm::mat4 matModel;
    };

    struct PushConstant_T
    {
        glm::mat4 matModel;
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

        int attach_size = 512;
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

            g_RenderPass =
            vl::CreateRenderPass(device, {{
                                         gPosition.AttachmentDescription,
                                         gNormal.AttachmentDescription,
                                         gAlbedo.AttachmentDescription,
                                         gDepth.AttachmentDescription
                                 }},
                                 {{
                                     vl::IGraphicsSubpass(
                                         {&attachmentRefs[0], 3},
                                         &attachmentRefs[3])
                                 }});
        }

        g_Framebuffer =
        vl::CreateFramebuffer(device, attach_size, attach_size,
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

        g_UniformBuffers[frameIdx]->memcpy(&ubo, sizeof(ubo));
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
        VkExtent2D renderExtent = {512, 512};
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


namespace RendererCompose
{
    VkRenderPass g_RenderPass = nullptr;
    VkFramebuffer g_Framebuffer = nullptr;
    VkPipeline g_Pipeline = nullptr;
    VkPipelineLayout g_PipelineLayout = nullptr;

    VkDescriptorSetLayout g_DescriptorSetLayout = nullptr;
    VkDescriptorSet g_DescriptorSet[vkx::INFLIGHT_FRAMES];

    void RecordCommands(VkCommandBuffer cmdbuf)
    {
        vkx::CommandBuffer cmd{cmdbuf};
        int frameIdx = vkx::CurrentInflightFrame;



        cmd.CmdBindGraphicsPipeline(g_Pipeline);
        cmd.CmdBindDescriptorSets(g_PipelineLayout, &g_DescriptorSet[frameIdx]);
        cmd.CmdDrawIndexed(6);
    }
}