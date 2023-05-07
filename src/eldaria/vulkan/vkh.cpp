//
// Created by Dreamtowards on 2023/3/24.
//


#include "vkx.h"


#include <ethertia/util/Strings.h>
#include <ethertia/util/Log.h>
#include <ethertia/util/Collections.h>




///////////////   vl:: Vulkan Low Level Encapsulate   ///////////////


// ================== Pipeline ==================

void vl::AllocateDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool, int descriptorSetCount, VkDescriptorSetLayout* descriptorSetLayouts, VkDescriptorSet* out_descriptorSets)
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = descriptorSetCount;
    allocInfo.pSetLayouts = descriptorSetLayouts;

    VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, out_descriptorSets));
}


VkPipelineShaderStageCreateInfo vl::CreateShaderModule_IPipelineShaderStage(VkDevice device, VkShaderStageFlagBits stage, std::span<const char> code)
{
    VkShaderModuleCreateInfo moduleInfo{};
    moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleInfo.codeSize = code.size();
    moduleInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    VK_CHECK_MSG(vkCreateShaderModule(device, &moduleInfo, nullptr, &shaderModule),
                 "failed to create shader module!");

    VkPipelineShaderStageCreateInfo stageInfo{};
    stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stageInfo.stage = stage;
    stageInfo.module = shaderModule;
    stageInfo.pName = "main";

    return stageInfo;
}

void vl::CreateGraphicsPipelines(VkDevice device,
                              VkPipelineCache pipelineCache,
                              uint32_t createCount,
                              VkPipeline* pPipeline,
                              const VkGraphicsPipelineCreateInfo *pPipelineInfo)
{
    VK_CHECK_MSG(vkCreateGraphicsPipelines(device, pipelineCache, createCount, pPipelineInfo, nullptr, pPipeline),
                 "failed to create pipeline.");
}

VkGraphicsPipelineCreateInfo vl::IGraphicsPipeline(
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
        VkPipelineLayout layout,
        VkRenderPass renderPass,
        uint32_t subpass,
        VkPipeline basePipelineHandle,
        int32_t basePipelineIndex)
{
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = shaderStages.size();
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputState;
    pipelineInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineInfo.pTessellationState = pTessellationState;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizationState;
    pipelineInfo.pMultisampleState = &multisampleState;
    pipelineInfo.pDepthStencilState = &depthStencilState;
    pipelineInfo.pColorBlendState = &colorBlendState;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = layout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = subpass;
    pipelineInfo.basePipelineHandle = basePipelineHandle;
    pipelineInfo.basePipelineIndex = basePipelineIndex;
    return pipelineInfo;
}

static int FormatSize(VkFormat format) {
    switch (format) {
        case VK_FORMAT_R32_SFLOAT:          return 4;
        case VK_FORMAT_R32G32_SFLOAT:       return 8;
        case VK_FORMAT_R32G32B32_SFLOAT:    return 12;
        case VK_FORMAT_R32G32B32A32_SFLOAT: return 16;

        case VK_FORMAT_R16_SFLOAT:          return 2;
        case VK_FORMAT_R16G16_SFLOAT:       return 4;
        case VK_FORMAT_R16G16B16_SFLOAT:    return 6;
        case VK_FORMAT_R16G16B16A16_SFLOAT: return 8;
        default: throw std::runtime_error("Unsupported format");
    }
}

VkPipelineVertexInputStateCreateInfo vl::IPipelineVertexInputState(
        std::initializer_list<VkFormat> attribsFormats,
        uint32_t attribBinding,
        VkVertexInputBindingDescription *out_bindingDesc,
        std::vector<VkVertexInputAttributeDescription> *out_attribsDesc)
{
    std::vector<VkVertexInputAttributeDescription>& attribsDesc = *out_attribsDesc;
    attribsDesc.resize(attribsFormats.size());
    uint32_t i = 0;
    uint32_t offset = 0;
    for (VkFormat attrib_format : attribsFormats)
    {
        attribsDesc[i] = VkVertexInputAttributeDescription{
                .location = i,
                .binding = attribBinding,
                .format = attrib_format,
                .offset = offset
        };
        offset += FormatSize(attrib_format);
        ++i;
    }

    VkVertexInputBindingDescription& bindingDesc = *out_bindingDesc;
    bindingDesc = {};
    bindingDesc.binding = attribBinding;
    bindingDesc.stride = offset;
    bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkPipelineVertexInputStateCreateInfo vertexInputState{};
    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputState.vertexBindingDescriptionCount = attribsFormats.size() ? 1 : 0;  // if no attribs info, this is an empty vertex input, so set 0.
    vertexInputState.pVertexBindingDescriptions = &bindingDesc;
    vertexInputState.vertexAttributeDescriptionCount = attribsDesc.size();
    vertexInputState.pVertexAttributeDescriptions = attribsDesc.data();

    return vertexInputState;
}


VkPipelineInputAssemblyStateCreateInfo vl::IPipelineInputAssemblyState(VkPrimitiveTopology topology)
{
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = topology;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    return inputAssembly;
}

VkPipelineViewportStateCreateInfo vl::IPipelineViewportState(
        uint32_t viewportCount, const VkViewport *pViewports,
        uint32_t scissorsCount, const VkRect2D *pScissors)
{
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = viewportCount;
    viewportState.pViewports = pViewports;
    viewportState.scissorCount = scissorsCount;
    viewportState.pScissors = pScissors;
    return viewportState;
}

VkPipelineRasterizationStateCreateInfo vl::IPipelineRasterizationState(VkPolygonMode polygonMode,
                                                                       VkCullModeFlags cullMode,
                                                                       VkFrontFace frontFace)
{
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.polygonMode = polygonMode;
    rasterizer.cullMode = cullMode;
    rasterizer.frontFace = frontFace;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.lineWidth = 1.0f;
    return rasterizer;
}

VkPipelineMultisampleStateCreateInfo vl::IPipelineMultisampleState()
{
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    return multisampling;
}

VkPipelineDepthStencilStateCreateInfo vl::IPipelineDepthStencilState()
{
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {}; // Optional
    depthStencil.back = {}; // Optional
    return depthStencil;
}


VkPipelineColorBlendStateCreateInfo vl::IPipelineColorBlendState(
        std::span<VkPipelineColorBlendAttachmentState> attachments)
{
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = attachments.size();
    colorBlending.pAttachments = attachments.data();
    return colorBlending;
}


VkPipelineDynamicStateCreateInfo vl::IPipelineDynamicState(std::span<const VkDynamicState> dynamicStates)
{
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = dynamicStates.size();
    dynamicState.pDynamicStates = dynamicStates.data();
    return dynamicState;
}




