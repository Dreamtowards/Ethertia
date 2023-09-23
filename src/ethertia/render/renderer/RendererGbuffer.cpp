

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

    static void RecordCommand(vk::CommandBuffer cmd, const std::vector<Entity*>& _Entities);

    static void UpdateUniformBuffer(int fifi);

};

static vk::Extent2D g_AttachmentSize{ 1280, 720 };


void RendererGbuffer::Init()
{
	VKX_CTX_device_allocator;

    g_VertexBuf = Loader::LoadVertexData("entity/gravestone/mesh.obj");

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
    //g_uboVert.matProjection[1][1] *= -1;

    g_UniformBuffers_Vert[fifi]->Upload(&g_uboVert);

    // UBO Frag
    g_uboFrag.MtlTexCap = Material::REGISTRY.size();
    g_UniformBuffers_Frag[fifi]->Upload(&g_uboFrag);
}

//void RendererGbuffer::RecordCommand(vk::CommandBuffer cmdbuf, const std::vector<Entity*>& _Entities)
//{
//    int fif_i = vkx::ctx().CurrentInflightFrame;
//
//    UpdateUniformBuffer(fif_i);
//
//    vkx::CommandBuffer cmd{cmdbuf};
//
//    cmd.BeginRenderPass(RenderPass, Framebuffer, g_AttachmentSize, 
//        {
//            vkx::ClearValueColor(0, 0.3, 0),
//            vkx::ClearValueColor(),
//            vkx::ClearValueColor(),
//            vkx::ClearValueDepthStencil()
//        });
//
//    cmd.SetViewport({}, g_AttachmentSize);
//    cmd.SetScissor({}, g_AttachmentSize);
//
//    cmd.BindDescriptorSets(Pipeline->PipelineLayout, Pipeline->DescriptorSets[fif_i]);
//
//    cmd.BindGraphicsPipeline(Pipeline->Pipeline);
//
//    PushConstant pc{};
//
//    for (Entity* entity : _Entities)
//    {
//        vkx::VertexBuffer* vtx = entity->m_Model;
//        if (vtx == nullptr)
//            continue;
//        //// Frustum Culling
//        //if (!Ethertia::getCamera().testFrustum(entity->getAABB()))
//        //    continue;
//        //if (entity == (void*)Ethertia::getPlayer() && Ethertia::getCamera().len == 0)
//        //    continue;
//
//        pc.matModel = Maths::matModel(entity->position());
//
//        cmd.PushConstants(Pipeline->PipelineLayout, vk::ShaderStageFlagBits::eVertex, pc);
//
//
//
//        cmd.BindVertexBuffers(vtx->vertexBuffer);
//
//        if (vtx->indexBuffer)
//        {
//            cmd.BindIndexBuffer(vtx->indexBuffer);
//            cmd.DrawIndexed(vtx->vertexCount);
//        } 
//        else
//        {
//            cmd.Draw(vtx->vertexCount);
//        }
//    }
//
//    cmd.EndRenderPass();
//}



































class RendererCompose
{
public:

    inline static vk::RenderPass RenderPass;
    inline static vk::Framebuffer Framebuffer;

    inline static vkx::Image* rtColor = nullptr;
    inline static vkx::Image* rtDepth = nullptr;

    inline static vkx::GraphicsPipeline* Pipeline = nullptr;

    inline static std::vector<vkx::UniformBuffer*> g_UniformBuffers;


    struct Light
    {
        alignas(16) glm::vec3 position;
        alignas(16) glm::vec3 color;
        alignas(16) glm::vec3 attenuation;

        alignas(16) glm::vec3 direction;
        glm::vec2 coneAngle;
    };
    inline static struct UBO
    {
        alignas(16) glm::vec3 CameraPos;

        alignas(16) glm::mat4 invMatView;
        alignas(16) glm::mat4 invMatProj;

        uint32_t lightCount;
        Light lights[64];

    } g_UBO;

    static void Init(vk::ImageView gPosition, vk::ImageView gNormal, vk::ImageView gAlbedo);

    static void RecordCommand(vk::CommandBuffer cmd);

    static void UpdateUniformBuffer(int fifi);
};


