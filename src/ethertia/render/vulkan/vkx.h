//
// Created by 56327 on 2023/3/30.
//

#ifndef ETHERTIA_VKH_H
#define ETHERTIA_VKH_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <functional>
#include <span>



static void _vk_check_result(VkResult rs)
{
    if (rs != VK_SUCCESS)
        throw std::runtime_error("[vulkan] Error: VkResult = ");
}
#define VK_CHECK(rs) _vk_check_result(rs)

#define VK_CHECK_MSG(rs, msg) _vk_check_result(rs)




// LowLevel Functional Encapsulate
namespace vl
{
    // vkAllocateDescriptorSets() helper
    void AllocateDescriptorSets(VkDevice device,
                                VkDescriptorPool descriptorPool,
                                int descriptorSetCount,
                                VkDescriptorSetLayout* descriptorSetLayouts,
                                VkDescriptorSet* out_descriptorSets);  // out

    // vkCreateDescriptorSetLayout()
    VkDescriptorSetLayout CreateDescriptorSetLayout(VkDevice device, int numBindings, const VkDescriptorSetLayoutBinding* pBindings);

    VkDescriptorSetLayout CreateDescriptorSetLayout(VkDevice device,
                                                    std::initializer_list<std::pair<VkDescriptorType, VkShaderStageFlags>> bindings);

    VkPipelineLayout CreatePipelineLayout(VkDevice device,
                                          std::span<const VkDescriptorSetLayout> descriptorSetLayouts,
                                          std::span<const VkPushConstantRange> pushConstantRanges = {});

    VkPushConstantRange IPushConstantRange(VkShaderStageFlags shaderStageFlags, uint32_t size, uint32_t offset = 0);

    // vkCreateShaderModule()
    VkPipelineShaderStageCreateInfo CreateShaderModule_IPipelineShaderStage(
            VkDevice device, VkShaderStageFlagBits stage, std::span<const char> code);






    void CreateGraphicsPipelines(VkDevice device,
                         VkPipelineCache pipelineCache,
                         uint32_t createCount,
                         VkPipeline* pPipelines,
                         const VkGraphicsPipelineCreateInfo* pPipelineInfos);

    VkGraphicsPipelineCreateInfo IGraphicsPipeline(
        std::span<const VkPipelineShaderStageCreateInfo> shaderStages,
        const VkPipelineVertexInputStateCreateInfo&      vertexInputState,
        const VkPipelineInputAssemblyStateCreateInfo&    inputAssemblyState,
        const VkPipelineTessellationStateCreateInfo*     pTessellationState,
        const VkPipelineViewportStateCreateInfo&         viewportState,
        const VkPipelineRasterizationStateCreateInfo&    rasterizationState,
        const VkPipelineMultisampleStateCreateInfo&      multisampleState,
        const VkPipelineDepthStencilStateCreateInfo&     depthStencilState,
        const VkPipelineColorBlendStateCreateInfo&       colorBlendState,
        const VkPipelineDynamicStateCreateInfo&          dynamicState,
        VkPipelineLayout                                 layout,
        VkRenderPass                                     renderPass,
        uint32_t                                         subpass = 0,
        VkPipeline                                       basePipelineHandle = VK_NULL_HANDLE,
        int32_t                                          basePipelineIndex = 0);


    VkPipelineVertexInputStateCreateInfo IPipelineVertexInputState(
            std::initializer_list<VkFormat> attribsFormats,
            uint32_t attribBinding,  // always = 0;
            VkVertexInputBindingDescription* out_bindingDesc,  // need to be persistent until pipeline created.
            std::vector<VkVertexInputAttributeDescription>* out_attribsDesc);  // same persistent.

    VkPipelineInputAssemblyStateCreateInfo IPipelineInputAssemblyState(
            VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

    VkPipelineTessellationStateCreateInfo IPipelineTessellationState(uint32_t patchControlPoints);

    // Default: pViewport and pScissors as nullptr remained to Pipeline DynamicState
    VkPipelineViewportStateCreateInfo IPipelineViewportState(
            uint32_t viewportCount = 1, const VkViewport* pViewports = nullptr,
            uint32_t scissorsCount = 1, const VkRect2D* pScissors = nullptr);

    VkPipelineRasterizationStateCreateInfo IPipelineRasterizationState(
            VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL,
            VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT,
            VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE);

    VkPipelineMultisampleStateCreateInfo IPipelineMultisampleState();

    VkPipelineDepthStencilStateCreateInfo IPipelineDepthStencilState();

    VkPipelineColorBlendStateCreateInfo IPipelineColorBlendState(
            std::span<VkPipelineColorBlendAttachmentState> attachments);

    VkPipelineDynamicStateCreateInfo IPipelineDynamicState(
            std::span<const VkDynamicState> dynamicStates);





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
                     VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL,
                     bool creatingCubeMap = false);


    VkImageView CreateImageView(VkDevice device,
                                VkImage image,
                                VkFormat format = VK_FORMAT_R8G8B8A8_SRGB,
                                VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);



    VkFramebuffer CreateFramebuffer(VkDevice device,
                                    VkExtent2D wh,
                                    VkRenderPass renderPass,
                                    std::span<const VkImageView> attachments);