VkDescriptorSetLayout vl::CreateDescriptorSetLayout(VkDevice device, int numBindings, const VkDescriptorSetLayoutBinding* pBindings)
{
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = numBindings;
    layoutInfo.pBindings = pBindings;

    VkDescriptorSetLayout descriptorSetLayout;
    VK_CHECK_MSG(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout),
                 "failed to create descriptor set layout.");
    return descriptorSetLayout;
}

VkDescriptorSetLayout vl::CreateDescriptorSetLayout(VkDevice device,
                                                    std::initializer_list<std::pair<VkDescriptorType, VkShaderStageFlags>> bindings_info)
{
    std::vector<VkDescriptorSetLayoutBinding> bindings{bindings_info.size()};

    uint32_t i = 0;
    for (auto& bind_info : bindings_info) {
        bindings[i] = {
            .binding = i,
            .descriptorType = bind_info.first,
            .descriptorCount = 1,
            .stageFlags = bind_info.second,
            .pImmutableSamplers = nullptr
        };
        ++i;
    }
    return vl::CreateDescriptorSetLayout(device, bindings.size(), bindings.data());
}

VkPipelineLayout vl::CreatePipelineLayout(VkDevice device,
                                          std::span<const VkDescriptorSetLayout> descriptorSetLayouts,
                                          std::span<const VkPushConstantRange> pushConstantRanges)
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pushConstantRangeCount = pushConstantRanges.size();
    pipelineLayoutInfo.pPushConstantRanges = pushConstantRanges.data();
    pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size();
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

    VkPipelineLayout pipelineLayout;
    VK_CHECK_MSG(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout),
                 "failed to create pipeline layout!");
    return pipelineLayout;
}

VkPushConstantRange vl::IPushConstantRange(VkShaderStageFlags shaderStageFlags, uint32_t size, uint32_t offset)
{
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = shaderStageFlags;
    pushConstantRange.offset = offset;
    pushConstantRange.size = size;
    return pushConstantRange;
}








VkImageView vl::CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.image = image;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;  // except VR.
    //viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    //viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    //viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    //viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    VkImageView imageView;
    VK_CHECK_MSG(vkCreateImageView(device, &viewInfo, nullptr, &imageView), "failed to create texture image view.");

    return imageView;
}



VkFramebuffer vl::CreateFramebuffer(VkDevice device, int w, int h, VkRenderPass renderPass, std::span<const VkImageView> attachments)
{
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.width = w;
    framebufferInfo.height = h;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = attachments.size();
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.layers = 1;

    VkFramebuffer framebuffer;
    VK_CHECK(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer));
    return framebuffer;
}


VkRenderPass vl::CreateRenderPass(VkDevice device,
                                  std::span<const VkAttachmentDescription> attachments,
                                  std::span<VkSubpassDescription> subpasses,
                                  std::span<VkSubpassDependency> dependencies)
{
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = attachments.size();
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = subpasses.size();
    renderPassInfo.pSubpasses = subpasses.data();
    renderPassInfo.dependencyCount = dependencies.size();
    renderPassInfo.pDependencies = dependencies.data();

    VkRenderPass renderPass;
    VK_CHECK_MSG(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass),
                 "failed to create render pass.");
    return renderPass;
}

VkRenderPass vl::CreateRenderPass(VkDevice device,
                                  std::initializer_list<VkAttachmentDescription> attachments,
                                  std::span<VkSubpassDescription> subpasses,
                                  std::span<VkSubpassDependency> dependencies)
{

    return vl::CreateRenderPass(device, {attachments.begin(), (int)attachments.size()}, subpasses, dependencies);
}

VkSubpassDescription vl::IGraphicsSubpass(
        std::span<const VkAttachmentReference> colorAttachmentRefs,
        const VkAttachmentReference* pDepthStencilAttachment)
{
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = colorAttachmentRefs.size();
    subpass.pColorAttachments = colorAttachmentRefs.data();

    subpass.pDepthStencilAttachment = pDepthStencilAttachment;
    return subpass;
}


VkAttachmentDescription vl::IAttachmentDescription(VkFormat format, VkImageLayout finalLayout)
{
    VkAttachmentDescription d{};
    d.format = format;
    d.samples = VK_SAMPLE_COUNT_1_BIT;
    d.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    d.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    d.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    d.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    d.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    d.finalLayout = finalLayout;  // VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL / VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    return d;
}



// ================== Command Buffer ==================

void vl::AllocateCommandBuffers(VkDevice device, VkCommandPool commandPool,
                                int cmdbufCount, VkCommandBuffer* out_cmdbufs, VkCommandBufferLevel level)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = level;
    allocInfo.commandBufferCount = cmdbufCount;

    VK_CHECK_MSG(vkAllocateCommandBuffers(device, &allocInfo, out_cmdbufs),
            "failed to allocate command buffers.");
}

void vl::BeginCommandBuffer(VkCommandBuffer cmdbuf, VkCommandBufferUsageFlags usageFlags)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = usageFlags;
    VK_CHECK(vkBeginCommandBuffer(cmdbuf, &beginInfo));
}