void RendererCompose::Init(vk::ImageView gPosition, vk::ImageView gNormal, vk::ImageView gAlbedo)
{
    VKX_CTX_device_allocator;

    // RenderPass
    rtColor = vkx::CreateColorImage(g_AttachmentSize, vk::Format::eR8G8B8A8Unorm);
    rtDepth = vkx::CreateDepthImage(g_AttachmentSize, vkxc.SwapchainDepthImageFormat);

    RenderPass = vkx::CreateRenderPass(
        {
            vkx::IAttachmentDesc(rtColor->format, vk::ImageLayout::eShaderReadOnlyOptimal),
            vkx::IAttachmentDesc(rtDepth->format, vk::ImageLayout::eDepthStencilAttachmentOptimal),
        },
        vkx::IGraphicsSubpass(
            vkx::IAttachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal),
            vkx::IAttachmentRef(1, vk::ImageLayout::eDepthStencilAttachmentOptimal)
        ));

    Framebuffer = vkx::CreateFramebuffer(g_AttachmentSize, RenderPass,
        {
            rtColor->imageView,
            rtDepth->imageView
        });

    // Pipeline, Descriptor

    Pipeline = vkx::CreateGraphicsPipeline(
        {
            {Loader::LoadAsset("shaders/def_compose.vert.spv"), vk::ShaderStageFlagBits::eVertex},
            {Loader::LoadAsset("shaders/def_compose.frag.spv"), vk::ShaderStageFlagBits::eFragment},
        },
        {},
        vkx::CreateDescriptorSetLayout({
            {vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eFragment},           // frag UBO
            {vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment},    // gPosition
            {vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment},    // gNormal
            {vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment},    // gAlbedo
            {vk::DescriptorType::eCombinedImageSampler, vk::ShaderStageFlagBits::eFragment}     // gDRAM
        }),
        {},
        {},
        RenderPass
    );


    for (int i = 0; i < vkxc.InflightFrames; ++i) {
        g_UniformBuffers.push_back(vkx::CreateUniformBuffer(sizeof(UBO)));
    }

    for (int i = 0; i < vkxc.InflightFrames; ++i) 
    {
        vkx::WriteDescriptorSet(Pipeline->DescriptorSets[i],
            {
                { .buffer = vkx::IDescriptorBuffer(g_UniformBuffers[i]) },
                { .image = vkx::IDescriptorImage(gPosition) },
                { .image = vkx::IDescriptorImage(gNormal) },
                { .image = vkx::IDescriptorImage(gAlbedo) },
                { .image = vkx::IDescriptorImage(gAlbedo) }  // todo DRAM
            });
    }


}

void RendererCompose::UpdateUniformBuffer(int fifi)
{
    UBO& ubo = g_UBO;
    Camera& cam = Ethertia::getCamera();

    ubo.CameraPos = cam.position;

    ubo.invMatView = glm::inverse(cam.matView);
    ubo.invMatProj = glm::inverse(cam.matProjection);

    //ubo.invMatProj = cam.matProjection;
    //ubo.invMatProj[1][1] *= -1;
    //ubo.invMatProj = glm::inverse(g_UBO.invMatProj);

    ubo.lightCount = 1;

    ubo.lights[0] = {
            .position = ubo.invMatView[3],
            .color = {3,2,1},
            .attenuation = {0.3, 0.1, 0.01}
    };


    g_UniformBuffers[fifi]->Upload(&g_UBO);

}


void RendererCompose::RecordCommand(vk::CommandBuffer cmdbuf)
{
    int fif_i = vkx::ctx().CurrentInflightFrame;

    UpdateUniformBuffer(fif_i);

    vkx::CommandBuffer cmd{ cmdbuf };

    cmd.BeginRenderPass(RenderPass, Framebuffer, g_AttachmentSize,
        {
            vkx::ClearValueColor(0, 0, 0.2f),
            vkx::ClearValueDepthStencil()
        });

    cmd.SetViewport({}, g_AttachmentSize);
    cmd.SetScissor({}, g_AttachmentSize);

    cmd.BindDescriptorSets(Pipeline->PipelineLayout, Pipeline->DescriptorSets[fif_i]);

    cmd.BindGraphicsPipeline(Pipeline->Pipeline);

    cmd.Draw(6);

    cmd.EndRenderPass();
}