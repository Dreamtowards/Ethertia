//
// Created by Dreamtowards on 2023/3/16.
//


#include <map>
#include <set>
#include <chrono>
#include <array>

#include <eldaria/util/Loader.h>
#include <ethertia/util/Collections.h>
#include <ethertia/util/Log.h>

#include <glm/glm.hpp>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>


#include "VulkanIntl.h"

#include <eldaria/vulkan/vkh.cpp>
#include <eldaria/imgui/Imgui.h>


vkh::VertexData g_TestModel;


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

    inline static VkInstance        g_Instance  = nullptr;
    inline static VkPhysicalDevice  g_PhysDevice= nullptr;
    inline static VkDevice          g_Device    = nullptr;  // Logical Device

    inline static VkRenderPass      g_RenderPass = nullptr;

    inline static VkQueue g_GraphicsQueue = nullptr;
    inline static VkQueue g_PresentQueue = nullptr;  // Surface Present

    inline static VkSurfaceKHR          g_SurfaceKHR = nullptr;
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

    inline static VkCommandPool g_CommandPool = nullptr;
    inline static std::vector<VkCommandBuffer> g_CommandBuffers;
    inline static VkDescriptorPool g_DescriptorPool = nullptr;
    inline static VkSampler g_TextureSampler = nullptr;

    inline static std::vector<VkSemaphore> g_SemaphoreImageAcquired;  // for each InflightFrame   ImageAcquired, RenderComplete
    inline static std::vector<VkSemaphore> g_SemaphoreRenderComplete;
    inline static std::vector<VkFence>     g_InflightFence;

    inline static const int MAX_FRAMES_INFLIGHT = 2;
    inline static int g_CurrentFrameInflight = 0;

    inline static std::vector<VkBuffer> g_UniformBuffers;  // for each InflightFrame
    inline static std::vector<VkDeviceMemory> g_UniformBuffersMemory;
    inline static std::vector<void*> g_UniformBuffersMapped;  // 'Persistent Mapping' since vkMapMemory cost.

    inline static std::vector<VkDescriptorSet> g_DescriptorSets;  // for each InflightFrame

    inline static vkh::Image g_DepthImage;
    inline static vkh::Image g_TextureImage;




    static void Init(GLFWwindow* glfwWindow)
    {
        g_Instance = vkh::CreateInstance();
        vkh::CreateSurface(g_Instance, g_WindowHandle=glfwWindow, g_SurfaceKHR);

        PickPhysicalDevice();
        CreateLogicalDevice();

        CreateDescriptorPool();
        CreateCommandPool();
        CreateCommandBuffers();
        CreateSyncObjects_Semaphores_Fences();
        vkh::g_Device = g_Device; vkh::g_PhysDevice = g_PhysDevice;
        vkh::g_GraphicsQueue = g_GraphicsQueue; vkh::g_CommandPool = g_CommandPool;

        g_TextureSampler = vkh::CreateTextureSampler();

        CreateSwapchainAndImageViews();
        CreateRenderPass();     // depend: Swapchain format
        CreateDepthTexture();
        CreateFramebuffers();   // depend: DepthTexture, RenderPass


        CreateDescriptorSetLayout();
        CreateGraphicsPipeline();  // depend: RenderPass, DescriptorSetLayout

        {
            BitmapImage bitmapImage = Loader::loadPNG("/Users/dreamtowards/Downloads/viking_room.png");
            vkh::CreateTextureImage(bitmapImage, g_TextureImage);

            VertexData vdata = Loader::loadOBJ("/Users/dreamtowards/Downloads/viking_room.obj");
            g_TestModel.m_VertexCount = vdata.vertexCount();
            vkh::CreateVertexBuffer(vdata.data(), vdata.size(),
                                    g_TestModel.m_VertexBuffer,
                                    g_TestModel.m_VertexBufferMemory);
        }

        CreateUniformBuffers();
        CreateDescriptorSets();


        InitDeferredRendering();

    }

    static void Destroy()
    {
        DestroySwapchain();

        g_TextureImage.destroy();
        g_TestModel.destroy();


        vkDestroyDescriptorSetLayout(g_Device, g_DescriptorSetLayout, nullptr);
        for (int i = 0; i < MAX_FRAMES_INFLIGHT; ++i) {
            vkDestroyBuffer(g_Device, g_UniformBuffers[i], nullptr);
            vkFreeMemory(g_Device, g_UniformBuffersMemory[i], nullptr);
        }

        vkDestroyPipeline(g_Device, g_GraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(g_Device, g_PipelineLayout, nullptr);
        vkDestroyRenderPass(g_Device, g_RenderPass, nullptr);

        for (int i = 0; i < MAX_FRAMES_INFLIGHT; ++i) {
            vkDestroySemaphore(g_Device, g_SemaphoreImageAcquired[i], nullptr);
            vkDestroySemaphore(g_Device, g_SemaphoreRenderComplete[i], nullptr);
            vkDestroyFence(g_Device, g_InflightFence[i], nullptr);
        }

        vkDestroySampler(g_Device, g_TextureSampler, nullptr);
        vkDestroyDescriptorPool(g_Device, g_DescriptorPool, nullptr);
        vkDestroyCommandPool(g_Device, g_CommandPool, nullptr);
        vkDestroyDevice(g_Device, nullptr);
        vkDestroySurfaceKHR(g_Instance, g_SurfaceKHR, nullptr);

        vkh::DestroyInstance(g_Instance);
    }

    static void PickPhysicalDevice()
    {
        uint32_t gpu_count = 0;
        vkEnumeratePhysicalDevices(g_Instance, &gpu_count, nullptr);

        Log::info("GPUs: ", gpu_count);
        assert(gpu_count && "failed to find GPU with vulkan support.");

        std::vector<VkPhysicalDevice> gpus(gpu_count);
        vkEnumeratePhysicalDevices(g_Instance, &gpu_count, gpus.data());

        VkPhysicalDevice dev = gpus[0];
        for (const auto& physGPU : gpus)
        {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(physGPU, &deviceProperties);

            VkPhysicalDeviceFeatures deviceFeatures;
            vkGetPhysicalDeviceFeatures(physGPU, &deviceFeatures);

            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                dev = physGPU;
                break;
            }
        }
        g_PhysDevice = dev;
    }

    static void CreateLogicalDevice()
    {
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        // Queue Family
        QueueFamilyIndices queueFamily = vkh::findQueueFamilies(g_PhysDevice, g_SurfaceKHR);
        float queuePriority = 1.0f;  // 0.0-1.0

        std::set<uint32_t> uniqQueueFamilyIdx = {queueFamily.m_GraphicsFamily, queueFamily.m_PresentFamily};
        std::vector<VkDeviceQueueCreateInfo> arrQueueCreateInfo;
        for (uint32_t queueFamilyIdx : uniqQueueFamilyIdx) {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.pQueuePriorities = &queuePriority;

            queueCreateInfo.queueFamilyIndex = queueFamilyIdx;
            queueCreateInfo.queueCount = 1;
            arrQueueCreateInfo.push_back(queueCreateInfo);
        }

        createInfo.pQueueCreateInfos = arrQueueCreateInfo.data();
        createInfo.queueCreateInfoCount = arrQueueCreateInfo.size();

        // Device Features
        VkPhysicalDeviceFeatures deviceFeatures{};
        vkGetPhysicalDeviceFeatures(g_PhysDevice, &deviceFeatures);
        deviceFeatures.samplerAnisotropy = VK_TRUE;
        createInfo.pEnabledFeatures = &deviceFeatures;

        // Device Extensions  (needs check is supported?)
        std::vector<const char*> deviceExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#ifdef __APPLE__
                "VK_KHR_portability_subset"
#endif
        };
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        createInfo.enabledExtensionCount = deviceExtensions.size();