void vl::SubmitCommandBuffer(const std::function<void(VkCommandBuffer)>& fn_record,
                             VkDevice device, VkCommandPool commandPool, VkQueue queue)
{
    VkCommandBuffer cmdbuf;
    vl::AllocateCommandBuffers(device, commandPool, 1, &cmdbuf, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    vl::BeginCommandBuffer(cmdbuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    fn_record(cmdbuf);

    vkEndCommandBuffer(cmdbuf);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdbuf;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(device, commandPool, 1, &cmdbuf);
}



void vl::QueueSubmit(VkQueue queue, VkCommandBuffer cmdbuf, VkSemaphore wait, VkSemaphore signal, VkFence fence)
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdbuf;

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &wait;

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &signal;

    VK_CHECK_MSG(vkQueueSubmit(queue, 1, &submitInfo, fence),
                 "failed to submit draw command buffer.");
}

VkResult vl::QueuePresentKHR(VkQueue presentQueue,
                         int numWaitSemaphores, const VkSemaphore* pSemaphores,
                         int numSwapchains, const VkSwapchainKHR* pSwapchains, const uint32_t* pImageIndices)
{
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = numWaitSemaphores;
    presentInfo.pWaitSemaphores = pSemaphores;
    presentInfo.swapchainCount = numSwapchains;
    presentInfo.pSwapchains = pSwapchains;
    presentInfo.pImageIndices = pImageIndices;

    //VK_CHECK
    return vkQueuePresentKHR(presentQueue, &presentInfo);
}


// no: when pClearValues=nullptr, color/depthStencil will be {0,0,0,1}, {1, 0}
void vl::CmdBeginRenderPass(VkCommandBuffer cmdbuf,
                            VkRenderPass renderPass,
                            VkFramebuffer framebuffer,
                            VkExtent2D renderAreaExtent,
                            int numClearValues,
                            VkClearValue* pClearValues)
{

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = renderAreaExtent;

    renderPassInfo.clearValueCount = numClearValues;
    renderPassInfo.pClearValues = pClearValues;

    vkCmdBeginRenderPass(cmdbuf, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void vl::CmdSetViewport(VkCommandBuffer cmdbuf, VkExtent2D wh, float x, float y, float minDepth, float maxDepth)
{
    VkViewport viewport{};
    viewport.x = x;
    viewport.y = y;
    viewport.width = wh.width;
    viewport.height = wh.height;
    viewport.minDepth = minDepth;
    viewport.maxDepth = maxDepth;
    vkCmdSetViewport(cmdbuf, 0, 1, &viewport);
}

void vl::CmdSetScissor(VkCommandBuffer cmdbuf, VkExtent2D extent, VkOffset2D offset)
{
    VkRect2D scissor{};
    scissor.offset = offset;
    scissor.extent = extent;
    vkCmdSetScissor(cmdbuf, 0, 1, &scissor);
}

void vl::CmdBindVertexBuffer(VkCommandBuffer cmdbuf, const VkBuffer vbuf)
{
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmdbuf, 0, 1, &vbuf, offsets);
}

void vl::CmdBindIndexBuffer(VkCommandBuffer cmdbuf, const VkBuffer idx_buf)
{
    vkCmdBindIndexBuffer(cmdbuf, idx_buf, 0, VK_INDEX_TYPE_UINT32);
}

void vl::CmdBindGraphicsPipeline(VkCommandBuffer cmdbuf, VkPipeline graphicsPipeline)
{
    vkCmdBindPipeline(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
}






// ================== Memory: Allocate, Buffer ==================


static uint32_t _findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vkx::ctx().PhysDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("failed to find suitable memory type!");
}
//VkMemoryRequirements memRequirements / VkDeviceSize size, uint32_t memoryTypeBits,
VkDeviceMemory vl::AllocateMemory(VkDevice device,
                                  VkMemoryRequirements memRequirements,
                                  VkMemoryPropertyFlags memProperties) {
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = _findMemoryType(memRequirements.memoryTypeBits, memProperties);

    VkDeviceMemory deviceMemory;
    VK_CHECK_MSG(vkAllocateMemory(device, &allocInfo, nullptr, &deviceMemory),
                 "failed to allocate device memory.");
    return deviceMemory;
}


void vl::CreateBuffer(VkDevice device, VkDeviceSize size, VkBuffer* pBuffer, VkDeviceMemory* pBufferMemory, VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags memProperties)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateBuffer(device, &bufferInfo, nullptr, pBuffer));

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, *pBuffer, &memRequirements);

    *pBufferMemory = vl::AllocateMemory(device, memRequirements, memProperties);

    VK_CHECK(vkBindBufferMemory(device, *pBuffer, *pBufferMemory, 0));
}






// ================== Image ==================

void vl::CreateImage(VkDevice device, int imgWidth, int imgHeight, VkImage* pImage, VkDeviceMemory* pImageMemory, VkFormat format,
                     VkImageUsageFlags usage, VkMemoryPropertyFlags memProperties, VkImageTiling tiling)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width  = imgWidth;
    imageInfo.extent.height = imgHeight;
    imageInfo.format = format;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = tiling;
    imageInfo.flags = 0; // Optional

    VK_CHECK_MSG(vkCreateImage(device, &imageInfo, nullptr, pImage), "failed to create image.");

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, *pImage, &memRequirements);

    *pImageMemory = vl::AllocateMemory(device, memRequirements, memProperties);

    VK_CHECK(vkBindImageMemory(device, *pImage, *pImageMemory, 0));
}
























vkx::Image::Image(VkImage image, VkDeviceMemory imageMemory, VkImageView imageView, int w, int h)
        : m_Image(image), m_ImageMemory(imageMemory), m_ImageView(imageView), width(w), height(h) {}
vkx::Image::~Image()
{
    VkDevice device = vkx::ctx().Device;
    vkDestroyImage(device, m_Image, nullptr);
    vkFreeMemory(device, m_ImageMemory, nullptr);
    vkDestroyImageView(device, m_ImageView, nullptr);
}


vkx::VertexBuffer::VertexBuffer(VkBuffer vertexBuffer, VkDeviceMemory vertexBufferMemory,
                                VkBuffer indexBuffer,  VkDeviceMemory indexBufferMemory,
                                int vertexCount) : m_VertexBuffer(vertexBuffer),
                                                   m_VertexBufferMemory(vertexBufferMemory),
                                                   m_IndexBuffer(indexBuffer),
                                                   m_IndexBufferMemory(indexBufferMemory),
                                                   m_VertexCount(vertexCount) {}
vkx::VertexBuffer::~VertexBuffer()
{
    VkDevice device = vkx::ctx().Device;
    vkDestroyBuffer(device, m_VertexBuffer, nullptr);
    vkFreeMemory(device, m_VertexBufferMemory, nullptr);

    vkDestroyBuffer(device, m_IndexBuffer, nullptr);
    vkFreeMemory(device, m_IndexBufferMemory, nullptr);
}


vkx::UniformBuffer::UniformBuffer(VkDeviceSize bufferSize)
{
    VkDevice device = vkx::ctx().Device;

    vl::CreateBuffer(device,
                      bufferSize,
                      &m_Buffer,
                      &m_BufferMemory,
                      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    vkMapMemory(device, m_BufferMemory, 0, bufferSize, 0, &m_BufferMapped);
}

vkx::UniformBuffer::~UniformBuffer()
{
    VkDevice device = vkx::ctx().Device;

    vkDestroyBuffer(device, m_Buffer, nullptr);
    vkFreeMemory(device, m_BufferMemory, nullptr);
}

void vkx::UniformBuffer::memcpy(void* src_ptr, size_t size)
{
    std::memcpy(m_BufferMapped, src_ptr, size);
}




// ============== CommandBuffer ==============

void vkx::CommandBuffer::BeginCommandBuffer(VkCommandBufferUsageFlags usage)
{
    vl::BeginCommandBuffer(m_CommandBuffer, usage);
}

void vkx::CommandBuffer::EndCommandBuffer()
{
    VK_CHECK(vkEndCommandBuffer(m_CommandBuffer));
}

// no: when pClearValues=nullptr, color/depthStencil will be {0,0,0,1}, {1, 0}
void vkx::CommandBuffer::CmdBeginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer,
        VkExtent2D renderAreaExtent, int numClearValues, VkClearValue* pClearValues)
{
    vl::CmdBeginRenderPass(m_CommandBuffer, renderPass, framebuffer, renderAreaExtent, numClearValues, pClearValues);
}
void vkx::CommandBuffer::CmdEndRenderPass()
{
    vkCmdEndRenderPass(m_CommandBuffer);
}

