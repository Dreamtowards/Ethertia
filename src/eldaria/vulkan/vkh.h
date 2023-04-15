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

    class Instance
    {
    public:
        ~Instance();

        VkInstance m_Instance = nullptr;
        VkSurfaceKHR m_SurfaceKHR = nullptr;
        bool m_EnabledValidationLayer = true;

        VkPhysicalDevice m_PhysDevice = nullptr;
        VkDevice m_Device = nullptr;
        VkQueue m_GraphicsQueue = nullptr;
        VkQueue m_PresentQueue = nullptr;

        VkCommandPool m_CommandPool = nullptr;



    };

    struct InstanceInfo
    {
        VkInstance Instance;
        VkSurfaceKHR SurfaceKHR;

        VkPhysicalDevice PhysDevice;
        VkDevice Device;
        VkQueue GraphicsQueue;
        VkQueue PresentQueue;

        VkCommandPool CommandPool;
    } Def;
    void SetDef(vkx::Instance* inst) {
        Def.Instance = inst->m_Instance;
        Def.SurfaceKHR = inst->m_SurfaceKHR;
        Def.PhysDevice = inst->m_PhysDevice;
        Def.Device = inst->m_Device;
        Def.GraphicsQueue = inst->m_GraphicsQueue;
        Def.PresentQueue = inst->m_PresentQueue;
    }

    vkx::Instance& GetInst();

    vkx::Instance* Init(GLFWwindow* glfwWindow, bool enableValidationLayer = true);

    VkInstance CreateInstance(bool enableValidationLayer = true);


    class CommandBuffer
    {
    public:
        VkCommandBuffer m_CommandBuffer;

        void BeginCommandBuffer(VkCommandBufferUsageFlags usage = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
        void EndCommandBuffer();

        void CmdBeginRenderPass(VkRenderPass renderPass,
                                VkFramebuffer framebuffer,
                                VkExtent2D renderAreaExtent,
                                int numClearValues,
                                VkClearValue* pClearValues);
        void CmdEndRenderPass();

        void CmdSetViewport(VkExtent2D wh,
                            float x = 0, float y = 0,
                            float minDepth = 0.0f, float maxDepth = 1.0f);

        void CmdSetScissor(VkExtent2D extent,
                           VkOffset2D offset = {0, 0});

        void CmdBindVertexBuffer(VkBuffer vbuf);

        void CmdBindIndexBuffer(VkBuffer idx_buf);

        void CmdBindGraphicsPipeline(VkPipeline graphics_pipeline);

        void CmdBindDescriptorSets(VkPipelineLayout pipelineLayout,
                                   const VkDescriptorSet* pDescriptorSets,
                                   int descriptorSetCount = 1,
                                   VkPipelineBindPoint ePipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS);

        void CmdDrawIndexed(uint32_t vertex_count);

    };

    // Onetime CommandBuffer
    void SubmitCommandBuffer(const std::function<void(VkCommandBuffer)>& fn_record,
                             VkDevice device, VkCommandPool commandPool, VkQueue queue);

    // use Global Default device/cmdpool/queue.
    void SubmitCommandBuffer(const std::function<void(VkCommandBuffer)>& fn_record);



    class Device
    {
    public:


    };


    class Pipeline
    {
    public:

        VkDescriptorSetLayout m_DescriptorSetLayout = nullptr;

        VkPipelineLayout m_PipelineLayout = nullptr;

    };

    class DescriptorWrites
    {
        VkDescriptorSet m_DescriptorSet;

        std::vector<VkWriteDescriptorSet> m_DescriptorWrites;

        std::vector<VkDescriptorBufferInfo> m_BufferInfos;  // keep the info valid in memory until operate Allocate.
        std::vector<VkDescriptorImageInfo>  m_ImageInfos;

    public:
        DescriptorWrites(VkDescriptorSet descriptorSet, int uboCount = 10, int imageCount = 10) : m_DescriptorSet(descriptorSet)
        {
            m_BufferInfos.reserve(uboCount);
            m_ImageInfos.reserve(imageCount);
        }

        void UniformBuffer(VkBuffer buffer, VkDeviceSize size)
        {
            VkDescriptorBufferInfo& bufferInfo = m_BufferInfos.emplace_back();
            bufferInfo.buffer = buffer;
            bufferInfo.offset = 0;
            bufferInfo.range = size;

            VkWriteDescriptorSet write{};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = m_DescriptorSet;
            write.dstBinding = m_DescriptorWrites.size();
            write.dstArrayElement = 0;
            write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write.descriptorCount = 1;
            write.pBufferInfo = &bufferInfo;

            m_DescriptorWrites.push_back(write);
        }

        void CombinedImageSampler(VkImageView imageView, VkSampler sampler)
        {
            VkDescriptorImageInfo& imageInfo = m_ImageInfos.emplace_back();
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.sampler = sampler;
            imageInfo.imageView = imageView;

            VkWriteDescriptorSet write{};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet = m_DescriptorSet;
            write.dstBinding = m_DescriptorWrites.size();
            write.dstArrayElement = 0;
            write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            write.descriptorCount = 1;
            write.pImageInfo = &imageInfo;

            m_DescriptorWrites.push_back(write);
        }

        void WriteDescriptorSets(VkDevice device)
        {
            vkUpdateDescriptorSets(device, m_DescriptorWrites.size(), m_DescriptorWrites.data(), 0, nullptr);
        }


    };






    // created from Loader::loadVertexBuffer(const VertexData& vdata)
    class VertexBuffer
    {
        VkBuffer m_VertexBuffer = nullptr;
        VkDeviceMemory m_VertexBufferMemory = nullptr;

        VkBuffer m_IndexBuffer = nullptr;
        VkDeviceMemory m_IndexBufferMemory = nullptr;

        // used by vkCmdDraw()
        int m_VertexCount = 0;
    public:
        VertexBuffer(VkBuffer vertexBuffer, VkDeviceMemory vertexBufferMemory,
                     VkBuffer indexBuffer,  VkDeviceMemory indexBufferMemory,
                     int vertexCount);
        ~VertexBuffer();

        VkBuffer vtxbuffer() { return m_VertexBuffer; }
        VkBuffer idxbuffer() { return m_IndexBuffer; }

        int vertexCount() const { return m_VertexCount; };
    };

    class UniformBuffer
    {
        VkBuffer m_Buffer = nullptr;
        VkDeviceMemory m_BufferMemory = nullptr;
        void* m_BufferMapped = nullptr;  // 'Persistent Mapping' since vkMapMemory costs.

//        VkDevice _devi;  // for vkDestroy..()
//    public:
//        UniformBuffer(VkDevice device, VkDeviceSize bufferSize);
//        ~UniformBuffer();

    public:
        void Create(VkDevice device, VkDeviceSize bufferSize);
        void Destroy(VkDevice device);
        void MemCpy(void* src_ptr, size_t size);

        VkBuffer buffer() { return m_Buffer; };
    };

    struct Image
    {
        VkImage m_Image = nullptr;
        VkDeviceMemory m_ImageMemory = nullptr;
        VkImageView m_ImageView = nullptr;

        Image(VkImage image, VkDeviceMemory imageMemory, VkImageView imageView);
        ~Image();
    };





    // ============ low level encapsulate ============

    // vkAllocateDescriptorSets() helper
    void AllocateDescriptorSets(VkDevice device,
                                VkDescriptorPool descriptorPool,
                                int descriptorSetCount,
                                VkDescriptorSetLayout* descriptorSetLayouts,
                                VkDescriptorSet* out_descriptorSets);  // out

    // vkAllocateMemory() -> VkDeviceMemory
    VkDeviceMemory AllocateMemory(VkDevice device,
                                  VkMemoryRequirements memRequirements,
                                  VkMemoryPropertyFlags memProperties);

    // vkCreateBuffer()
    void CreateBuffer(VkDevice device,
                      VkDeviceSize size,
                      VkBuffer* pBuffer,  // out
                      VkDeviceMemory* pBufferMemory,  // out
                      VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                      VkMemoryPropertyFlags memProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void CreateImage(VkDevice device,
                     int imgWidth, int imgHeight,
                     VkImage* pImage,  // out
                     VkDeviceMemory* pImageMemory,  // out
                     VkFormat format = VK_FORMAT_R8G8B8A8_SRGB,
                     VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                     VkMemoryPropertyFlags memProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL);


    void CreateStagedImage(VkDevice device,
                           int w, int h,
                           void* pixels,  // 4 channel rgba8 pixels
                           VkImage* pImage,  // out
                           VkDeviceMemory* pImageMemory,  // out
                           VkImageView* pImageView);  // out
}





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



    static VkSampler CreateTextureSampler();

    static void CreateDepthTextureImage(int w, int h, VkImage& depthImage, VkDeviceMemory& depthImageMemory, VkImageView& depthImageView);


    // Static buffer. (high effective read on GPU, but cannot visible/modify from CPU)
    static void CreateVertexBuffer(const void* in_data, size_t size,
                                   VkBuffer& out_buffer, VkDeviceMemory& out_bufferMemory, bool bufferUsageIndexBuffer = false);


    static void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);










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
