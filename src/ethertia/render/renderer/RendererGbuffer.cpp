

#include <vkx/vkx.hpp>

#include <glm/glm.hpp>

#include <vector>

#include <ethertia/util/Loader.h>

class RendererGbuffer
{
public:

	inline static vk::RenderPass RenderPass;
	inline static vk::Framebuffer Framebuffer;

    inline static vkx::GraphicsPipeline* Pipeline;

    inline static std::vector<vkx::UniformBuffer*> g_UniformBuffers_Vert;
    inline static std::vector<vkx::UniformBuffer*> g_UniformBuffers_Frag;

    inline static vkx::VertexBuffer* g_VertexBuf;

    inline static vkx::Image* gPosition;
    inline static vkx::Image* gNormal;
    inline static vkx::Image* gAlbedo;
    inline static vkx::Image* gDepth;

    inline static struct UBO_Vert
    {
        glm::mat4 matProjection;
        glm::mat4 matView;
    } g_uboVert{};

    inline static struct UBO_Frag
    {
        uint32_t MtlTexCap;
        float MtlTexScale = 3.5;
        float MtlTriplanarBlendPow = 6.0f;
        float MtlHeightmapBlendPow = 0.6f;
    } g_uboFrag{};

    struct PushConstant
    {
        glm::mat4 matModel;
    };

	static void Init();

    static void RecordCommand(vk::CommandBuffer cmd);

    static void UpdateUniformBuffer(int fifi);

};

static vk::Extent2D g_AttachmentSize{ 1280, 720 };


void RendererGbuffer::Init()
{
	VKX_CTX_device_allocator;

    g_VertexBuf = Loader::LoadVertexData("entity/cone.obj");

    // RenderPass
    {
        vk::Format rgbFormat = vk::Format::eR16G16B16A16Sfloat;
        vk::Format depthFormat = vkxc.SwapchainDepthImageFormat;

        RenderPass = vkx::CreateRenderPass(
            {
                vkx::IAttachmentDesc(rgbFormat, vk::ImageLayout::eShaderReadOnlyOptimal),
                vkx::IAttachmentDesc(rgbFormat, vk::ImageLayout::eShaderReadOnlyOptimal),
                vkx::IAttachmentDesc(rgbFormat, vk::ImageLayout::eShaderReadOnlyOptimal),
                vkx::IAttachmentDesc(depthFormat, vk::ImageLayout::eDepthStencilAttachmentOptimal),
            },
            vkx::IGraphicsSubpass(
                {
                    vkx::IAttachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal),
                    vkx::IAttachmentRef(1, vk::ImageLayout::eColorAttachmentOptimal),
                    vkx::IAttachmentRef(2, vk::ImageLayout::eColorAttachmentOptimal)
                },
                vkx::IAttachmentRef(3, vk::ImageLayout::eDepthStencilAttachmentOptimal)));


        gPosition = vkx::CreateColorImage(g_AttachmentSize, rgbFormat);
        gNormal = vkx::CreateColorImage(g_AttachmentSize, rgbFormat);
        gAlbedo = vkx::CreateColorImage(g_AttachmentSize, rgbFormat);
        gDepth = vkx::CreateDepthImage(g_AttachmentSize, depthFormat);

        Framebuffer = vkx::CreateFramebuffer(g_AttachmentSize, RenderPass,
            {
                gPosition->imageView,
                gNormal->imageView,
                gAlbedo->imageView,
                gDepth->imageView
            });
    }





    Pipeline = vkx::CreateGraphicsPipeline(
        {
            { Loader::LoadAsset("shaders/def_gbuffer.vert.spv"), vk::ShaderStageFlagBits::eVertex },
            { Loader::LoadAsset("shaders/def_gbuffer.frag.spv"), vk::ShaderStageFlagBits::eFragment }
        }, 
        {
            vk::Format::eR32G32B32Sfloat,
            vk::Format::eR32G32Sfloat,
            vk::Format::eR32G32B32Sfloat,
        }, 
        vkx::CreateDescriptorSetLayout({
            {vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex},            // vert UBO
            {vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment},          // frag UBO
            {vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment},   // frag diffuseMap
            {vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment},   // frag normMap
            {vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment}    // frag dramMap (Disp, Rough, AO, Metal)
        }),
        { vkx::IPushConstantRange(vk::ShaderStageFlagBits::eVertex, sizeof(PushConstant)) },
        {
            .colorBlendAttachments = { vkx::IPipelineColorBlendAttachment(), vkx::IPipelineColorBlendAttachment(), vkx::IPipelineColorBlendAttachment() }
        },
        RenderPass);


    // Uniform Buffers
    for (int i = 0; i < vkxc.InflightFrames; ++i) {
        g_UniformBuffers_Vert.push_back(vkx::CreateUniformBuffer(sizeof(UBO_Vert)));
        g_UniformBuffers_Frag.push_back(vkx::CreateUniformBuffer(sizeof(UBO_Frag)));
    }

    for (int i = 0; i < vkxc.InflightFrames; ++i)
    {
        vkx::WriteDescriptorSet(Pipeline->DescriptorSets[i],
            {
                {.buffer = vkx::IDescriptorBuffer(g_UniformBuffers_Vert[i]) },
                {.buffer = vkx::IDescriptorBuffer(g_UniformBuffers_Frag[i]) },
                {.image = vkx::IDescriptorImage(MaterialTextures::ATLAS_DIFFUSE->imageView) },
                {.image = vkx::IDescriptorImage(MaterialTextures::ATLAS_NORM->imageView) },
                {.image = vkx::IDescriptorImage(MaterialTextures::ATLAS_DRAM->imageView) },
            });
    }

}