void vkx::CommandBuffer::CmdSetViewport(VkExtent2D wh, float x, float y, float minDepth, float maxDepth)
{
    vl::CmdSetViewport(m_CommandBuffer, wh, x, y, minDepth, maxDepth);
}

void vkx::CommandBuffer::CmdSetScissor(VkExtent2D extent, VkOffset2D offset)
{
    vl::CmdSetScissor(m_CommandBuffer, extent, offset);
}

void vkx::CommandBuffer::CmdBindVertexBuffer(VkBuffer vtxbuf)
{
    vl::CmdBindVertexBuffer(m_CommandBuffer, vtxbuf);
}

void vkx::CommandBuffer::CmdBindIndexBuffer(VkBuffer idxbuf)
{
    vl::CmdBindIndexBuffer(m_CommandBuffer, idxbuf);
}

void vkx::CommandBuffer::CmdBindGraphicsPipeline(VkPipeline graphics_pipeline)
{
    vl::CmdBindGraphicsPipeline(m_CommandBuffer, graphics_pipeline);
}

void vkx::CommandBuffer::CmdDrawIndexed(uint32_t vertex_count)
{
    vkCmdDrawIndexed(m_CommandBuffer, vertex_count, 1, 0, 0, 0);
}

void vkx::CommandBuffer::CmdBindDescriptorSets(VkPipelineLayout pipelineLayout, const VkDescriptorSet* pDescriptorSets,
                                               int descriptorSetCount, VkPipelineBindPoint ePipelineBindPoint)
{
    vkCmdBindDescriptorSets(m_CommandBuffer, ePipelineBindPoint, pipelineLayout, 0, descriptorSetCount,
                            pDescriptorSets, 0, nullptr);
}

void vkx::CommandBuffer::CmdPushConstants(VkPipelineLayout pipelineLayout, VkShaderStageFlags shaderStageFlags,
                                          const void *pValues, uint32_t size, uint32_t offset)
{
    vkCmdPushConstants(m_CommandBuffer, pipelineLayout, shaderStageFlags, offset, size, pValues);
}



VkPipelineColorBlendAttachmentState _IPipelineColorBlendAttachmentState()
{
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    return colorBlendAttachment;
}

VkPipeline vkx::CreateGraphicsPipeline(std::array<std::span<const char>, 2> shaderStagesSources,
                                       std::initializer_list<VkFormat> vertexInputAttribsFormats,
                                       VkPrimitiveTopology topology,
                                       int numColorBlendAttachments,
                                       std::initializer_list<VkDynamicState> dynamicStates,
                                       VkPipelineLayout pipelineLayout,
                                       VkRenderPass renderPass)
{
    VkDevice device = vkx::ctx().Device;

    VkPipelineShaderStageCreateInfo shaderStages[2];
    shaderStages[0] = vl::CreateShaderModule_IPipelineShaderStage(device, VK_SHADER_STAGE_VERTEX_BIT, shaderStagesSources[0]);
    shaderStages[1] = vl::CreateShaderModule_IPipelineShaderStage(device, VK_SHADER_STAGE_FRAGMENT_BIT, shaderStagesSources[1]);

    VkVertexInputBindingDescription _vertexInputBindingDescription;
    std::vector<VkVertexInputAttributeDescription> _vertexInputAttributeDescription;

    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachs;
    colorBlendAttachs.resize(numColorBlendAttachments);
    for (int i = 0; i < numColorBlendAttachments; ++i) {
        colorBlendAttachs[i] = _IPipelineColorBlendAttachmentState();
    }

    VkGraphicsPipelineCreateInfo pipelineInfo =
            vl::IGraphicsPipeline(
                    {shaderStages, 2},
                    vl::IPipelineVertexInputState(vertexInputAttribsFormats, 0, &_vertexInputBindingDescription, &_vertexInputAttributeDescription),
                    vl::IPipelineInputAssemblyState(topology),  // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
                    nullptr,
                    vl::IPipelineViewportState(),
                    vl::IPipelineRasterizationState(),
                    vl::IPipelineMultisampleState(),
                    vl::IPipelineDepthStencilState(),
                    vl::IPipelineColorBlendState({colorBlendAttachs.data(), (int)colorBlendAttachs.size()}),
                    vl::IPipelineDynamicState({dynamicStates.begin(), (int)dynamicStates.size()}),
                    pipelineLayout,
                    renderPass);

    VkPipeline pipeline;
    vl::CreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline, &pipelineInfo);

    vkDestroyShaderModule(device, shaderStages[0].module, nullptr);
    vkDestroyShaderModule(device, shaderStages[1].module, nullptr);

    return pipeline;
}




void vkx::SubmitCommandBuffer(const std::function<void(VkCommandBuffer)>& fn_record)
{
    vl::SubmitCommandBuffer(fn_record, vkx::ctx().Device, vkx::ctx().CommandPool, vkx::ctx().GraphicsQueue);
}

void vkx::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    vkx::SubmitCommandBuffer([&](VkCommandBuffer cmdbuf)
    {
        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        vkCmdCopyBuffer(cmdbuf, srcBuffer, dstBuffer, 1, &copyRegion);
    });
}





static void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    vkx::SubmitCommandBuffer([&](VkCommandBuffer cmdbuf)
    {
        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = { width, height, 1 };

        vkCmdCopyBufferToImage(cmdbuf, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               1,&region);
    });
}

