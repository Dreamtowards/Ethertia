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
                                          int numSetLayouts, VkDescriptorSetLayout* pSetLayouts);

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


//    VkPipeline CreateGraphicsPipeline(VkDevice device);




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


    VkImageView CreateImageView(VkDevice device,
                                VkImage image,
                                VkFormat format = VK_FORMAT_R8G8B8A8_SRGB,
                                VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);



    VkFramebuffer CreateFramebuffer(VkDevice device,
                                    int w, int h,
                                    VkRenderPass renderPass,
                                    int attchCount,
                                    VkImageView* pAttach);

    // vkCreateRenderPass()
    VkRenderPass CreateRenderPass(VkDevice device,
                                  std::span<VkAttachmentDescription> attachments,
                                  std::span<VkSubpassDescription> subpasses,
                                  std::span<VkSubpassDependency> dependencies);







    // vkAllocateCommandBuffers()
    void AllocateCommandBuffers(VkDevice device,
                                VkCommandPool commandPool,
                                int cmdbufCount,
                                VkCommandBuffer* out_cmdbufs,
                                VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);



    // Allocate & Record & Submit Onetime CommandBuffer
    void SubmitCommandBuffer(const std::function<void(VkCommandBuffer)>& fn_record,
                             VkDevice device, VkCommandPool commandPool, VkQueue queue);

    void QueueSubmit(VkQueue queue,
                     VkCommandBuffer cmdbuf,
                     VkSemaphore wait,
                     VkSemaphore signal,
                     VkFence fence);

    void QueuePresentKHR(VkQueue presentQueue,
                         int numWaitSemaphores, const VkSemaphore* pSemaphores,
                         int numSwapchains, const VkSwapchainKHR* pSwapchains, const uint32_t* pImageIndices);

    void CmdBeginRenderPass(VkCommandBuffer cmdbuf,
                            VkRenderPass renderPass,
                            VkFramebuffer framebuffer,
                            VkExtent2D renderAreaExtent,
                            int numClearValues,
                            VkClearValue* pClearValues);

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

    struct Instance
    {
    public:
//        Instance(GLFWwindow* glfwWindow, bool enableValidationLayer = true);
//        ~Instance();

        VkInstance Inst = nullptr;
        VkSurfaceKHR SurfaceKHR = nullptr;
        bool m_EnabledValidationLayer = true;

        VkPhysicalDevice PhysDevice = nullptr;
        VkDevice Device = nullptr;
        VkQueue GraphicsQueue = nullptr;
        VkQueue PresentQueue = nullptr;

        VkCommandPool CommandPool = nullptr;

        VkSampler ImageSampler = nullptr;  // default sampler.
        VkDescriptorPool DescriptorPool = nullptr;

        VkRenderPass _RenderPass = nullptr;  // external. main render pass.
    };

    void ctx(vkx::Instance* inst);
    vkx::Instance& ctx();


    void Init(GLFWwindow* glfwWindow, bool enableValidationLayer);

    void Destroy();





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
        void memcpy(void* src_ptr, size_t size);
        VkBuffer buffer() { return m_Buffer; };
    };





    VkPipeline CreateGraphicsPipeline(
            std::array<std::span<const char>, 2> shaderStagesSources,
            std::initializer_list<VkFormat> vertexInputAttribsFormats,
            VkPrimitiveTopology topology,
            int numColorBlendAttachments,
            std::initializer_list<VkDynamicState> dynamicStates,
            VkPipelineLayout pipelineLayout,
            VkRenderPass renderPass);





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



    void CreateSwapchain(VkDevice device, VkPhysicalDevice physDevice, VkSurfaceKHR surfaceKHR, GLFWwindow* glfwWindow,
                         VkSwapchainKHR& out_SwapchainKHR, VkExtent2D& out_SwapchainExtent,
                         VkFormat& out_SwapchainImageFormat, std::vector<VkImage>& out_SwapchainImages,
                         std::vector<VkImageView>& out_SwapchainImageViews);


    VkFormat findDepthFormat();

    void CreateDepthImage(int w, int h, vkx::Image* img);


    VkSampler CreateImageSampler();

}







class vkh
{
public:






//    static void LoadShaderStages_H(VkPipelineShaderStageCreateInfo* dst, const std::string& spv_filename_pat);
//
//    static void DestroyShaderModules(VkPipelineShaderStageCreateInfo* dst);






    // ============ CreateInfo of VkPipeline ============

//    // Thread unsafe!
//    static VkPipelineVertexInputStateCreateInfo c_PipelineVertexInputState(
//            int numVertBinding = 0,
//            VkVertexInputBindingDescription* pVertBinding = nullptr,
//            int numVertAttr = 0,
//            VkVertexInputAttributeDescription* pVertAttr = nullptr);
//
//    static VkPipelineVertexInputStateCreateInfo c_PipelineVertexInputState_H(
//            std::initializer_list<VkFormat> attribs, int binding = 0);
//
//    static VkPipelineInputAssemblyStateCreateInfo c_PipelineInputAssemblyState(
//            VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
//
//    static VkPipelineRasterizationStateCreateInfo c_PipelineRasterizationState(
//            VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL,
//            VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT,
//            VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE);
//
//    static VkPipelineViewportStateCreateInfo c_PipelineViewportState(
//            int viewportCount = 1, int scissorCount = 1);
//
//    static VkPipelineDepthStencilStateCreateInfo c_PipelineDepthStencilState();
//
//    static VkPipelineMultisampleStateCreateInfo c_PipelineMultisampleState();
//
//    static VkPipelineDynamicStateCreateInfo c_PipelineDynamicState(
//            int numStates = 0, VkDynamicState* pStates = nullptr);
//
//    static VkPipelineDynamicStateCreateInfo c_PipelineDynamicState_H_ViewportScissor();
//
//    static VkPipelineColorBlendAttachmentState c_PipelineColorBlendAttachmentState();
//
//    static VkPipelineColorBlendStateCreateInfo c_PipelineColorBlendState(
//            int numAttach, VkPipelineColorBlendAttachmentState* pAttach);






    static VkAttachmentDescription c_AttachmentDescription(
            VkFormat format = VK_FORMAT_R16G16B16_SFLOAT,
            VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


//    static VkDescriptorSetLayoutBinding c_DescriptorSetLayoutBinding(int bind,
//                                                                     VkDescriptorType descType,
//                                                                     VkShaderStageFlags shaderStageFlags);
};

#endif //ETHERTIA_VKH_H
