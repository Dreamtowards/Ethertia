//
// Created by Dreamtowards on 2023/3/16.
//


#include <map>
#include <set>
#include <chrono>
#include <array>

#include <ethertia/util/Loader.h>
#include <ethertia/util/Collections.h>
#include <ethertia/util/Log.h>

#include <glm/glm.hpp>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>


#include "VulkanIntl.h"

#include <eldaria/imgui/Imgui.h>
#include <eldaria/vulkan/vkh.cpp>


vkx::VertexBuffer* g_TestModel = nullptr;


#define DECL_unif alignas(16)

struct UniformBufferObject
{
    DECL_unif glm::mat4 model;
    DECL_unif glm::mat4 view;
    DECL_unif glm::mat4 proj;
};



class VulkanIntl_Impl
{
public:

//    inline static VkInstance        g_Instance  = nullptr;
//    inline static VkPhysicalDevice  g_PhysDevice= nullptr;
//    inline static VkDevice          g_Device    = nullptr;  // Logical Device

    inline static VkRenderPass      g_RenderPass = nullptr;

//    inline static VkQueue g_GraphicsQueue = nullptr;
//    inline static VkQueue g_PresentQueue = nullptr;  // Surface Present

//    inline static VkSurfaceKHR          g_SurfaceKHR = nullptr;
    inline static VkSwapchainKHR        g_SwapchainKHR = nullptr;
    inline static std::vector<VkImage>  g_SwapchainImages;  // auto clean by vk swapchain
    inline static std::vector<VkImageView> g_SwapchainImageViews;
    inline static VkExtent2D            g_SwapchainExtent = {};
    inline static VkFormat              g_SwapchainImageFormat = {};
    inline static std::vector<VkFramebuffer> g_SwapchainFramebuffers;  // for each Swapchain Image.

    inline static bool g_RecreateSwapchainRequested = false;  // when Window/Framebuffer resized.
    inline static GLFWwindow* g_WindowHandle = nullptr;

    inline static VkDescriptorSetLayout g_DescriptorSetLayout = nullptr;
    inline static VkPipelineLayout  g_PipelineLayout = nullptr;
    inline static VkPipeline        g_GraphicsPipeline = nullptr;

//    inline static VkCommandPool g_CommandPool = nullptr;
    inline static std::vector<VkCommandBuffer> g_CommandBuffers;
//    inline static VkDescriptorPool g_DescriptorPool = nullptr;

    inline static std::vector<VkSemaphore> g_SemaphoreImageAcquired;  // for each InflightFrame   ImageAcquired, RenderComplete
    inline static std::vector<VkSemaphore> g_SemaphoreRenderComplete;
    inline static std::vector<VkFence>     g_InflightFence;

    inline static const int MAX_FRAMES_INFLIGHT = 2;
    inline static int g_CurrentFrameInflight = 0;

    inline static std::vector<vkx::UniformBuffer*> g_UniformBuffers;  // for each InflightFrame

    inline static std::vector<VkDescriptorSet> g_DescriptorSets;  // for each InflightFrame

    inline static vkx::Image* g_DepthImage = new vkx::Image(0,0,0, 0,0);;
    inline static vkx::Image* g_TextureImage;





    static void Init(GLFWwindow* glfwWindow)
    {
        vkx::Init(glfwWindow, true);

        CreateCommandBuffers();
        CreateSyncObjects_Semaphores_Fences();


        vkx::CreateSwapchain();
        CreateRenderPass();     // depend: Swapchain format
        CreateDepthTexture();
        CreateFramebuffers();   // depend: DepthTexture, RenderPass

        CreateDescriptorSetLayout();

        g_PipelineLayout = vl::CreatePipelineLayout(vkx::ctx().Device, 1, &g_DescriptorSetLayout);
//        CreateGraphicsPipeline();  // depend: RenderPass, DescriptorSetLayout

        {
            BitmapImage bitmapImage = Loader::loadPNG("./assets/entity/viking_room/viking_room.png");

            g_TextureImage = Loader::loadImage(bitmapImage);

            VertexData* vtx = Loader::loadOBJ("./assets/entity/viking_room/viking_room.obj");
            g_TestModel = Loader::loadVertexBuffer(vtx);
        }

        CreateUniformBuffers();
        CreateDescriptorSets();


        InitDeferredRendering();

    }