static bool _hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}
static void TransitionImageLayout(VkImage image, VkFormat format,
                                  VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if (_hasStencilComponent(format)) {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    } else {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkx::SubmitCommandBuffer([&](VkCommandBuffer cmdbuf)
    {
        vkCmdPipelineBarrier(
                cmdbuf,
                sourceStage, destinationStage,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier
        );
    });
}



void vkx::CreateStagedImage(int imgWidth, int imgHeight, void* pixels,
                            VkImage* out_image,
                            VkDeviceMemory* out_imageMemory,
                            VkImageView* out_imageView)
{
    VkDevice device = vkx::ctx().Device;
    VkDeviceSize imageSize = imgWidth * imgHeight * 4;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    vl::CreateBuffer(device, imageSize, &stagingBuffer, &stagingBufferMemory,
                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, imageSize);
    vkUnmapMemory(device, stagingBufferMemory);


    vl::CreateImage(device, imgWidth, imgHeight,
                    out_image, out_imageMemory,
                    VK_FORMAT_R8G8B8A8_SRGB,
                    VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    TransitionImageLayout(*out_image, VK_FORMAT_R8G8B8A8_SRGB,
                          VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    CopyBufferToImage(stagingBuffer, *out_image, imgWidth, imgHeight);

    TransitionImageLayout(*out_image, VK_FORMAT_R8G8B8A8_SRGB,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);

    *out_imageView = vl::CreateImageView(device, *out_image, VK_FORMAT_R8G8B8A8_SRGB);
}



static VkFormat _findSupportedFormat(const std::vector<VkFormat>& candidates,
                                     VkImageTiling tiling,
                                     VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(vkx::ctx().PhysDevice, format, &props);
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    throw std::runtime_error("failed to find supported format.");
}
VkFormat vkx::findDepthFormat() {
    return _findSupportedFormat(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

void vkx::CreateDepthImage(int w, int h, vkx::Image* depthImage)
{
    VkDevice device = vkx::ctx().Device;
    VkFormat depthFormat = findDepthFormat();

    depthImage->width = w;
    depthImage->height = h;
    vl::CreateImage(device, w,h, &depthImage->m_Image, &depthImage->m_ImageMemory,
                    depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

    depthImage->m_ImageView = vl::CreateImageView(device, depthImage->m_Image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    TransitionImageLayout(depthImage->m_Image, depthFormat,
                          VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}






















// for VkInstance, not VkDevice.
static void CheckValidationLayersSupport(const std::vector<const char*>& validationLayers) {
    uint32_t layerCount = 0;
    if (vkEnumerateInstanceLayerProperties(&layerCount, nullptr) != VK_SUCCESS)
        throw std::runtime_error("ERR");

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool found = false;
        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            throw std::runtime_error(Strings::fmt("required validation layer not available: {} of available {}", layerName, layerCount));
        }
    }
}

inline static VkDebugUtilsMessengerEXT g_DebugMessengerEXT = nullptr;
inline static std::vector<const char*> g_ValidationLayers = {
        "VK_LAYER_KHRONOS_validation"
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugMessengerCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                              VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {

    const char* MSG_SERV = "VERBO";
    if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) MSG_SERV = "INFO";
    else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) MSG_SERV = "WARN";
    else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) MSG_SERV = "ERROR";

    const char* MSG_TYPE = "GENERAL";
    if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) MSG_TYPE = "VALIDATION";
    else if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) MSG_TYPE = "PERFORMANCE";

    if (messageSeverity != VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    {
        std::cerr << Strings::fmt("VkLayer[{}][{}]: ", MSG_SERV, MSG_TYPE) << pCallbackData->pMessage << std::endl;

        std::cerr.flush();
    }

    return VK_FALSE;
}

static void extDestroyDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    assert(func);
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

static VkInstance CreateInstance(bool enableValidationLayer)
{
    VkInstanceCreateInfo instInfo{};
    instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    // Optional Info.
    VkApplicationInfo vkaInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "N/A",
        .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(0, 0, 1),
        .apiVersion = VK_API_VERSION_1_0
    };
    instInfo.pApplicationInfo = &vkaInfo;


    // VkInstance Extensions.
    std::vector<const char*> extensionsRequired;
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (int i = 0; i < glfwExtensionCount; ++i) {
        extensionsRequired.push_back(glfwExtensions[i]);
    }

#ifdef __APPLE__
    // for prevents VK_ERROR_INCOMPATIBLE_DRIVER err on MacOS MoltenVK sdk.
    extensionsRequired.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    instInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    // for supports VK_KHR_portability_subset logical-device-extension on MacOS.
    extensionsRequired.push_back("VK_KHR_get_physical_device_properties2");
#endif

    if (enableValidationLayer) {
        extensionsRequired.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    instInfo.enabledExtensionCount = extensionsRequired.size();
    instInfo.ppEnabledExtensionNames = extensionsRequired.data();


    //    {   // Verbose
    //        uint32_t vkExtensionCount = 0;
    //        vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, nullptr);
    //
    //        VkExtensionProperties vkExtProps[vkExtensionCount];
    //        vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, vkExtProps);
    //
    //        Log::info("{} VkInstance Extension supported: \1", vkExtensionCount);
    //        for (int i = 0; i < vkExtensionCount; ++i) {
    //            std::cout << vkExtProps[i].extensionName << ", ";
    //        }
    //        std::cout << "\n";

    //        Log::info("VkInstance Extensions: \1");
    //        for (auto & i : extensionsRequired) {
    //            std::cout << i << ", ";
    //        }
    //        std::cout << "\n";
    //    }


    // VkInstance Layers
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo{};
    if (enableValidationLayer) {
        CheckValidationLayersSupport(g_ValidationLayers);

        instInfo.enabledLayerCount = g_ValidationLayers.size();
        instInfo.ppEnabledLayerNames = g_ValidationLayers.data();

        debugMessengerInfo.flags = 0;
        debugMessengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugMessengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugMessengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugMessengerInfo.pfnUserCallback = debugMessengerCallback;
        debugMessengerInfo.pUserData = nullptr; // Optional
        instInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugMessengerInfo;
    } else {
        instInfo.enabledLayerCount = 0;
        instInfo.pNext = nullptr;
    }


    VkInstance instance;
    VkResult err = vkCreateInstance(&instInfo, nullptr, &instance);
    if (err) {
        throw std::runtime_error(Strings::fmt("failed to create vulkan instance. ", err));
    }

    if (enableValidationLayer) {
        // Setup EXT DebugMessenger
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            VK_CHECK(func(instance, &debugMessengerInfo, nullptr, &g_DebugMessengerEXT));
        } else {
            throw std::runtime_error("ext DebugMessenger not present.");
        }
    }
    return instance;
}

VkSurfaceKHR CreateSurface(VkInstance instance, GLFWwindow* glfwWindow)
{
    VkSurfaceKHR surfaceKHR;
    if (glfwCreateWindowSurface(instance, glfwWindow, nullptr, &surfaceKHR) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface via glfw.");
    }
    return surfaceKHR;
}

static VkPhysicalDevice PickPhysicalDevice(VkInstance vkInst)
{
    uint32_t gpu_count = 0;
    vkEnumeratePhysicalDevices(vkInst, &gpu_count, nullptr);

    Log::info("GPUs: ", gpu_count);
    assert(gpu_count && "failed to find GPU with vulkan support.");

    std::vector<VkPhysicalDevice> gpus(gpu_count);
    vkEnumeratePhysicalDevices(vkInst, &gpu_count, gpus.data());

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
    return dev;
}



struct QueueFamilyIndices {
    uint32_t m_GraphicsFamily = -1;
    uint32_t m_PresentFamily = -1;  //Surface Present.

    bool isComplete() const {
        return m_GraphicsFamily != -1 && m_PresentFamily != -1;
    }
};
static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surfaceKHR)
{
    QueueFamilyIndices queueFamilyIdxs;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    assert(queueFamilyCount > 0);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    // todo: Find the BestFit queue for specific QueueFlag. https://stackoverflow.com/a/57210037
    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queueFamilyIdxs.m_GraphicsFamily = i;
        }

        VkBool32 supportPresent = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surfaceKHR, &supportPresent);
        if (supportPresent)
            queueFamilyIdxs.m_PresentFamily = i;

        if (queueFamilyIdxs.isComplete())
            break;
        i++;
    }

    return queueFamilyIdxs;
}