    // vkCreateRenderPass()
    VkRenderPass CreateRenderPass(VkDevice device,
                                  std::span<const VkAttachmentDescription> attachments,
                                  std::span<const VkSubpassDescription> subpasses,
                                  std::span<VkSubpassDependency> dependencies = {});

//    VkRenderPass CreateRenderPass(VkDevice device,
//                                  std::initializer_list<VkAttachmentDescription> attachments,
//                                  std::span<VkSubpassDescription> subpasses,
//                                  std::span<VkSubpassDependency> dependencies = {nullptr, (int)0});

    VkSubpassDescription IGraphicsSubpass(
            std::span<const VkAttachmentReference> colorAttachmentRefs,
            const VkAttachmentReference& depthStencilAttachment = {});

    VkAttachmentDescription IAttachmentDescription(
            VkFormat format = VK_FORMAT_R16G16B16_SFLOAT,
            VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);






    // vkAllocateCommandBuffers()
    void AllocateCommandBuffers(VkDevice device,
                                VkCommandPool commandPool,
                                int cmdbufCount,
                                VkCommandBuffer* out_cmdbufs,
                                VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    // vkBeginCommandBuffer()
    void BeginCommandBuffer(VkCommandBuffer cmdbuf, VkCommandBufferUsageFlags usageFlags);

    // Allocate & Record & Submit Onetime CommandBuffer
    void SubmitCommandBuffer(const std::function<void(VkCommandBuffer)>& fn_record,
                             VkDevice device, VkCommandPool commandPool, VkQueue queue);

    void QueueSubmit(VkQueue queue,
                     VkCommandBuffer cmdbuf,
                     VkSemaphore wait,
                     VkSemaphore signal,
                     VkFence fence);

    VkResult QueuePresentKHR(VkQueue presentQueue,
                             std::span<const VkSemaphore> waitSemaphores,
                             int numSwapchains, const VkSwapchainKHR* pSwapchains, const uint32_t* pImageIndices);

    void CmdBeginRenderPass(VkCommandBuffer cmdbuf,
                            VkRenderPass renderPass,
                            VkFramebuffer framebuffer,
                            VkExtent2D renderAreaExtent,
                            std::span<const VkClearValue> clearValues);

    void CmdSetViewport(VkCommandBuffer cmdbuf,
                        VkExtent2D wh,
                        float x = 0, float y = 0,
                        float minDepth = 0.0f, float maxDepth = 1.0f);

    void CmdSetScissor(VkCommandBuffer cmdbuf,
                       VkExtent2D extent,
                       VkOffset2D offset = {0, 0});

    void CmdBindVertexBuffer(VkCommandBuffer cmdbuf, VkBuffer vbuf);

    void CmdBindIndexBuffer(VkCommandBuffer cmdbuf, VkBuffer idx_buf);

    void CmdBindGraphicsPipeline(VkCommandBuffer cmdbuf, VkPipeline graphicsPipeline);



}





// Global State
// HighLevel Object-Oriented Encapsulate
namespace vkx
{
    class Image;

    const int INFLIGHT_FRAMES = 2;
    inline uint32_t CurrentInflightFrame = 0;   // index
    inline uint32_t CurrentSwapchainImage = 0;  // index

    struct Instance
    {
    public:
//        Instance(GLFWwindow* glfwWindow, bool enableValidationLayer = true);
//        ~Instance();

        VkInstance Inst = nullptr;
        VkSurfaceKHR SurfaceKHR = nullptr;
        bool m_EnabledValidationLayer = true;
        GLFWwindow* WindowHandle = nullptr;
        // std::vector<const char*> ExtraExtensions;  // for Instance & Device

        VkPhysicalDeviceProperties PhysDeviceProperties;
        VkPhysicalDeviceFeatures PhysDeviceFeatures;
        VkPhysicalDevice PhysDevice = nullptr;
        VkDevice Device = nullptr;
        VkQueue GraphicsQueue = nullptr;
        VkQueue PresentQueue = nullptr;

        VkCommandPool CommandPool = nullptr;

        VkSampler ImageSampler = nullptr;  // default sampler. VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT
        VkDescriptorPool DescriptorPool = nullptr;

        // Swapchain
        VkSwapchainKHR              SwapchainKHR = nullptr;
        VkExtent2D                  SwapchainExtent = {};
        VkFormat                    SwapchainImageFormat = {};
        std::vector<VkImage>        SwapchainImages;        // num: swapchain imageCount (always. 2-3). auto destroy by VkSwapchainKHR.
        std::vector<VkImageView>    SwapchainImageViews;
        std::vector<VkFramebuffer>  SwapchainFramebuffers;  // for each Swapchain Image.
        vkx::Image*                 SwapchainDepthImage = nullptr;


        VkCommandBuffer CommandBuffers[vkx::INFLIGHT_FRAMES];

        VkFence         CommandBuffersFences[vkx::INFLIGHT_FRAMES];
        VkSemaphore     SemaphoreImageAcquired[vkx::INFLIGHT_FRAMES];
        VkSemaphore     SemaphoreRenderComplete[vkx::INFLIGHT_FRAMES];


