//
// Created by 56327 on 2023/3/30.
//

#ifndef ETHERTIA_VKH_H
#define ETHERTIA_VKH_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <functional>

// vulkan helper

namespace vkx
{

    class VertexBuffer
    {
    public:
        ~VertexBuffer() { destroy(); }

        VkBuffer m_VertexBuffer = nullptr;
        VkDeviceMemory m_VertexBufferMemory = nullptr;

        VkBuffer m_IndexBuffer = nullptr;
        VkDeviceMemory m_IndexBufferMemory = nullptr;

        // used by vkCmdDraw()
        int m_VertexCount = 0;

        void destroy();
    };

    class Pipeline
    {
    public:


    };
}

struct Image
{
    VkImage m_Image = nullptr;
    VkDeviceMemory m_ImageMemory = nullptr;
    VkImageView m_ImageView = nullptr;

    void destroy();
};




struct QueueFamilyIndices {
    uint32_t m_GraphicsFamily = -1;
    uint32_t m_PresentFamily = -1;  //Surface Present.

    bool isComplete() const {
        return m_GraphicsFamily != -1 && m_PresentFamily != -1;
    }
};

struct SwapchainSupportDetails
{
    VkSurfaceCapabilitiesKHR m_Capabilities{};
    std::vector<VkSurfaceFormatKHR> m_Formats;
    std::vector<VkPresentModeKHR> m_PresentModes;

    bool isSwapChainAdequate() const {
        return !m_Formats.empty() && !m_PresentModes.empty();
    }
};


class vkh
{
public:
    // needs manual set from Reals.
    inline static VkDevice g_Device = nullptr;
    inline static VkPhysicalDevice g_PhysDevice = nullptr;
    inline static VkCommandPool g_CommandPool = nullptr;
    inline static VkQueue       g_GraphicsQueue = nullptr;


    [[nodiscard]]
    static VkInstance CreateInstance();

    static void DestroyInstance(VkInstance instance);

    static VkImageView CreateImageView(VkImage image, VkFormat format = VK_FORMAT_R8G8B8A8_SRGB, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

    static void CreateSwapchain(
            VkPhysicalDevice physDevice,
            VkSurfaceKHR surfaceKHR,
            GLFWwindow* glfwWindow,
            VkSwapchainKHR& out_SwapchainKHR,
            VkExtent2D& out_SwapchainExtent,
            VkFormat& out_SwapchainImageFormat,
            std::vector<VkImage>& out_SwapchainImages,
            std::vector<VkImageView>& out_SwapchainImageViews);

    static void CreateSurface(VkInstance instance, GLFWwindow* glfwWindow, VkSurfaceKHR& out_SurfaceKHR);

    static VkDescriptorSetLayout CreateDescriptorSetLayout(std::initializer_list<VkDescriptorSetLayoutBinding> bindings);

    static VkPipelineLayout CreatePipelineLayout(int numSetLayouts, VkDescriptorSetLayout* pSetLayouts);

    static VkDeviceMemory AllocateMemory(VkMemoryRequirements memRequirements,
                                         VkMemoryPropertyFlags memProperties);


    static void CreateBuffer(VkDeviceSize size,
                             VkBuffer& buffer,
                             VkDeviceMemory& bufferMemory,
                             VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                             VkMemoryPropertyFlags memProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    static void CreateImage(int texWidth, int texHeight,
                            VkImage& image,
                            VkDeviceMemory& imageMemory,
                            VkFormat format = VK_FORMAT_R8G8B8A8_SRGB,
                            VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                            VkMemoryPropertyFlags memProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                            VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL);

    static VkSampler CreateTextureSampler();

    static void CreateTextureImage(BitmapImage& bitmapImage,
                                   VkImage& out_image, VkDeviceMemory& out_imageMemory,
                                   VkImageView* out_imageView = nullptr);

    static void CreateTextureImage(BitmapImage& bitmapImage, Image& img);

    static void CreateDepthTextureImage(int w, int h, VkImage& depthImage, VkDeviceMemory& depthImageMemory, VkImageView& depthImageView);


// Static buffer. (high effective read on GPU, but cannot visible/modify from CPU)
    static void CreateVertexBuffer(const void* in_data, size_t size,
                                   VkBuffer& out_buffer, VkDeviceMemory& out_bufferMemory, bool bufferUsageIndexBuffer = false);


    static void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    static void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);











    static VkCommandBuffer BeginCommandBuffer_Onetime();

    static void EndCommandBuffer_Onetime(VkCommandBuffer cmdbuf);

    static void SubmitCommandBuffer_Onetime(const std::function<void(VkCommandBuffer)>& fn_record);


    static void QueueSubmit(VkQueue queue, VkCommandBuffer cmdbuf, VkSemaphore wait, VkSemaphore signal, VkFence fence);




    static void CmdBeginRenderPass(VkCommandBuffer cmdbuf,
                                   VkRenderPass renderPass,
                                   VkFramebuffer framebuffer,
                                   VkExtent2D renderAreaExtent,
                                   int numClearValues,
                                   VkClearValue* pClearValues);

    static void CmdSetViewport(VkCommandBuffer cmdbuf,
                               VkExtent2D wh,
                               float x = 0, float y = 0,
                               float minDepth = 0.0f, float maxDepth = 1.0f);

    static void CmdSetScissor(VkCommandBuffer cmdbuf,
                              VkExtent2D extent,
                              VkOffset2D offset = {0, 0});

    static void CmdBindVertexBuffer(VkCommandBuffer cmdbuf, const VkBuffer vbuf);

    static void CmdBindIndexBuffer(VkCommandBuffer cmdbuf, const VkBuffer idx_buf);

    static void CmdBindGraphicsPipeline(VkCommandBuffer cmdbuf, VkPipeline graphicsPipeline);








    static VkFormat findDepthFormat();

    static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surfaceKHR);