#include <set>

static VkDevice CreateLogicalDevice(VkPhysicalDevice physDevice, const QueueFamilyIndices& queueFamily, VkQueue* out_GraphicsQueue, VkQueue* out_PresentQueue)
{
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    // Queue Family
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
    vkGetPhysicalDeviceFeatures(physDevice, &deviceFeatures);
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

    // VK_KHR_swapchain
    VkDevice device;
    if (vkCreateDevice(physDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device.");
    }

    // Get Queue by the way.
    vkGetDeviceQueue(device, queueFamily.m_GraphicsFamily, 0, out_GraphicsQueue);
    vkGetDeviceQueue(device, queueFamily.m_PresentFamily, 0, out_PresentQueue);

    return device;
}

static VkCommandPool CreateCommandPool(VkDevice device, uint32_t queueFamily)
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamily;

    VkCommandPool commandPool;
    VK_CHECK_MSG(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool),
                 "failed to create command pool.");
    return commandPool;
}










struct SwapchainSupportDetails
{
    VkSurfaceCapabilitiesKHR m_Capabilities{};
    std::vector<VkSurfaceFormatKHR> m_Formats;
    std::vector<VkPresentModeKHR> m_PresentModes;

    bool isSwapChainAdequate() const {
        return !m_Formats.empty() && !m_PresentModes.empty();
    }
};
static SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice physDevice, VkSurfaceKHR surface)
{
    SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice, surface, &details.m_Capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &formatCount, nullptr);
    if (formatCount) {
        details.m_Formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &formatCount, details.m_Formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &presentModeCount, nullptr);
    if (presentModeCount) {
        details.m_PresentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &presentModeCount, details.m_PresentModes.data());
    }

    return details;
}

static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
        return {VK_FORMAT_B8G8R8A8_UNORM,VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }

    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return availableFormat;
    }
    return availableFormats[0];
}
static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    if (Collections::find(availablePresentModes, VK_PRESENT_MODE_MAILBOX_KHR) != -1)
        return VK_PRESENT_MODE_MAILBOX_KHR;
    if (Collections::find(availablePresentModes, VK_PRESENT_MODE_IMMEDIATE_KHR) != -1)
        return VK_PRESENT_MODE_IMMEDIATE_KHR;

    return VK_PRESENT_MODE_FIFO_KHR;  // FIFO_KHR is vk guaranteed available.
}
static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* glfwWindow)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;

    int width, height;
    glfwGetFramebufferSize(glfwWindow, &width, &height);
    VkExtent2D extent = { (uint32_t)width, (uint32_t)height };
    extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    return extent;
}


static
void CreateSwapchain(GLFWwindow* glfwWindow             = vkx::ctx().WindowHandle,
                     VkDevice device                    = vkx::ctx().Device,
                     VkPhysicalDevice physDevice        = vkx::ctx().PhysDevice,
                     VkSurfaceKHR surfaceKHR            = vkx::ctx().SurfaceKHR,
                     VkSwapchainKHR& out_SwapchainKHR   = vkx::ctx().SwapchainKHR,
                     VkExtent2D& out_SwapchainExtent    = vkx::ctx().SwapchainExtent,
                     VkFormat& out_SwapchainImageFormat = vkx::ctx().SwapchainImageFormat,
                     std::vector<VkImage>& out_SwapchainImages          = vkx::ctx().SwapchainImages,
                     std::vector<VkImageView>& out_SwapchainImageViews  = vkx::ctx().SwapchainImageViews)
{
    SwapchainSupportDetails swapchainDetails = querySwapchainSupport(physDevice, surfaceKHR);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainDetails.m_Formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapchainDetails.m_PresentModes);
    VkExtent2D extent = chooseSwapExtent(swapchainDetails.m_Capabilities, glfwWindow);
    out_SwapchainExtent = extent;

    // tri buf
    uint32_t imageCount = swapchainDetails.m_Capabilities.minImageCount + 1;
    if (swapchainDetails.m_Capabilities.maxImageCount > 0 && imageCount > swapchainDetails.m_Capabilities.maxImageCount) {
        imageCount = swapchainDetails.m_Capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchainInfo{};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.surface = surfaceKHR;
    swapchainInfo.minImageCount = imageCount;
    swapchainInfo.imageFormat = surfaceFormat.format;
    swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainInfo.imageExtent = extent;
    swapchainInfo.imageArrayLayers = 1;  // normally 1, except VR.
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    out_SwapchainImageFormat = surfaceFormat.format;

    // Image Share Mode. Queue Family.
    QueueFamilyIndices queueFamily = findQueueFamilies(physDevice, surfaceKHR);
    uint32_t queueFamilyIdxs[] = {queueFamily.m_GraphicsFamily, queueFamily.m_PresentFamily};

    if (queueFamily.m_GraphicsFamily != queueFamily.m_PresentFamily) {
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainInfo.queueFamilyIndexCount = 2;
        swapchainInfo.pQueueFamilyIndices = queueFamilyIdxs;
    } else {
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;  // most performance.
        swapchainInfo.queueFamilyIndexCount = 0;  // opt
        swapchainInfo.pQueueFamilyIndices = nullptr;
    }

    swapchainInfo.preTransform = swapchainDetails.m_Capabilities.currentTransform;  // Non transform
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;  // Non Alpha.
    swapchainInfo.presentMode = presentMode;
    swapchainInfo.clipped = VK_TRUE;  // true: not care the pixels behind other windows for vk optims.  but may cause problem when we read the pixels.
    swapchainInfo.oldSwapchain = nullptr;

    VK_CHECK_MSG(vkCreateSwapchainKHR(device, &swapchainInfo, nullptr, &out_SwapchainKHR),
                 "failed to create vk swapchain khr.");

    // Get Swapchain Images.
    int expectedImageCount = imageCount;
    vkGetSwapchainImagesKHR(device, out_SwapchainKHR, &imageCount, nullptr);
    assert(expectedImageCount == imageCount);

    out_SwapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, out_SwapchainKHR, &imageCount, out_SwapchainImages.data());

    // Image Views
    out_SwapchainImageViews.resize(imageCount);
    for (int i = 0; i < imageCount; ++i)
    {
        out_SwapchainImageViews[i] = vl::CreateImageView(device, out_SwapchainImages[i], surfaceFormat.format);
    }
}