        VkRenderPass MainRenderPass = nullptr;  // external. main render pass.
    };

    void ctx(vkx::Instance* inst);
    vkx::Instance& ctx();


    void Init(GLFWwindow* glfwWindow, bool enableValidationLayer, const std::vector<const char*>& extraExtensions = {});

    void Destroy();


    void BeginFrame(VkCommandBuffer* out_cmdbuf);

    void EndFrame(VkCommandBuffer cmdbuf);


    void BeginMainRenderPass(VkCommandBuffer cmdbuf);
    void EndMainRenderPass(VkCommandBuffer cmdbuf);





    class CommandBuffer
    {
    public:
        VkCommandBuffer m_CommandBuffer;

        void BeginCommandBuffer(VkCommandBufferUsageFlags usage = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
        void EndCommandBuffer();

        void CmdBeginRenderPass(VkRenderPass renderPass,
                                VkFramebuffer framebuffer,
                                VkExtent2D renderAreaExtent,
                                std::span<const VkClearValue> clearValues);
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

        void CmdPushConstants(VkPipelineLayout pipelineLayout,
                              VkShaderStageFlags shaderStageFlags,
                              const void* pValues,
                              uint32_t size, uint32_t offset = 0);

        void CmdDrawIndexed(uint32_t vertex_count);

        void CmdDraw(uint32_t vertex_count);

    };




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


//    VkWriteDescriptorSet IWriteDescriptorCombinedImageSampler();
//
//    void UpdateDescriptorSets(VkDevice device,
//                              std::span<const VkWriteDescriptorSet> writes,
//                              std::span<const VkCopyDescriptorSet> copies = {})
//{
//    vkUpdateDescriptorSets(device, writes.size(), writes.data(), copies.size(), copies.data());
//}


    struct FramebufferAttachment   // aka RenderTarget
    {
        vkx::Image* Image;
        VkAttachmentDescription AttachmentDescription;

        static FramebufferAttachment Create(VkExtent2D wh, VkFormat format, bool depth = false);
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

        VkBuffer vtxbuffer() const { return m_VertexBuffer; }
        VkBuffer idxbuffer() const { return m_IndexBuffer; }

        bool isIndexed() const { return idxbuffer() != nullptr; }

        uint32_t vertexCount() const { return m_VertexCount; };
    };

    struct Image
    {
        VkImage m_Image = nullptr;
        VkDeviceMemory m_ImageMemory = nullptr;
        VkImageView m_ImageView = nullptr;

        int width;
        int height;

        Image(VkImage image, VkDeviceMemory imageMemory, VkImageView imageView, int width, int height);
        ~Image();
    };



    class UniformBuffer
    {
        VkBuffer m_Buffer = nullptr;
        VkDeviceMemory m_BufferMemory = nullptr;
        void* m_BufferMapped = nullptr;  // 'Persistent Mapping' since vkMapMemory costs.

    public:
        UniformBuffer(VkDeviceSize bufferSize);
        ~UniformBuffer();

        // cpy mem to the buffer.
        void update(void* src_ptr, size_t size);
        VkBuffer buffer() { return m_Buffer; };
    };





    VkPipeline CreateGraphicsPipeline(
            std::span<const std::pair<std::span<const char>, VkShaderStageFlagBits>> shaderStagesSources,
            std::initializer_list<VkFormat> vertexInputAttribsFormats,
            VkPrimitiveTopology topology,
            int numColorBlendAttachments,
            std::initializer_list<VkDynamicState> dynamicStates,
            VkPipelineLayout pipelineLayout,
            VkRenderPass renderPass,
            VkCullModeFlagBits cullModeFlags = VK_CULL_MODE_BACK_BIT,
            uint32_t tessControlPoints = 0);  // 0: tess state nullptr.





    // use Global Default device/cmdpool/queue.
    void SubmitCommandBuffer(const std::function<void(VkCommandBuffer)>& fn_record);

    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);


    void CreateStagedImage(int w, int h,
                           void* pixels,  // 4 channel rgba8 pixels
                           VkImage* pImage,  // out
                           VkDeviceMemory* pImageMemory,  // out
                           VkImageView* pImageView);  // out


    // Static buffer. (high effective read on GPU, but cannot visible/modify from CPU)
    void CreateStagedBuffer(const void* bufferData, VkDeviceSize bufferSize,
                            VkBuffer* out_buffer, VkDeviceMemory* out_bufferMemory,
                            VkBufferUsageFlags usage);  // usage Vert: VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, Idx: VK_BUFFER_USAGE_INDEX_BUFFER_BIT




    void RecreateSwapchain();


    VkFormat findDepthFormat();

    void CreateDepthImage(int w, int h, vkx::Image* img);


    VkSampler CreateImageSampler(VkFilter magFilter = VK_FILTER_NEAREST,
                                 VkFilter minFilter = VK_FILTER_NEAREST,
                                 VkSamplerAddressMode addressModeUVW = VK_SAMPLER_ADDRESS_MODE_REPEAT);











}



#endif //ETHERTIA_VKH_H