void RendererGbuffer::UpdateUniformBuffer(int fifi)
{
    // UBO Vert
    Camera& cam = Ethertia::getCamera();
    g_uboVert.matProjection = cam.matProjection;
    g_uboVert.matView = cam.matView;

    // GLM was originally designed for OpenGL, where the Y coordinate of the clip coordinates is inverted.
    // The easiest way to compensate for that is to flip the sign on the scaling factor of the Y axis in
    // the projection matrix. If you don't do this, then the image will be rendered upside down.
    // g_uboVert.matProjection[1][1] *= -1;

    g_UniformBuffers_Vert[fifi]->Upload(&g_uboVert);

    // UBO Frag
    g_uboFrag.MtlTexCap = Material::REGISTRY.size();
    g_UniformBuffers_Frag[fifi]->Upload(&g_uboFrag);
}

void RendererGbuffer::RecordCommand(vk::CommandBuffer cmdbuf)
{
    int fif_i = vkx::ctx().CurrentInflightFrame;

    vkx::CommandBuffer cmd{cmdbuf};

    cmd.BeginRenderPass(RenderPass, Framebuffer, g_AttachmentSize, 
        {
            vkx::ClearValueColor(),
            vkx::ClearValueColor(),
            vkx::ClearValueColor(),
            vkx::ClearValueDepthStencil()
        });

    cmd.SetViewport({}, g_AttachmentSize);
    cmd.SetScissor({}, g_AttachmentSize);

    cmd.BindDescriptorSets(Pipeline->PipelineLayout, Pipeline->DescriptorSets[fif_i]);

    cmd.BindGraphicsPipeline(Pipeline->Pipeline);

    {
        PushConstant pc;
        pc.matModel = glm::mat4{ 1.0 };

        cmd.PushConstants(Pipeline->PipelineLayout, vk::ShaderStageFlagBits::eVertex, pc);

        cmd.BindVertexBuffers(g_VertexBuf->vertexBuffer);

        if (g_VertexBuf->indexBuffer)
        {
            cmd.BindIndexBuffer(g_VertexBuf->indexBuffer);
            cmd.DrawIndexed(g_VertexBuf->vertexCount);
        } 
        else
        {
            cmd.Draw(g_VertexBuf->vertexCount);
        }
    }

    cmd.EndRenderPass();
}