static void DestroySwapchain_(
        VkDevice device                                           = vkx::ctx().Device,
        const std::vector<VkFramebuffer>& swapchainFramebuffers   = vkx::ctx().SwapchainFramebuffers,
        const std::vector<VkImageView>& swapchainImageViews       = vkx::ctx().SwapchainImageViews,
        vkx::Image* swapchainDepthImage                           = vkx::ctx().SwapchainDepthImage,
        VkSwapchainKHR swapchainKHR                               = vkx::ctx().SwapchainKHR)
{
    delete swapchainDepthImage;

    for (auto fb : swapchainFramebuffers) {
        vkDestroyFramebuffer(device, fb, nullptr);
    }
    for (auto imageview : swapchainImageViews) {
        vkDestroyImageView(device, imageview, nullptr);
    }
    vkDestroySwapchainKHR(device, swapchainKHR, nullptr);
}



static VkDescriptorPool CreateDescriptorPool_(VkDevice device)
{
    // tho kinda oversize.
    VkDescriptorPoolSize pool_sizes[] =
            {
                    { VK_DESCRIPTOR_TYPE_SAMPLER,               1000 },
                    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,1000 },
                    { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,         1000 },
                    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,         1000 },
                    { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,  1000 },
                    { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,  1000 },
                    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,        1000 },
                    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,        1000 },
                    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,1000 },
                    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,1000 },
                    { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,      1000 }
            };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    VkDescriptorPool descriptorPool;
    VK_CHECK(vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptorPool));
    return descriptorPool;
}


static VkRenderPass CreateMainRenderPass(
        VkDevice device = vkx::ctx().Device,
        VkFormat swapchainImageFormat = vkx::ctx().SwapchainImageFormat)
{


    VkAttachmentDescription attachmentsDesc[] = {
            vl::IAttachmentDescription(swapchainImageFormat, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR),
            vl::IAttachmentDescription(vkx::findDepthFormat(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) // .storeOp: VK_ATTACHMENT_STORE_OP_DONT_CARE
    };

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

    return vl::CreateRenderPass(
            device,
            {attachmentsDesc, std::size(attachmentsDesc)},
            {&subpass, 1},
            {&dependency, 1});
}

static void CreateSwapchainDepthImage(int width     = vkx::ctx().SwapchainExtent.width,
                                      int height    = vkx::ctx().SwapchainExtent.height)
{
    vkx::Image* depthImage = vkx::ctx().SwapchainDepthImage = new vkx::Image(0,0,0,0,0);
    vkx::CreateDepthImage(width, height, depthImage);
}

static void CreateSwapchainFramebuffers(
        VkDevice device                                     = vkx::ctx().Device,
        VkRenderPass renderPass                             = vkx::ctx().MainRenderPass,
        VkExtent2D swapchainExtent                          = vkx::ctx().SwapchainExtent,
        VkImageView depthImageView                          = vkx::ctx().SwapchainDepthImage->m_ImageView,
        const std::vector<VkImageView>& swapchainImageViews = vkx::ctx().SwapchainImageViews,
        std::vector<VkFramebuffer>& out_swapchainFramebuffers = vkx::ctx().SwapchainFramebuffers)
{
    out_swapchainFramebuffers.resize(swapchainImageViews.size());

    for (size_t i = 0; i < swapchainImageViews.size(); i++)
    {
        std::array<VkImageView, 2> attachments = { swapchainImageViews[i], depthImageView };

        out_swapchainFramebuffers[i] = vl::CreateFramebuffer(device,
                                                             swapchainExtent.width, swapchainExtent.height,
                                                             renderPass,
                                                             attachments);
    }
}

void vkx::RecreateSwapchain()
{
    vkDeviceWaitIdle(vkx::ctx().Device);
    DestroySwapchain_();
    Log::info("RecreateSwapchain");

    CreateSwapchain();
    CreateSwapchainDepthImage();
    CreateSwapchainFramebuffers();
}



static void CreateSyncObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkDevice device = vkx::ctx().Device;
    auto& g = vkx::ctx();
    for (int i = 0; i < vkx::INFLIGHT_FRAMES; ++i)
    {
        VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &g.SemaphoreImageAcquired[i]));
        VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &g.SemaphoreRenderComplete[i]));
        VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &g.CommandBuffersFences[i]));
    }
}