    static void Destroy()
    {
        DestroySwapchain();

        delete g_DepthImage;
        delete g_TextureImage;
        delete g_TestModel;

        VkDevice device = vkx::ctx().Device;

        vkDestroyPipelineLayout(device, g_Deferred_Compose.m_PipelineLayout, nullptr);
        vkDestroyPipeline(device, g_Deferred_Compose.m_Pipeline, nullptr);

        vkDestroyDescriptorSetLayout(device, g_Deferred_Compose.m_DescSetLayout, nullptr);
        vkDestroyRenderPass(device, g_Deferred_Gbuffer.m_RenderPass, nullptr);
        vkDestroyPipeline(device, g_Deferred_Gbuffer.m_Pipeline, nullptr);
        vkDestroyFramebuffer(device, g_Deferred_Gbuffer.m_Framebuffer, nullptr);
        delete g_Deferred_Gbuffer.gPosition.m_Img;
        delete g_Deferred_Gbuffer.gNormal.m_Img;
        delete g_Deferred_Gbuffer.gAlbedo.m_Img;
        delete g_Deferred_Gbuffer.gDepth.m_Img;



        vkDestroyDescriptorSetLayout(device, g_DescriptorSetLayout, nullptr);
        for (int i = 0; i < MAX_FRAMES_INFLIGHT; ++i) {
            delete g_UniformBuffers[i];
        }

        vkDestroyPipeline(device, g_GraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(device, g_PipelineLayout, nullptr);
        vkDestroyRenderPass(device, g_RenderPass, nullptr);

        for (int i = 0; i < MAX_FRAMES_INFLIGHT; ++i) {
            vkDestroySemaphore(device, g_SemaphoreImageAcquired[i], nullptr);
            vkDestroySemaphore(device, g_SemaphoreRenderComplete[i], nullptr);
            vkDestroyFence(device, g_InflightFence[i], nullptr);
        }

        vkx::Destroy();
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
        int w, h;
        glfwGetFramebufferSize(g_WindowHandle, &w, &h);
        while (w==0 || h==0) {
            glfwGetFramebufferSize(g_WindowHandle, &w, &h);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(vkx::ctx().Device);
        DestroySwapchain();
        Log::info("RecreateSwapchain");

        vkx::CreateSwapchain();
        CreateDepthTexture();
        CreateFramebuffers();
    }

    static void CreateDepthTexture()
    {
        vkx::CreateDepthImage(g_SwapchainExtent.width, g_SwapchainExtent.height, g_DepthImage);
    }





    // aka RenderTarget in DX12
    struct FramebufferAttachment
    {
        vkx::Image* m_Img;
        VkAttachmentDescription m_Desc;
    };
    inline static struct
    {
        VkRenderPass m_RenderPass;
        VkFramebuffer m_Framebuffer;
        VkPipeline m_Pipeline;
        int m_Width;
        int m_Height;

        FramebufferAttachment gPosition;
        FramebufferAttachment gNormal;
        FramebufferAttachment gAlbedo;
        FramebufferAttachment gDepth;

        struct
        {
            glm::mat4 matModel;
            glm::mat4 matView;
            glm::mat4 matProjection;
        } UBO_VS;
    } g_Deferred_Gbuffer;

    inline static struct
    {
        VkPipeline m_Pipeline;

        VkPipelineLayout m_PipelineLayout;
        VkDescriptorSet m_DescSet;
        VkDescriptorSetLayout m_DescSetLayout;

    } g_Deferred_Compose;






    static FramebufferAttachment CreateFramebufferAttachment(int w, int h, VkFormat format, bool depth = false)
    {
        FramebufferAttachment out{};
        VkDevice device = vkx::ctx().Device;

        out.m_Img = new vkx::Image(0,0,0,0,0);
        vl::CreateImage(device, w, h, &out.m_Img->m_Image, &out.m_Img->m_ImageMemory, format,
                    depth ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);

        out.m_Img->m_ImageView = vl::CreateImageView(device, out.m_Img->m_Image, format,
                    depth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT);

        out.m_Desc = vl::IAttachmentDescription(format,
                    depth ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        return out;
    }


    // Init Deferred Rendering. not use Subpass. use isolated RenderPass
    // 1. Create Gbuffer Attachments [Image, Memory, View] for gPosition, gNormal, gAlbedo
    // 2. Create Gbuffer RenderPass
    // 3. Create Gbuffer Framebuffer
    // 4. Create Gbuffer Pipeline
    static void InitDeferredRendering()
    {
        VkDevice device = vkx::ctx().Device;

        // Gbuffer Attachments
        int attach_size = 1024;
        VkFormat rgbFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
        g_Deferred_Gbuffer.gPosition = CreateFramebufferAttachment(attach_size,attach_size, rgbFormat);
        g_Deferred_Gbuffer.gNormal   = CreateFramebufferAttachment(attach_size,attach_size, rgbFormat);
        g_Deferred_Gbuffer.gAlbedo   = CreateFramebufferAttachment(attach_size,attach_size, rgbFormat);
        g_Deferred_Gbuffer.gDepth    = CreateFramebufferAttachment(attach_size,attach_size, vkx::findDepthFormat(), true);

        VkAttachmentDescription attachmentDesc[] = {
                g_Deferred_Gbuffer.gPosition.m_Desc,
                g_Deferred_Gbuffer.gNormal.m_Desc,
                g_Deferred_Gbuffer.gAlbedo.m_Desc,
                g_Deferred_Gbuffer.gDepth.m_Desc
        };

        // Gbuffer RenderPass

        VkAttachmentReference colorAttachmentRefs[] = {
                {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},  // gPosition
                {1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},  // gNormal
                {2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},  // gAlbedo
        };
        VkAttachmentReference depthAttachmentRef =
                {3, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};  // gDepth

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = std::size(colorAttachmentRefs);
        subpass.pColorAttachments = colorAttachmentRefs;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcAccessMask = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        g_Deferred_Gbuffer.m_RenderPass =
        vl::CreateRenderPass(device,
                             {attachmentDesc, std::size(attachmentDesc)},
                             {&subpass, 1},
                             {&dependency, 1});

        // Gbuffer Framebuffer

        VkImageView attachmentViews[] = {
                g_Deferred_Gbuffer.gPosition.m_Img->m_ImageView,
                g_Deferred_Gbuffer.gNormal.m_Img->m_ImageView,
                g_Deferred_Gbuffer.gAlbedo.m_Img->m_ImageView,
                g_Deferred_Gbuffer.gDepth.m_Img->m_ImageView,
        };

        g_Deferred_Gbuffer.m_Framebuffer = vl::CreateFramebuffer(device,
                                                                 attach_size, attach_size,
                                                                 g_Deferred_Gbuffer.m_RenderPass,
                                                                 std::size(attachmentViews), attachmentViews);


        // Gbuffer Pipeline
//
//        VkPipelineVertexInputStateCreateInfo vertexInputState = vkh::c_PipelineVertexInputState_H({
//            VK_FORMAT_R32G32B32_SFLOAT, // in_pos
//            VK_FORMAT_R32G32_SFLOAT,    // in_tex
//            VK_FORMAT_R32G32B32_SFLOAT  // in_norm
//        });
//        VkPipelineInputAssemblyStateCreateInfo inputAssembly = vkh::c_PipelineInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
//        VkPipelineViewportStateCreateInfo viewportState = vkh::c_PipelineViewportState(1, 1);
//        VkPipelineRasterizationStateCreateInfo rasterizer = vkh::c_PipelineRasterizationState();  // VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE
//        VkPipelineMultisampleStateCreateInfo multisampling = vkh::c_PipelineMultisampleState();
//        VkPipelineDepthStencilStateCreateInfo depthStencil = vkh::c_PipelineDepthStencilState();
//        VkPipelineDynamicStateCreateInfo dynamicState = vkh::c_PipelineDynamicState_H_ViewportScissor();
//
//        // for gPosition, gNormal, gAlbedo, if no, as color mask = 0x0, black will be render.
//        VkPipelineColorBlendAttachmentState colorBlendAttachments[] = {
//                vkh::c_PipelineColorBlendAttachmentState(),
//                vkh::c_PipelineColorBlendAttachmentState(),
//                vkh::c_PipelineColorBlendAttachmentState()
//        };
//        VkPipelineColorBlendStateCreateInfo colorBlending = vkh::c_PipelineColorBlendState(3, colorBlendAttachments);
//
//        VkPipelineShaderStageCreateInfo shaderStages[2];
//        vkh::LoadShaderStages_H(shaderStages, "shaders-vk/spv/def_gbuffer/{}.spv");
//
//        VkGraphicsPipelineCreateInfo pipelineInfo{};
//        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//        pipelineInfo.stageCount = std::size(shaderStages);
//        pipelineInfo.pStages = shaderStages;
//        pipelineInfo.pVertexInputState = &vertexInputState;
//        pipelineInfo.pInputAssemblyState = &inputAssembly;
//        pipelineInfo.pViewportState = &viewportState;
//        pipelineInfo.pRasterizationState = &rasterizer;
//        pipelineInfo.pMultisampleState = &multisampling;
//        pipelineInfo.pColorBlendState = &colorBlending;
//        pipelineInfo.pDynamicState = &dynamicState;
//        pipelineInfo.pDepthStencilState = &depthStencil;
//        pipelineInfo.layout = g_PipelineLayout;
//        pipelineInfo.renderPass = g_Deferred_Gbuffer.m_RenderPass;
//        pipelineInfo.subpass = 0;
//
////        vl::CreateGraphicsPipelines();
//        VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &g_Deferred_Gbuffer.m_Pipeline));
//
//        vkh::DestroyShaderModules(shaderStages);  // right here. before next load another.


        g_Deferred_Gbuffer.m_Pipeline =
        vkx::CreateGraphicsPipeline(
                {
                    Loader::loadAssets("shaders-vk/spv/def_gbuffer/vert.spv"),
                    Loader::loadAssets("shaders-vk/spv/def_gbuffer/frag.spv")
                },
                {
                    VK_FORMAT_R32G32B32_SFLOAT, // in_pos
                    VK_FORMAT_R32G32_SFLOAT,    // in_tex
                    VK_FORMAT_R32G32B32_SFLOAT  // in_norm
                },
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                3,
                {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR},
                g_PipelineLayout,
                g_Deferred_Gbuffer.m_RenderPass);







        VkDescriptorSetLayout descriptorSetLayout = vl::CreateDescriptorSetLayout(device, {
                // binding 0: vsh uniform
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
                // 1: fsh uniform
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT},
                // 2: fsh gPosition
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
                // 3: fsh gNormal
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
                // 4: fsh gAlbedo
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
        });

        VkPipelineLayout pipelineLayout = vl::CreatePipelineLayout(device, 1, &descriptorSetLayout);

        g_Deferred_Compose.m_PipelineLayout = pipelineLayout;
        g_Deferred_Compose.m_DescSet =
                CreateGCompose_DescSets(descriptorSetLayout);
        g_Deferred_Compose.m_DescSetLayout = descriptorSetLayout;

        //  Compose Pipeline


//        vkh::LoadShaderStages_H(shaderStages, "shaders-vk/spv/def_compose/{}.spv");
//
//        pipelineInfo.stageCount = std::size(shaderStages);
//        pipelineInfo.pStages = shaderStages;
//
//        VkPipelineVertexInputStateCreateInfo emptyVertexInputState = vkh::c_PipelineVertexInputState();
//        pipelineInfo.pVertexInputState = &emptyVertexInputState;
//
//        colorBlending = vkh::c_PipelineColorBlendState(1, colorBlendAttachments);
//        pipelineInfo.pColorBlendState = &colorBlending;
//
//        pipelineInfo.renderPass = g_RenderPass;
//        pipelineInfo.layout = pipelineLayout;
//
//
//
//        VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &g_Deferred_Compose.m_Pipeline));
//
//
//
//        vkh::DestroyShaderModules(shaderStages);


        g_Deferred_Compose.m_Pipeline =
        vkx::CreateGraphicsPipeline(
                {
                        Loader::loadAssets("shaders-vk/spv/def_compose/vert.spv"),
                        Loader::loadAssets("shaders-vk/spv/def_compose/frag.spv")
                },
                {},  // empty
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                1,
                {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR},
                pipelineLayout,
                g_RenderPass);

    }


















    static void CreateRenderPass()
    {
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

        VkAttachmentDescription attachmentsDesc[] = {
                vl::IAttachmentDescription(g_SwapchainImageFormat, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR),
                vl::IAttachmentDescription(vkx::findDepthFormat(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) // .storeOp: VK_ATTACHMENT_STORE_OP_DONT_CARE
        };

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = std::size(attachmentsDesc);
        renderPassInfo.pAttachments = attachmentsDesc;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(vkx::ctx().Device, &renderPassInfo, nullptr, &g_RenderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass.");
        }

    }





//    static void CreateGraphicsPipeline()
//    {
//        VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkh::c_PipelineVertexInputState_H(
//                { VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT });
//
//        VkPipelineInputAssemblyStateCreateInfo inputAssembly = vkh::c_PipelineInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
//        VkPipelineViewportStateCreateInfo viewportState = vkh::c_PipelineViewportState(1, 1);
//        VkPipelineRasterizationStateCreateInfo rasterizer = vkh::c_PipelineRasterizationState(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
//        VkPipelineMultisampleStateCreateInfo multisampling = vkh::c_PipelineMultisampleState();
//        VkPipelineDepthStencilStateCreateInfo depthStencil = vkh::c_PipelineDepthStencilState();
//        VkPipelineDynamicStateCreateInfo dynamicState = vkh::c_PipelineDynamicState_H_ViewportScissor();
//
//        VkPipelineColorBlendAttachmentState colorBlendAttachment = vkh::c_PipelineColorBlendAttachmentState();
//        VkPipelineColorBlendStateCreateInfo colorBlending = vkh::c_PipelineColorBlendState(1, &colorBlendAttachment);
//
//
//        VkPipelineShaderStageCreateInfo shaderStages[2];
//        vkh::LoadShaderStages_H(shaderStages, "shaders-vk/spv/def_gbuffer/{}.spv");
//
////        vkx::PipelineCreator newPipeline;
//
//        VkGraphicsPipelineCreateInfo pipelineInfo{};
//        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//        pipelineInfo.stageCount = std::size(shaderStages);
//        pipelineInfo.pStages = shaderStages;
//        pipelineInfo.pVertexInputState = &vertexInputInfo;
//        pipelineInfo.pInputAssemblyState = &inputAssembly;
//        pipelineInfo.pViewportState = &viewportState;
//        pipelineInfo.pRasterizationState = &rasterizer;
//        pipelineInfo.pMultisampleState = &multisampling;
//        pipelineInfo.pColorBlendState = &colorBlending;
//        pipelineInfo.pDynamicState = &dynamicState;
//        pipelineInfo.pDepthStencilState = &depthStencil;
//        pipelineInfo.layout = g_PipelineLayout;
//        pipelineInfo.renderPass = g_RenderPass;
//        pipelineInfo.subpass = 0;
//        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
//
//        if (vkCreateGraphicsPipelines(vkx::ctx().Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &g_GraphicsPipeline) != VK_SUCCESS) {
//            throw std::runtime_error("failed to create graphics pipeline.");
//        }
//
//        vkh::DestroyShaderModules(shaderStages);
//    }


























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



    static void CreateCommandBuffers()
    {
        g_CommandBuffers.resize(MAX_FRAMES_INFLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = vkx::ctx().CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = g_CommandBuffers.size();

        VK_CHECK_MSG(
                vkAllocateCommandBuffers(vkx::ctx().Device, &allocInfo, g_CommandBuffers.data()),
                "failed to allocate command buffers!");
    }


    static void CreateSyncObjects_Semaphores_Fences()
    {

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        g_SemaphoreImageAcquired.resize(MAX_FRAMES_INFLIGHT);
        g_SemaphoreRenderComplete.resize(MAX_FRAMES_INFLIGHT);
        g_InflightFence.resize(MAX_FRAMES_INFLIGHT);

        VkDevice device = vkx::ctx().Device;
        for (int i = 0; i < MAX_FRAMES_INFLIGHT; ++i)
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

        g_UniformBuffers.resize(MAX_FRAMES_INFLIGHT);

        for (int i = 0; i < MAX_FRAMES_INFLIGHT; ++i)
        {
            g_UniformBuffers[i] = new vkx::UniformBuffer(bufferSize);
        }
    }

    static void UpdateUniformBuffer(uint32_t currframe)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo{};

        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(45.0f), g_SwapchainExtent.width / (float) g_SwapchainExtent.height, 0.1f, 10.0f);

        ubo.proj[1][1] *= -1;

        g_UniformBuffers[currframe]->memcpy(&ubo, sizeof(ubo));
    }


//    static void CreateDescriptorPool()
//    {
//        VkDescriptorPoolSize pool_sizes[] = {
//                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100},
//                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100}
//        };
//
//        VkDescriptorPoolCreateInfo poolInfo{};
//        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//        poolInfo.poolSizeCount = std::size(pool_sizes);
//        poolInfo.pPoolSizes = pool_sizes;
//        poolInfo.maxSets = 100;
//
//        if (vkCreateDescriptorPool(vkx::ctx().Device, &poolInfo, nullptr, &g_DescriptorPool) != VK_SUCCESS) {
//            throw std::runtime_error("failed to create descriptor pool.");
//        }
//    }

    static VkDescriptorSet CreateGCompose_DescSets(VkDescriptorSetLayout descriptorSetLayout)
    {
        VkDevice device = vkx::ctx().Device;
        VkSampler sampler = vkx::ctx().ImageSampler;

        VkDescriptorSet descriptorSet;
        vl::AllocateDescriptorSets(device, vkx::ctx().DescriptorPool, 1, &descriptorSetLayout, &descriptorSet);

        vkx::DescriptorWrites dwrites{descriptorSet};
        dwrites.UniformBuffer(g_UniformBuffers[0]->buffer(), sizeof(UniformBufferObject));
        dwrites.UniformBuffer(g_UniformBuffers[0]->buffer(), sizeof(UniformBufferObject));
        dwrites.CombinedImageSampler(g_Deferred_Gbuffer.gPosition.m_Img->m_ImageView, sampler);
        dwrites.CombinedImageSampler(g_Deferred_Gbuffer.gNormal.m_Img->m_ImageView, sampler);
        dwrites.CombinedImageSampler(g_Deferred_Gbuffer.gAlbedo.m_Img->m_ImageView, sampler);

        dwrites.WriteDescriptorSets(device);

        return descriptorSet;
    }

    static void CreateDescriptorSets()
    {
        VkDevice device = vkx::ctx().Device;
        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_INFLIGHT, g_DescriptorSetLayout);

        g_DescriptorSets.resize(MAX_FRAMES_INFLIGHT);
        vl::AllocateDescriptorSets(device, vkx::ctx().DescriptorPool, MAX_FRAMES_INFLIGHT, layouts.data(), g_DescriptorSets.data());

        for (int i = 0; i < MAX_FRAMES_INFLIGHT; ++i)
        {
            vkx::DescriptorWrites writes{g_DescriptorSets[i]};

            writes.UniformBuffer(g_UniformBuffers[i]->buffer(), sizeof(UniformBufferObject));
            writes.CombinedImageSampler(g_TextureImage->m_ImageView, vkx::ctx().ImageSampler);

            writes.WriteDescriptorSets(device);
        }
    }


































    static void RecordCommandBuffer(VkCommandBuffer cmdbuf, uint32_t imageIdx)
    {
        vkx::CommandBuffer cmd{cmdbuf};
        cmd.BeginCommandBuffer(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

        // Deferred :: Gbuffer

        VkClearValue clearValues[4]{};
        clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};  // gPosition
        clearValues[1].color = {0.0f, 0.0f, 0.0f, 1.0f};  // gNormal
        clearValues[2].color = {0.0f, 0.0f, 0.0f, 1.0f};  // gAlbedo
        clearValues[3].depthStencil = {1.0f, 0};
        VkExtent2D gbufferExtent = {1024, 1024};
        cmd.CmdBeginRenderPass(g_Deferred_Gbuffer.m_RenderPass, g_Deferred_Gbuffer.m_Framebuffer, gbufferExtent,
                               4, clearValues);

        cmd.CmdBindGraphicsPipeline(g_Deferred_Gbuffer.m_Pipeline);
        cmd.CmdSetViewport(gbufferExtent);
        cmd.CmdSetScissor(gbufferExtent);

        cmd.CmdBindDescriptorSets(g_PipelineLayout, &g_DescriptorSets[g_CurrentFrameInflight]);

        cmd.CmdBindVertexBuffer(g_TestModel->vtxbuffer());
        cmd.CmdBindIndexBuffer(g_TestModel->idxbuffer());
        cmd.CmdDrawIndexed(g_TestModel->vertexCount());

        cmd.CmdEndRenderPass();



        // Deferred :: Compose






        // Main
        clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        cmd.CmdBeginRenderPass(g_RenderPass, g_SwapchainFramebuffers[imageIdx], g_SwapchainExtent,
                               2, clearValues);

//        cmd.CmdBindGraphicsPipeline(g_GraphicsPipeline);
        cmd.CmdBindGraphicsPipeline(g_Deferred_Compose.m_Pipeline);
        cmd.CmdSetViewport(g_SwapchainExtent);
        cmd.CmdSetScissor(g_SwapchainExtent);

        cmd.CmdBindDescriptorSets(g_Deferred_Compose.m_PipelineLayout, &g_Deferred_Compose.m_DescSet);

        cmd.CmdDrawIndexed(6);

        Imgui::RenderGUI(cmdbuf);

        cmd.CmdEndRenderPass();

        cmd.EndCommandBuffer();
    }



    static void DrawFrameIntl()
    {
        VkDevice device = vkx::ctx().Device;
        const int currframe = g_CurrentFrameInflight;

        vkWaitForFences(device, 1, &g_InflightFence[currframe], VK_TRUE, UINT64_MAX);

        uint32_t imageIdx;
        vkAcquireNextImageKHR(device, g_SwapchainKHR, UINT64_MAX, g_SemaphoreImageAcquired[currframe], nullptr, &imageIdx);
        vkResetFences(device, 1, &g_InflightFence[currframe]);
        vkResetCommandBuffer(g_CommandBuffers[currframe], 0);

        UpdateUniformBuffer(currframe);

        RecordCommandBuffer(g_CommandBuffers[currframe], imageIdx);


        vl::QueueSubmit(vkx::ctx().GraphicsQueue, g_CommandBuffers[currframe],
                         g_SemaphoreImageAcquired[currframe], g_SemaphoreRenderComplete[currframe],
                         g_InflightFence[currframe]);

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &g_SemaphoreRenderComplete[currframe];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &g_SwapchainKHR;
        presentInfo.pImageIndices = &imageIdx;

        vkQueuePresentKHR(vkx::ctx().PresentQueue, &presentInfo);
        if (g_RecreateSwapchainRequested) {
            g_RecreateSwapchainRequested = false;
            RecreateSwapchain();
        }

//         vkQueueWaitIdle(g_PresentQueue);  // BigWaste on GPU.

        g_CurrentFrameInflight = (g_CurrentFrameInflight + 1) % MAX_FRAMES_INFLIGHT;
    }



};


















void VulkanIntl::Init(GLFWwindow* glfwWindow)
{
    VulkanIntl_Impl::Init(glfwWindow);

    vkx::ctx()._RenderPass = VulkanIntl_Impl::g_RenderPass;
}

void VulkanIntl::Destroy() {
    VulkanIntl_Impl::Destroy();
}

void VulkanIntl::DrawFrame() {
    VulkanIntl_Impl::DrawFrameIntl();
}

void VulkanIntl::RequestRecreateSwapchain() {
    VulkanIntl_Impl::g_RecreateSwapchainRequested = true;
}






VkImageView VulkanIntl::getTestImgView() {
    return VulkanIntl_Impl::g_Deferred_Gbuffer.gPosition.m_Img->m_ImageView;
}

