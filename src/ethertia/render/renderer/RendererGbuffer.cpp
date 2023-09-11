

#include <vkx/vkx.hpp>

#include <glm/glm.hpp>

#include <vector>

#include <ethertia/util/Loader.h>

class RendererGbuffer
{
public:

	static vk::RenderPass RenderPass;
	static vk::Framebuffer Framebuffer;

	static vkx::GraphicsPipeline* Pipeline;

    static std::vector<vkx::UniformBuffer*> g_UniformBuffers_Vert;
    static std::vector<vkx::UniformBuffer*> g_UniformBuffers_Frag;

    struct UBO_Vert
    {
        glm::mat4 matProjection;
        glm::mat4 matView;
    } g_uboVert{};

    struct UBO_Frag
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

};


struct RenderTarget
{
    vkx::Image* image;

    vk::AttachmentDescription attachmentDesc;
};


RenderTarget CreateRenderTarget(vk::Extent2D wh, vk::Format format, bool depth = false)
{
    RenderTarget out{};

    vkx::Image* img = new vkx::Image();
    
    img->image = vkx::CreateImage(wh.width, wh.height, img->imageMemory, img->format = format, 
        depth ? vk::ImageUsageFlagBits::eDepthStencilAttachment : vk::ImageUsageFlagBits::eColorAttachment);

    img->imageView = vkx::CreateImageView(img->image, img->format,
        depth ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor);

    out.image = img;

    out.attachmentDesc = vkx::IAttachmentDesc(img->format, depth ? vk::ImageLayout::eDepthStencilAttachmentOptimal : vk::ImageLayout::eShaderReadOnlyOptimal);

    return out;
}

static vk::Extent2D g_AttachmentSize{ 1280, 720 };


static vkx::Image* gPosition;
static vkx::Image* gNormal;
static vkx::Image* gAlbedo;
static vkx::Image* gDepth;


void RendererGbuffer::Init()
{
	VKX_CTX_device_allocator;



    // RenderPass
    {
        vk::Format rgbFormat = vk::Format::eR16G16B16A16Sfloat;
        vk::Format depthFormat = vkxc.SwapchainDepthImageFormat;

        gPosition = vkx::CreateColorImage(g_AttachmentSize, rgbFormat);
        gNormal = vkx::CreateColorImage(g_AttachmentSize, rgbFormat);
        gAlbedo = vkx::CreateColorImage(g_AttachmentSize, rgbFormat);
        gDepth = vkx::CreateDepthImage(g_AttachmentSize, depthFormat);

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
            { Loader::LoadAsset("shaders/gbuffer/vert.spv"), vk::ShaderStageFlagBits::eVertex },
            { Loader::LoadAsset("shaders/gbuffer/frag.spv"), vk::ShaderStageFlagBits::eFragment }
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
        // {vkx::IPushConstantRange(vk::ShaderStageFlagBits::eVertex, sizeof(PushConstant))}
        {

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