void vkx::Init(GLFWwindow* glfwWindow, bool enableValidationLayer)
{
    vkx::Instance* inst = new vkx::Instance();
    vkx::ctx(inst);
    vkx::Instance& i = *inst;
    i.m_EnabledValidationLayer = enableValidationLayer;
    i.WindowHandle = glfwWindow;

    i.Inst =
    CreateInstance(enableValidationLayer);
    i.SurfaceKHR =
    CreateSurface(i.Inst, glfwWindow);

    i.PhysDevice =
    PickPhysicalDevice(i.Inst);

    QueueFamilyIndices queueFamily = findQueueFamilies(i.PhysDevice, i.SurfaceKHR);
    i.Device =
    CreateLogicalDevice(i.PhysDevice, queueFamily, &i.GraphicsQueue, &i.PresentQueue);


    i.CommandPool =
    CreateCommandPool(i.Device, queueFamily.m_GraphicsFamily);
    i.ImageSampler =
    vkx::CreateImageSampler();
    i.DescriptorPool =
    CreateDescriptorPool_(i.Device);

    // Swapchain
    {
        CreateSwapchain();

        i.MainRenderPass =
        CreateMainRenderPass();     // depend: Swapchain format

        CreateSwapchainDepthImage();
        CreateSwapchainFramebuffers();   // depend: DepthTexture, RenderPass
    }

    // CommandBuffers
    vl::AllocateCommandBuffers(i.Device, i.CommandPool, vkx::INFLIGHT_FRAMES, i.CommandBuffers,VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    CreateSyncObjects();
}

void vkx::Destroy()
{
    auto& g = vkx::ctx();
    VkDevice device = g.Device;
    vkDeviceWaitIdle(device);  // blocking.

    for (int i = 0; i < vkx::INFLIGHT_FRAMES; ++i) {
        vkDestroyFence(device, g.CommandBuffersFences[i], nullptr);
        vkDestroySemaphore(device, g.SemaphoreImageAcquired[i], nullptr);
        vkDestroySemaphore(device, g.SemaphoreRenderComplete[i], nullptr);
    }

    vkDestroyRenderPass(device, g.MainRenderPass, nullptr);
    DestroySwapchain_();

    vkDestroyDescriptorPool(device, g.DescriptorPool, nullptr);
    vkDestroySampler(device, g.ImageSampler, nullptr);
    vkDestroyCommandPool(device, g.CommandPool, nullptr);

    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(g.Inst, g.SurfaceKHR, nullptr);

    if (g.m_EnabledValidationLayer) {
        extDestroyDebugMessenger(g.Inst, g_DebugMessengerEXT, nullptr);
    }
    vkDestroyInstance(g.Inst, nullptr);
}



// The Default Instance;
static vkx::Instance* _DefaultInst = nullptr;
void vkx::ctx(vkx::Instance* inst) {
    assert(_DefaultInst == nullptr);
    _DefaultInst = inst;
}
vkx::Instance& vkx::ctx() {
    return *_DefaultInst;
}




void vkx::BeginFrame(VkCommandBuffer* out_cmdbuf)
{
    auto& g = vkx::ctx();
    VkDevice device = g.Device;
    const int frameIdx = vkx::CurrentInflightFrame;
    VkCommandBuffer cmdbuf = g.CommandBuffers[frameIdx];

    // blocking until the CommandBuffer has finished executing
    vkWaitForFences(device, 1, &g.CommandBuffersFences[frameIdx], VK_TRUE, UINT64_MAX);

    // acquire swapchain image, and signal SemaphoreImageAcquired[i] when acquired. (when the presentation engine is finished using the image)
    uint32_t imageIdx;
    vkAcquireNextImageKHR(device, g.SwapchainKHR, UINT64_MAX, g.SemaphoreImageAcquired[frameIdx], nullptr, &imageIdx);
    vkx::CurrentSwapchainImage = imageIdx;

    vkResetFences(device, 1, &g.CommandBuffersFences[frameIdx]);  // reset the fence to the unsignaled state
    vkResetCommandBuffer(cmdbuf, 0);

    vl::BeginCommandBuffer(cmdbuf, VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

    *out_cmdbuf = cmdbuf;
}



void vkx::EndFrame(VkCommandBuffer cmdbuf)
{
    VK_CHECK(vkEndCommandBuffer(cmdbuf));

    auto& g = vkx::ctx();
    uint32_t frameIdx = vkx::CurrentInflightFrame;
    uint32_t imageIdx = vkx::CurrentSwapchainImage;

    // submit the CommandBuffer.
    vl::QueueSubmit(g.GraphicsQueue, cmdbuf,
                    g.SemaphoreImageAcquired[frameIdx], g.SemaphoreRenderComplete[frameIdx],
                    g.CommandBuffersFences[frameIdx]);

    VkResult vkr =
    vl::QueuePresentKHR(g.PresentQueue,
                        1, &g.SemaphoreRenderComplete[frameIdx],
                        1, &g.SwapchainKHR, &imageIdx);

    if (vkr == VK_SUBOPTIMAL_KHR) {
        vkx::RecreateSwapchain();
    }
//    vkQueueWaitIdle(vkx::ctx().PresentQueue);  // BigWaste on GPU.

    vkx::CurrentInflightFrame = (vkx::CurrentInflightFrame + 1) % vkx::INFLIGHT_FRAMES;
}



void vkx::BeginMainRenderPass(VkCommandBuffer cmdbuf)
{
    VkClearValue clearValues[2]{};
    vkx::CommandBuffer cmd{cmdbuf};

    clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    cmd.CmdBeginRenderPass(vkx::ctx().MainRenderPass, vkx::ctx().SwapchainFramebuffers[vkx::CurrentSwapchainImage],
                           vkx::ctx().SwapchainExtent, 2, clearValues);
    cmd.CmdSetViewport(vkx::ctx().SwapchainExtent);
    cmd.CmdSetScissor(vkx::ctx().SwapchainExtent);
}

void vkx::EndMainRenderPass(VkCommandBuffer cmdbuf)
{
    vkCmdEndRenderPass(cmdbuf);
}




//    class GPU
//    {
//    public:
//        bool isDiscrete;
//
//        VkPhysicalDeviceProperties m_Properties;
//        VkPhysicalDeviceFeatures m_Features;
//    };
//    static int RateGPU(VkPhysicalDevice physicalDevice, VkSurfaceKHR surfaceKHR)
//    {
//        VkPhysicalDeviceProperties deviceProperties;
//        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
//
//        VkPhysicalDeviceFeatures deviceFeatures;
//        vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
//
//        if (!deviceFeatures.samplerAnisotropy)
//            return 0;
//
//        QueueFamilyIndices queueFamily = vkh::findQueueFamilies(physicalDevice, surfaceKHR);
//        if (!queueFamily.isComplete())
//            return 0;
//
//        SwapchainSupportDetails swapchainSupport = querySwapchainSupport(physicalDevice, surfaceKHR);
//        if (!swapchainSupport.isSwapChainAdequate())
//            return 0;
//
//        int score = 0;
//        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
//            score += 800;
//        }
//        score += deviceProperties.limits.maxImageDimension2D;
//
//        Log::info("Device ", deviceProperties.deviceName);
//        return score;
//    }













VkSampler vkx::CreateImageSampler()
{
    VkDevice device = vkx::ctx().Device;
    VkPhysicalDevice physDevice = vkx::ctx().PhysDevice;

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    VkPhysicalDeviceProperties properties{};  // todo optim.
    vkGetPhysicalDeviceProperties(physDevice, &properties);
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    VkSampler textureSampler;
    VK_CHECK_MSG(
            vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler),
            "failed to create texture sampler!");
    return textureSampler;
}







void vkx::CreateStagedBuffer(const void* bufferData, VkDeviceSize bufferSize, VkBuffer* out_buffer, VkDeviceMemory* out_bufferMemory, VkBufferUsageFlags usage)
{
    VkDevice device = vkx::ctx().Device;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    vl::CreateBuffer(device, bufferSize, &stagingBuffer, &stagingBufferMemory,
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* mapped_dst;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &mapped_dst);
    memcpy(mapped_dst, bufferData, bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    vl::CreateBuffer(device, bufferSize, out_buffer, out_bufferMemory,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    vkx::CopyBuffer(stagingBuffer, *out_buffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, nullptr);
    vkFreeMemory(device, stagingBufferMemory, nullptr);
}

