    // load .spv file data, vkCreateShaderModule
    static VkShaderModule LoadShaderModule(const void* data, size_t size);

    static VkPipelineShaderStageCreateInfo LoadShaderStage(VkShaderStageFlagBits stage, const std::string& spv_filename);

    static void LoadShaderStages_H(VkPipelineShaderStageCreateInfo* dst, const std::string& spv_filename_pat);

    static void DestroyShaderModules(VkPipelineShaderStageCreateInfo* dst);






    // ============ CreateInfo of VkPipeline ============

    // Thread unsafe!
    static VkPipelineVertexInputStateCreateInfo c_PipelineVertexInputState(
            int numVertBinding = 0,
            VkVertexInputBindingDescription* pVertBinding = nullptr,
            int numVertAttr = 0,
            VkVertexInputAttributeDescription* pVertAttr = nullptr);

    static VkPipelineVertexInputStateCreateInfo c_PipelineVertexInputState_H(
            std::initializer_list<VkFormat> attribs, int binding = 0);

    static VkPipelineInputAssemblyStateCreateInfo c_PipelineInputAssemblyState(
            VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

    static VkPipelineRasterizationStateCreateInfo c_PipelineRasterizationState(
            VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL,
            VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT,
            VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE);

    static VkPipelineViewportStateCreateInfo c_PipelineViewportState(
            int viewportCount = 1, int scissorCount = 1);

    static VkPipelineDepthStencilStateCreateInfo c_PipelineDepthStencilState();

    static VkPipelineMultisampleStateCreateInfo c_PipelineMultisampleState();

    static VkPipelineDynamicStateCreateInfo c_PipelineDynamicState(
            int numStates = 0, VkDynamicState* pStates = nullptr);

    static VkPipelineDynamicStateCreateInfo c_PipelineDynamicState_H_ViewportScissor();

    static VkPipelineColorBlendAttachmentState c_PipelineColorBlendAttachmentState();

    static VkPipelineColorBlendStateCreateInfo c_PipelineColorBlendState(
            int numAttach, VkPipelineColorBlendAttachmentState* pAttach);












    static VkAttachmentDescription c_AttachmentDescription(
            VkFormat format = VK_FORMAT_R16G16B16_SFLOAT,
            VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    static VkFramebufferCreateInfo c_Framebuffer(int w, int h,
                                                 VkRenderPass renderPass,
                                                 int attchCount,
                                                 VkImageView* pAttach);

    static VkDescriptorSetLayoutBinding c_DescriptorSetLayoutBinding(int bind,
                                                                     VkDescriptorType descType,
                                                                     VkShaderStageFlags shaderStageFlags);
};

#endif //ETHERTIA_VKH_H