//        // Device Validation Layer: already deprecated. ValidationLayer only belongs to VkInstance.
//        if (g_EnableValidationLayer) {
//            createInfo.ppEnabledLayerNames = g_ValidationLayers.data();
//            createInfo.enabledLayerCount = g_ValidationLayers.size();
//        } else {
//            createInfo.enabledLayerCount = 0;
//        }

        // VK_KHR_swapchain
        if (vkCreateDevice(g_PhysDevice, &createInfo, nullptr, &g_Device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device.");
        }

        // Get Queue by the way.
        vkGetDeviceQueue(g_Device, queueFamily.m_GraphicsFamily, 0, &g_GraphicsQueue);
        vkGetDeviceQueue(g_Device, queueFamily.m_PresentFamily, 0, &g_PresentQueue);
    }

    static void CreateSwapchainAndImageViews()
    {
        vkh::CreateSwapchain(g_PhysDevice, g_SurfaceKHR, g_WindowHandle,
                             g_SwapchainKHR,
                             g_SwapchainExtent, g_SwapchainImageFormat,
                             g_SwapchainImages, g_SwapchainImageViews);
    }

    static void DestroySwapchain()
    {
        g_DepthImage.destroy();

        for (auto fb : g_SwapchainFramebuffers) {
            vkDestroyFramebuffer(g_Device, fb, nullptr);
        }
        for (auto imageview : g_SwapchainImageViews) {
            vkDestroyImageView(g_Device, imageview, nullptr);
        }
        vkDestroySwapchainKHR(g_Device, g_SwapchainKHR, nullptr);
    }

    static void RecreateSwapchain()
    {
        int w, h;
        glfwGetFramebufferSize(g_WindowHandle, &w, &h);
        while (w==0 || h==0) {
            glfwGetFramebufferSize(g_WindowHandle, &w, &h);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(g_Device);
        DestroySwapchain();
        Log::info("RecreateSwapchain");

        CreateSwapchainAndImageViews();
        CreateDepthTexture();
        CreateFramebuffers();
    }

    static void CreateDepthTexture()
    {
        vkh::CreateDepthTextureImage(g_SwapchainExtent.width, g_SwapchainExtent.height,
                                     g_DepthImage.m_Image, g_DepthImage.m_ImageMemory, g_DepthImage.m_ImageView);
    }





    // aka RenderTarget in DX12
    struct FramebufferAttachment
    {
//        VkImage m_Image;
//        VkDeviceMemory m_ImageMemory;
//        VkImageView m_ImageView;
        vkh::Image m_Img;
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

    } g_Deferred_Compose;






    static FramebufferAttachment CreateFramebufferAttachment(int w, int h, VkFormat format, bool depth = false)
    {
        FramebufferAttachment out{};

        vkh::CreateImage(w, h, out.m_Img.m_Image,out.m_Img.m_ImageMemory, format,
                    depth ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);

        out.m_Img.m_ImageView = vkh::CreateImageView(out.m_Img.m_Image, format,
                    depth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT);

        out.m_Desc = vkh::c_AttachmentDescription(format,
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
        // Gbuffer Attachments
        int attach_size = 1024;
        VkFormat rgbFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
        g_Deferred_Gbuffer.gPosition = CreateFramebufferAttachment(attach_size,attach_size, rgbFormat);
        g_Deferred_Gbuffer.gNormal   = CreateFramebufferAttachment(attach_size,attach_size, rgbFormat);
        g_Deferred_Gbuffer.gAlbedo   = CreateFramebufferAttachment(attach_size,attach_size, rgbFormat);
        g_Deferred_Gbuffer.gDepth    = CreateFramebufferAttachment(attach_size,attach_size, vkh::findDepthFormat(), true);

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
//            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
//            dependency.dstSubpass = 0;
//            dependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
//            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//            dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
//            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//            dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
//
//            dependencies[1].srcSubpass = 0;
//            dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
//            dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//            dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
//            dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//            dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
//            dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcAccessMask = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = std::size(attachmentDesc);
        renderPassInfo.pAttachments = attachmentDesc;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;//std::size(dependencies);
        renderPassInfo.pDependencies = &dependency;

        VK_CHECK(vkCreateRenderPass(g_Device, &renderPassInfo, nullptr, &g_Deferred_Gbuffer.m_RenderPass));

        // Gbuffer Framebuffer

        VkImageView attachmentViews[] = {
                g_Deferred_Gbuffer.gPosition.m_Img.m_ImageView,
                g_Deferred_Gbuffer.gNormal.m_Img.m_ImageView,
                g_Deferred_Gbuffer.gAlbedo.m_Img.m_ImageView,
                g_Deferred_Gbuffer.gDepth.m_Img.m_ImageView,
        };
        VkFramebufferCreateInfo framebufferInfo =
                vkh::c_Framebuffer(attach_size,attach_size, g_Deferred_Gbuffer.m_RenderPass,
                                   std::size(attachmentViews), attachmentViews);

        VK_CHECK(vkCreateFramebuffer(g_Device, &framebufferInfo, nullptr, &g_Deferred_Gbuffer.m_Framebuffer));



        // Gbuffer Pipeline

        VkPipelineVertexInputStateCreateInfo vertexInputState = vkh::c_PipelineVertexInputState_H({
            VK_FORMAT_R32G32B32_SFLOAT, // in_pos
            VK_FORMAT_R32G32_SFLOAT,    // in_tex
            VK_FORMAT_R32G32B32_SFLOAT  // in_norm
        });
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = vkh::c_PipelineInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        VkPipelineViewportStateCreateInfo viewportState = vkh::c_PipelineViewportState(1, 1);
        VkPipelineRasterizationStateCreateInfo rasterizer = vkh::c_PipelineRasterizationState();  // VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE
        VkPipelineMultisampleStateCreateInfo multisampling = vkh::c_PipelineMultisampleState();
        VkPipelineDepthStencilStateCreateInfo depthStencil = vkh::c_PipelineDepthStencilState();
        VkPipelineDynamicStateCreateInfo dynamicState = vkh::c_PipelineDynamicState_H_ViewportScissor();

        // for gPosition, gNormal, gAlbedo, if no, as color mask = 0x0, black will be render.
        VkPipelineColorBlendAttachmentState colorBlendAttachments[] = {
                vkh::c_PipelineColorBlendAttachmentState(),
                vkh::c_PipelineColorBlendAttachmentState(),
                vkh::c_PipelineColorBlendAttachmentState()
        };
        VkPipelineColorBlendStateCreateInfo colorBlending = vkh::c_PipelineColorBlendState(3, colorBlendAttachments);

        VkPipelineShaderStageCreateInfo shaderStages[2];
        vkh::LoadShaderStages_H(shaderStages, "shaders-vk/spv/def_gbuffer/{}.spv");

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = std::size(shaderStages);
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputState;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.layout = g_PipelineLayout;
        pipelineInfo.renderPass = g_Deferred_Gbuffer.m_RenderPass;
        pipelineInfo.subpass = 0;

        VK_CHECK(vkCreateGraphicsPipelines(g_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &g_Deferred_Gbuffer.m_Pipeline));

        vkh::DestroyShaderModules(shaderStages);  // right here. before next load another.





//        VkDescriptorSetLayout descriptorSetLayout = vkh::CreateDescriptorSetLayout({
//                // binding 0: vsh uniform
//                vkh::c_DescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
//                // 1: fsh uniform
//                vkh::c_DescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT),
//                // 2: fsh gPosition
//                vkh::c_DescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
//                // 3: fsh gNormal
//                vkh::c_DescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT),
//                // 4: fsh gAlbedo
//                vkh::c_DescriptorSetLayoutBinding(4, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT),
//        });
//
//        VkPipelineLayout pipelineLayout = vkh::CreatePipelineLayout(1, &descriptorSetLayout);


        //  Compose Pipeline


        vkh::LoadShaderStages_H(shaderStages, "shaders-vk/spv/def_compose/{}.spv");

        pipelineInfo.stageCount = std::size(shaderStages);
        pipelineInfo.pStages = shaderStages;

        VkPipelineVertexInputStateCreateInfo emptyVertexInputState = vkh::c_PipelineVertexInputState();
        pipelineInfo.pVertexInputState = &emptyVertexInputState;

        colorBlending = vkh::c_PipelineColorBlendState(1, colorBlendAttachments);
        pipelineInfo.pColorBlendState = &colorBlending;

        pipelineInfo.renderPass = g_RenderPass;



        VK_CHECK(vkCreateGraphicsPipelines(g_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &g_Deferred_Compose.m_Pipeline));



        vkh::DestroyShaderModules(shaderStages);

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
                vkh::c_AttachmentDescription(g_SwapchainImageFormat, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR),
                vkh::c_AttachmentDescription(vkh::findDepthFormat(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) // .storeOp: VK_ATTACHMENT_STORE_OP_DONT_CARE
        };

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = std::size(attachmentsDesc);
        renderPassInfo.pAttachments = attachmentsDesc;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(g_Device, &renderPassInfo, nullptr, &g_RenderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass.");
        }

    }





    static void CreateGraphicsPipeline()
    {
        VkPipelineVertexInputStateCreateInfo vertexInputInfo = vkh::c_PipelineVertexInputState_H(
                { VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT });

        VkPipelineInputAssemblyStateCreateInfo inputAssembly = vkh::c_PipelineInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        VkPipelineViewportStateCreateInfo viewportState = vkh::c_PipelineViewportState(1, 1);
        VkPipelineRasterizationStateCreateInfo rasterizer = vkh::c_PipelineRasterizationState(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
        VkPipelineMultisampleStateCreateInfo multisampling = vkh::c_PipelineMultisampleState();
        VkPipelineDepthStencilStateCreateInfo depthStencil = vkh::c_PipelineDepthStencilState();
        VkPipelineDynamicStateCreateInfo dynamicState = vkh::c_PipelineDynamicState_H_ViewportScissor();

        VkPipelineColorBlendAttachmentState colorBlendAttachment = vkh::c_PipelineColorBlendAttachmentState();
        VkPipelineColorBlendStateCreateInfo colorBlending = vkh::c_PipelineColorBlendState(1, &colorBlendAttachment);

        g_PipelineLayout = vkh::CreatePipelineLayout(1, &g_DescriptorSetLayout);

        VkPipelineShaderStageCreateInfo shaderStages[2];
        vkh::LoadShaderStages_H(shaderStages, "shaders-vk/spv/def_gbuffer/{}.spv");

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = std::size(shaderStages);
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.layout = g_PipelineLayout;
        pipelineInfo.renderPass = g_RenderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(g_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &g_GraphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline.");
        }

        vkh::DestroyShaderModules(shaderStages);
    }


























    static void CreateFramebuffers()
    {
        g_SwapchainFramebuffers.resize(g_SwapchainImageViews.size());

        for (size_t i = 0; i < g_SwapchainImageViews.size(); i++)
        {
            std::array<VkImageView, 2> attachments = { g_SwapchainImageViews[i], g_DepthImage.m_ImageView };

            VkFramebufferCreateInfo framebufferInfo =
                    vkh::c_Framebuffer(g_SwapchainExtent.width, g_SwapchainExtent.height,
                                       g_RenderPass, attachments.size(), attachments.data());

            if (vkCreateFramebuffer(g_Device, &framebufferInfo, nullptr, &g_SwapchainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }


    static void CreateCommandPool()
    {
        QueueFamilyIndices queueFamilyIndices = vkh::findQueueFamilies(g_PhysDevice, g_SurfaceKHR);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.m_GraphicsFamily;

        if (vkCreateCommandPool(g_Device, &poolInfo, nullptr, &g_CommandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool.");
        }
    }


    static void CreateCommandBuffers()
    {
        g_CommandBuffers.resize(MAX_FRAMES_INFLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = g_CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = g_CommandBuffers.size();

        VK_CHECK_MSG(
                vkAllocateCommandBuffers(g_Device, &allocInfo, g_CommandBuffers.data()),
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

        for (int i = 0; i < MAX_FRAMES_INFLIGHT; ++i)
        {
            VK_CHECK(vkCreateSemaphore(g_Device, &semaphoreInfo, nullptr, &g_SemaphoreImageAcquired[i]));
            VK_CHECK(vkCreateSemaphore(g_Device, &semaphoreInfo, nullptr, &g_SemaphoreRenderComplete[i]));
            VK_CHECK(vkCreateFence(g_Device, &fenceInfo, nullptr, &g_InflightFence[i]));
        }
    }










    static void CreateDescriptorSetLayout()
    {
        g_DescriptorSetLayout = vkh::CreateDescriptorSetLayout({
            vkh::c_DescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
            vkh::c_DescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
        });
    }

    static void CreateUniformBuffers()
    {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        g_UniformBuffers.resize(MAX_FRAMES_INFLIGHT);
        g_UniformBuffersMemory.resize(MAX_FRAMES_INFLIGHT);
        g_UniformBuffersMapped.resize(MAX_FRAMES_INFLIGHT);

        for (int i = 0; i < MAX_FRAMES_INFLIGHT; ++i) {
            vkh::CreateBuffer(bufferSize,
                         g_UniformBuffers[i],
                         g_UniformBuffersMemory[i],
                         VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            vkMapMemory(g_Device, g_UniformBuffersMemory[i], 0, sizeof(UniformBufferObject), 0,
                        &g_UniformBuffersMapped[i]);
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

        memcpy(g_UniformBuffersMapped[currframe], &ubo, sizeof(ubo));
    }


    static void CreateDescriptorPool()
    {
        VkDescriptorPoolSize pool_sizes[] = {
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_INFLIGHT},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_FRAMES_INFLIGHT}
        };

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = std::size(pool_sizes);
        poolInfo.pPoolSizes = pool_sizes;
        poolInfo.maxSets = MAX_FRAMES_INFLIGHT;

        if (vkCreateDescriptorPool(g_Device, &poolInfo, nullptr, &g_DescriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool.");
        }
    }

    static void CreateDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_INFLIGHT, g_DescriptorSetLayout);

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = g_DescriptorPool;
        allocInfo.descriptorSetCount = MAX_FRAMES_INFLIGHT;
        allocInfo.pSetLayouts = layouts.data();

        g_DescriptorSets.resize(MAX_FRAMES_INFLIGHT);
        if (vkAllocateDescriptorSets(g_Device, &allocInfo, g_DescriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets.");
        }

        for (int i = 0; i < MAX_FRAMES_INFLIGHT; ++i)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = g_UniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = g_TextureImage.m_ImageView;
            imageInfo.sampler = g_TextureSampler;

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = g_DescriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = g_DescriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(g_Device, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
        }
    }


































    static void RecordCommandBuffer(VkCommandBuffer cmdbuf, uint32_t imageIdx)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT; // Optional
        VK_CHECK(vkBeginCommandBuffer(cmdbuf, &beginInfo));

        // Gbuffer

        VkClearValue clearValues[4]{};
        clearValues[0].color = {0.0f, 0.0f, 0.2f, 1.0f};
        clearValues[1].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues[2].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues[3].depthStencil = {1.0f, 0};
        VkExtent2D gbufferExtent = {1024, 1024};
        vkh::CmdBeginRenderPass(cmdbuf, g_Deferred_Gbuffer.m_RenderPass,
                                        g_Deferred_Gbuffer.m_Framebuffer, gbufferExtent, 4, clearValues);

        vkh::CmdBindGraphicsPipeline(cmdbuf, g_Deferred_Gbuffer.m_Pipeline);
        vkh::CmdSetViewport(cmdbuf, gbufferExtent);
        vkh::CmdSetScissor(cmdbuf, gbufferExtent);

        vkCmdBindDescriptorSets(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, g_PipelineLayout, 0, 1,
                                &g_DescriptorSets[g_CurrentFrameInflight], 0, nullptr);  // i?

        vkh::CmdBindVertexBuffer(cmdbuf, g_TestModel.m_VertexBuffer);

        vkCmdDraw(cmdbuf, g_TestModel.m_VertexCount, 1, 0, 0);


        vkCmdEndRenderPass(cmdbuf);  // End Gbuffer





        // Main
        clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        vkh::CmdBeginRenderPass(cmdbuf, g_RenderPass, g_SwapchainFramebuffers[imageIdx], g_SwapchainExtent,
                                2, clearValues);

        vkh::CmdBindGraphicsPipeline(cmdbuf, g_GraphicsPipeline);
        vkh::CmdSetViewport(cmdbuf, g_SwapchainExtent);
        vkh::CmdSetScissor(cmdbuf, g_SwapchainExtent);

        vkCmdBindDescriptorSets(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, g_PipelineLayout, 0, 1,
                                &g_DescriptorSets[g_CurrentFrameInflight], 0, nullptr);  // i?

        vkh::CmdBindVertexBuffer(cmdbuf, g_TestModel.m_VertexBuffer);

        vkCmdDraw(cmdbuf, g_TestModel.m_VertexCount, 1, 0, 0);


        Imgui::RenderGUI(cmdbuf);

        vkCmdEndRenderPass(cmdbuf);


        if (vkEndCommandBuffer(cmdbuf) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer.");
        }
    }



    static void DrawFrameIntl()
    {
        const int currframe = g_CurrentFrameInflight;

        vkWaitForFences(g_Device, 1, &g_InflightFence[currframe], VK_TRUE, UINT64_MAX);

        uint32_t imageIdx;
        vkAcquireNextImageKHR(g_Device, g_SwapchainKHR, UINT64_MAX, g_SemaphoreImageAcquired[currframe], nullptr, &imageIdx);
        vkResetFences(g_Device, 1, &g_InflightFence[currframe]);
        vkResetCommandBuffer(g_CommandBuffers[currframe], 0);

        UpdateUniformBuffer(currframe);

        RecordCommandBuffer(g_CommandBuffers[currframe], imageIdx);


        vkh::QueueSubmit(g_GraphicsQueue, g_CommandBuffers[currframe],
                         g_SemaphoreImageAcquired[currframe], g_SemaphoreRenderComplete[currframe],
                         g_InflightFence[currframe]);

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &g_SemaphoreRenderComplete[currframe];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &g_SwapchainKHR;
        presentInfo.pImageIndices = &imageIdx;

        vkQueuePresentKHR(g_PresentQueue, &presentInfo);
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

    VulkanIntl::GetState(true);  // init state sync.
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






void VulkanIntl::SubmitOnetimeCommandBuffer(const std::function<void(VkCommandBuffer)>& fn_record)
{
    vkh::SubmitOnetimeCommandBuffer(fn_record);
}

VkImageView VulkanIntl::getTestImgView() {
    return VulkanIntl_Impl::g_Deferred_Gbuffer.gPosition.m_Img.m_ImageView;
}




VulkanIntl::State& VulkanIntl::GetState(bool init)
{
    static VulkanIntl::State vk;
    if (init) {
#define ASSIGN_VK_STATE(varname) vk.varname = VulkanIntl_Impl::varname;

        ASSIGN_VK_STATE(g_Instance);
        ASSIGN_VK_STATE(g_PhysDevice);
        ASSIGN_VK_STATE(g_Device);
        ASSIGN_VK_STATE(g_GraphicsQueue);
        ASSIGN_VK_STATE(g_PresentQueue);
        ASSIGN_VK_STATE(g_RenderPass);
        ASSIGN_VK_STATE(g_CommandPool);
        ASSIGN_VK_STATE(g_TextureSampler);
    }
    return vk;
}
