//
// Created by Dreamtowards on 2023/3/16.
//


#include <map>
#include <set>
#include <chrono>
#include <array>

#include <eldaria/Loader.h>

#include <ethertia/util/Collections.h>
#include <ethertia/util/Log.h>

#include <glm/glm.hpp>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>



#include "imgui/Imgui.h"

#include "VulkanIntl.h"


class vkh
{
public:
    // needs manual set from Reals.
    inline static VkDevice g_Device = nullptr;
    inline static VkPhysicalDevice g_PhysDevice = nullptr;
    inline static VkCommandPool g_CommandPool = nullptr;
    inline static VkQueue       g_GraphicsQueue = nullptr;



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


    static VkShaderModule LoadShaderModule(const void* data, size_t size)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = size;
        createInfo.pCode = reinterpret_cast<const uint32_t*>(data);

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(g_Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }

    static VkPipelineShaderStageCreateInfo LoadShaderStage(VkShaderStageFlagBits stage, const std::string& spv_filename)
    {
        auto src = Loader::loadFile(spv_filename);
        VkShaderModule shaderModule = LoadShaderModule(src.data(), src.size());

        VkPipelineShaderStageCreateInfo shaderInfo{};
        shaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderInfo.stage = stage;
        shaderInfo.module = shaderModule;
        shaderInfo.pName = "main";

        return shaderInfo;
    }

    static void LoadShaderStages_H(
            VkPipelineShaderStageCreateInfo* dst,
            const std::string& spv_filename_pat)
    {
        dst[0] = LoadShaderStage(VK_SHADER_STAGE_VERTEX_BIT,
                                 Loader::fileResolve(Strings::fmt(spv_filename_pat, "vert")));
        dst[1] = LoadShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT,
                                 Loader::fileResolve(Strings::fmt(spv_filename_pat, "frag")));
    }

    static void DestroyShaderModules(VkPipelineShaderStageCreateInfo* dst) {
        vkDestroyShaderModule(g_Device, dst[0].module, nullptr);
        vkDestroyShaderModule(g_Device, dst[1].module, nullptr);
    }

    // Thread unsafe!
    static VkPipelineVertexInputStateCreateInfo c_PipelineVertexInputState(
            int numVertBinding = 0,
            VkVertexInputBindingDescription* pVertBinding = nullptr,
            int numVertAttr = 0,
            VkVertexInputAttributeDescription* pVertAttr = nullptr)
    {
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = numVertBinding;
        vertexInputInfo.pVertexBindingDescriptions = pVertBinding;
        vertexInputInfo.vertexAttributeDescriptionCount = numVertAttr;
        vertexInputInfo.pVertexAttributeDescriptions = pVertAttr;
        return vertexInputInfo;
    }
    static VkPipelineVertexInputStateCreateInfo c_PipelineVertexInputState_H(
            std::initializer_list<VkFormat> attribs, int binding = 0) {
        static std::vector<VkVertexInputAttributeDescription> _attrDesc{};
        _attrDesc.clear();
        int i = 0;
        int offset = 0;
        for (auto& attr_format : attribs) {
            VkVertexInputAttributeDescription d{};
            d.binding = binding;
            d.location = i;
            d.format = attr_format;
            d.offset = offset;
            _attrDesc.push_back(d);
            ++i;
            offset += vkh::FormatSize(attr_format);
        }
        static VkVertexInputBindingDescription _bindingDesc{};
        _bindingDesc.binding = binding;
        _bindingDesc.stride = offset;
        _bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return c_PipelineVertexInputState(1, &_bindingDesc, _attrDesc.size(), _attrDesc.data());
    }

    static VkPipelineInputAssemblyStateCreateInfo c_PipelineInputAssemblyState(
            VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST) {
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = topology;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
        return inputAssembly;
    }
    static VkPipelineRasterizationStateCreateInfo c_PipelineRasterizationState(
            VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL,
            VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT,
            VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE) {
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
    static VkPipelineViewportStateCreateInfo c_PipelineViewportState(
            int viewportCount = 1, int scissorCount = 1) {
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = viewportCount;
        viewportState.scissorCount = scissorCount;
        return viewportState;
    }
    static VkPipelineDepthStencilStateCreateInfo c_PipelineDepthStencilState() {
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
    static VkPipelineMultisampleStateCreateInfo c_PipelineMultisampleState() {
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        return multisampling;
    }
    static VkPipelineDynamicStateCreateInfo c_PipelineDynamicState(int numStates = 0, VkDynamicState* pStates = nullptr) {
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = numStates;
        dynamicState.pDynamicStates = pStates;
        return dynamicState;
    }
    static VkPipelineColorBlendAttachmentState c_PipelineColorBlendAttachmentState() {
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        return colorBlendAttachment;
    }
    static VkPipelineColorBlendStateCreateInfo c_PipelineColorBlendState(int numAttach, VkPipelineColorBlendAttachmentState* pAttach) {
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = numAttach;
        colorBlending.pAttachments = pAttach;
        return colorBlending;
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

    static VkAttachmentDescription c_AttachmentDescription(VkFormat format = VK_FORMAT_R16G16B16_SFLOAT, VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
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
    static VkFramebufferCreateInfo c_Framebuffer(int w, int h,
                                                 VkRenderPass renderPass,
                                                 int attchCount,
                                                 VkImageView* pAttach)
    {
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.width = w;
        framebufferInfo.height = h;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = attchCount;
        framebufferInfo.pAttachments = pAttach;
        framebufferInfo.layers = 1;
        return framebufferInfo;
    }




    // (0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
    static VkDescriptorSetLayoutBinding c_DescriptorSetLayoutBinding(int bind,
                                                                     VkDescriptorType descType,
                                                                     VkShaderStageFlags shaderStageFlags) {
        VkDescriptorSetLayoutBinding binding{};
        binding.binding = bind;
        binding.descriptorType = descType;
        binding.descriptorCount = 1;
        binding.stageFlags = shaderStageFlags;
        return binding;
    }
    static VkDescriptorSetLayout CreateDescriptorSetLayout(std::initializer_list<VkDescriptorSetLayoutBinding> bindings)
    {
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = bindings.size();
        layoutInfo.pBindings = bindings.begin();

        VkDescriptorSetLayout descriptorSetLayout;
        VK_CHECK_MSG(
                vkCreateDescriptorSetLayout(g_Device, &layoutInfo, nullptr, &descriptorSetLayout),
                "failed to create descriptor set layout.");
        return descriptorSetLayout;
    }

//        static VkPipelineLayoutCreateInfo c_PipelineLayout(int numSetLayouts, VkDescriptorSetLayout* pSetLayouts)
//        {
//            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
//            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//            pipelineLayoutInfo.pushConstantRangeCount = 0;
//            pipelineLayoutInfo.pPushConstantRanges = nullptr;
//            pipelineLayoutInfo.setLayoutCount = numSetLayouts;
//            pipelineLayoutInfo.pSetLayouts = pSetLayouts;
//            return pipelineLayoutInfo;
//        }
    static VkPipelineLayout CreatePipelineLayout(int numSetLayouts, VkDescriptorSetLayout* pSetLayouts) {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        pipelineLayoutInfo.setLayoutCount = numSetLayouts;
        pipelineLayoutInfo.pSetLayouts = pSetLayouts;

        VkPipelineLayout pipelineLayout;
        VK_CHECK_MSG(
                vkCreatePipelineLayout(g_Device, &pipelineLayoutInfo, nullptr, &pipelineLayout),
                "failed to create pipeline layout!");
        return pipelineLayout;
    }


    static uint32_t _findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(g_PhysDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }
    //VkMemoryRequirements memRequirements / VkDeviceSize size, uint32_t memoryTypeBits,
    static VkDeviceMemory AllocateMemory(VkMemoryRequirements memRequirements,
                                         VkMemoryPropertyFlags memProperties) {
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = _findMemoryType(memRequirements.memoryTypeBits, memProperties);

        VkDeviceMemory deviceMemory;
        VK_CHECK_MSG(vkAllocateMemory(g_Device, &allocInfo, nullptr, &deviceMemory),
                     "failed to allocate device memory.");
        return deviceMemory;
    }










    static void CreateBuffer(VkDeviceSize size,
                             VkBuffer& buffer,
                             VkDeviceMemory& bufferMemory,
                             VkBufferUsageFlags usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                             VkMemoryPropertyFlags memProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VK_CHECK_MSG(
                vkCreateBuffer(g_Device, &bufferInfo, nullptr, &buffer),
                "failed to create a vertex buffer");

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(g_Device, buffer, &memRequirements);

        bufferMemory = vkh::AllocateMemory(memRequirements, memProperties);

        vkBindBufferMemory(g_Device, buffer, bufferMemory, 0);
    }

    static void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
    {
        VkCommandBuffer cmdbuf = BeginOnetimeCommandBuffer();

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        vkCmdCopyBuffer(cmdbuf, srcBuffer, dstBuffer, 1, &copyRegion);

        EndOnetimeCommandBuffer(cmdbuf);
    }



    static void CreateImage(int texWidth, int texHeight,
                            VkImage& image,
                            VkDeviceMemory& imageMemory,
                            VkFormat format = VK_FORMAT_R8G8B8A8_SRGB,
                            VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                            VkMemoryPropertyFlags memProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                            VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL)
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width  = texWidth;
        imageInfo.extent.height = texHeight;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0; // Optional

        VK_CHECK_MSG(vkCreateImage(g_Device, &imageInfo, nullptr, &image),
                     "failed to create image.");

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(g_Device, image, &memRequirements);

        imageMemory = vkh::AllocateMemory(memRequirements, memProperties);

        VK_CHECK(vkBindImageMemory(g_Device, image, imageMemory, 0));
    }

    // Common
    static VkImageView CreateImageView(VkImage image, VkFormat format = VK_FORMAT_R8G8B8A8_SRGB, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
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
        VK_CHECK_MSG(
                vkCreateImageView(g_Device, &viewInfo, nullptr, &imageView),
                "failed to create texture image view.");

//        if (vkCreateImageView(g_Device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
//            throw std::runtime_error("failed to create texture image view.");
//        }
        return imageView;
    }


    static VkSampler CreateTextureSampler()
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        VkPhysicalDeviceProperties properties{};  // todo optim.
        vkGetPhysicalDeviceProperties(g_PhysDevice, &properties);
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
                vkCreateSampler(g_Device, &samplerInfo, nullptr, &textureSampler),
                "failed to create texture sampler!");
        return textureSampler;
    }









    // Static buffer. (high effective read on GPU, but cannot visible/modify from CPU)
    static void CreateVertexBuffer(const void* in_data, size_t size, VkBuffer& out_buffer, VkDeviceMemory& out_bufferMemory)
    {
        VkDeviceSize bufferSize = size;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        CreateBuffer(bufferSize, stagingBuffer, stagingBufferMemory,
                     VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        void* mapped_dst;
        vkMapMemory(g_Device, stagingBufferMemory, 0, bufferSize, 0, &mapped_dst);
        memcpy(mapped_dst, in_data, bufferSize);
        vkUnmapMemory(g_Device, stagingBufferMemory);

        CreateBuffer(bufferSize, out_buffer, out_bufferMemory,
                     VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        CopyBuffer(stagingBuffer, out_buffer, bufferSize);

        vkDestroyBuffer(g_Device, stagingBuffer, nullptr);
        vkFreeMemory(g_Device, stagingBufferMemory, nullptr);
    }

    static void CreateTextureImage(BitmapImage& bitmapImage, VkImage& out_image, VkDeviceMemory& out_imageMemory, VkImageView* out_imageView = nullptr)
    {
        int texWidth = bitmapImage.m_Width;
        int texHeight = bitmapImage.m_Height;
        void* pixels = bitmapImage.m_Pixels;
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        vkh::CreateBuffer(imageSize, stagingBuffer, stagingBufferMemory,
                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        void* data;
        vkMapMemory(g_Device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, imageSize);
        vkUnmapMemory(g_Device, stagingBufferMemory);


        CreateImage(texWidth, texHeight,
                    out_image, out_imageMemory,
                    VK_FORMAT_R8G8B8A8_SRGB,
                    VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

        TransitionImageLayout(out_image, VK_FORMAT_R8G8B8A8_SRGB,
                              VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        CopyBufferToImage(stagingBuffer, out_image, texWidth, texHeight);

        TransitionImageLayout(out_image, VK_FORMAT_R8G8B8A8_SRGB,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkDestroyBuffer(g_Device, stagingBuffer, nullptr);
        vkFreeMemory(g_Device, stagingBufferMemory, nullptr);

        if (out_imageView)
        {
            *out_imageView = vkh::CreateImageView(out_image, VK_FORMAT_R8G8B8A8_SRGB);
        }
    }


    static void CreateDepthTextureImage(int w, int h, VkImage& depthImage, VkDeviceMemory& depthImageMemory, VkImageView& depthImageView)
    {
        VkFormat depthFormat = findDepthFormat();

        vkh::CreateImage(w,h, depthImage, depthImageMemory,
                    depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

        depthImageView = CreateImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

        vkh::TransitionImageLayout(depthImage, depthFormat,
                              VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    }


    static void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
    {
        VkCommandBuffer cmdbuf = BeginOnetimeCommandBuffer();

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

        vkCmdCopyBufferToImage(cmdbuf, buffer, image,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               1,&region);

        EndOnetimeCommandBuffer(cmdbuf);
    }



    static VkFormat _findSupportedFormat(const std::vector<VkFormat>& candidates,
                                        VkImageTiling tiling,
                                        VkFormatFeatureFlags features) {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(g_PhysDevice, format, &props);
            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }
        throw std::runtime_error("failed to find supported format.");
    }
    static VkFormat findDepthFormat() {
        return _findSupportedFormat(
                {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                VK_IMAGE_TILING_OPTIMAL,
                VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }





    static void TransitionImageLayout(VkImage image, VkFormat format,
                                      VkImageLayout oldLayout, VkImageLayout newLayout)
    {
        VkCommandBuffer cmdbuf = BeginOnetimeCommandBuffer();

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

        vkCmdPipelineBarrier(
                cmdbuf,
                sourceStage, destinationStage,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier
        );

        EndOnetimeCommandBuffer(cmdbuf);
    }
    static bool _hasStencilComponent(VkFormat format) {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }






    static VkCommandBuffer BeginOnetimeCommandBuffer()
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = g_CommandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer cmdbuf;
        vkAllocateCommandBuffers(g_Device, &allocInfo, &cmdbuf);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(cmdbuf, &beginInfo);

        return cmdbuf;
    }
    static void EndOnetimeCommandBuffer(VkCommandBuffer cmdbuf)
    {
        vkEndCommandBuffer(cmdbuf);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmdbuf;

        vkQueueSubmit(g_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(g_GraphicsQueue);

        vkFreeCommandBuffers(g_Device, g_CommandPool, 1, &cmdbuf);
    }
    static void SubmitOnetimeCommandBuffer(const std::function<void(VkCommandBuffer)>& fn_record)
    {
        VkCommandBuffer cmdbuf = BeginOnetimeCommandBuffer();

        fn_record(cmdbuf);

        EndOnetimeCommandBuffer(cmdbuf);
    }







};


struct Vertex
{
    glm::vec3 pos;
    glm::vec3 col;
    glm::vec2 tex;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }


    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, col);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, tex);
        return attributeDescriptions;
    }
};

int g_DrawVerts = 0;
inline static VkBuffer g_VertexBuffer = nullptr;
inline static VkDeviceMemory g_VertexBufferMemory;


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

    inline static VkDescriptorSetLayout g_DescriptorSetLayout = nullptr;
    inline static VkPipelineLayout  g_PipelineLayout = nullptr;
    inline static VkPipeline        g_GraphicsPipeline = nullptr;

    inline static VkCommandPool g_CommandPool = nullptr;
    inline static std::vector<VkCommandBuffer> g_CommandBuffers;

    inline static std::vector<VkSemaphore> g_SemaphoreImageAcquired;  // for each InflightFrame   ImageAcquired, RenderComplete
    inline static std::vector<VkSemaphore> g_SemaphoreRenderComplete;
    inline static std::vector<VkFence>     g_InflightFence;

    inline static const int MAX_FRAMES_INFLIGHT = 2;
    inline static int g_CurrentFrameInflight = 0;

    inline static std::vector<VkBuffer> g_UniformBuffers;  // for each InflightFrame
    inline static std::vector<VkDeviceMemory> g_UniformBuffersMemory;
    inline static std::vector<void*> g_UniformBuffersMapped;  // 'Persistent Mapping' since vkMapMemory cost.

    inline static VkDescriptorPool g_DescriptorPool = nullptr;
    inline static std::vector<VkDescriptorSet> g_DescriptorSets;  // for each InflightFrame

    inline static VkImage g_TextureImage = nullptr;
    inline static VkDeviceMemory g_TextureImageMemory = nullptr;
    inline static VkImageView g_TextureImageView = nullptr;
    inline static VkSampler g_TextureSampler = nullptr;

    inline static VkImage g_DepthImage = nullptr;
    inline static VkDeviceMemory g_DepthImageMemory = nullptr;
    inline static VkImageView g_DepthImageView = nullptr;

    inline static bool g_RecreateSwapchainRequested = false;  // when Window/Framebuffer resized.
    inline static GLFWwindow* g_WindowHandle = nullptr;

    inline static bool g_EnableValidationLayer = true;
    inline static std::vector<const char*> g_ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };
    inline static VkDebugUtilsMessengerEXT g_DebugMessengerEXT = nullptr;


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


    static void Init(GLFWwindow* glfwWindow)
    {
        CreateVkInstance();
        CreateSurface(g_WindowHandle=glfwWindow);

        PickPhysicalDevice();
        CreateLogicalDevice();

        CreateCommandPool();
        CreateCommandBuffers();
        CreateSyncObjects_Semaphores_Fences();
        vkh::g_Device = g_Device;
        vkh::g_PhysDevice = g_PhysDevice;
        vkh::g_GraphicsQueue = g_GraphicsQueue;
        vkh::g_CommandPool = g_CommandPool;

        CreateSwapchainAndImageViews();
        CreateRenderPass();     // depend: Swapchain format
        CreateDepthTexture();
        CreateFramebuffers();   // depend: DepthTexture, RenderPass

        g_TextureSampler = vkh::CreateTextureSampler();

        CreateDescriptorSetLayout();
        CreateGraphicsPipeline();  // depend: RenderPass, DescriptorSetLayout



        {
            BitmapImage bitmapImage = Loader::loadPNG("/Users/dreamtowards/Downloads/viking_room.png");
            vkh::CreateTextureImage(bitmapImage, g_TextureImage, g_TextureImageMemory, &g_TextureImageView);

            VertexData vdata = Loader::loadOBJ("/Users/dreamtowards/Downloads/viking_room.obj");
            g_DrawVerts = vdata.vertexCount();
            vkh::CreateVertexBuffer(vdata.data(), vdata.size(), g_VertexBuffer, g_VertexBufferMemory);
        }

        CreateUniformBuffers();
        CreateDescriptorPool();
        CreateDescriptorSets();


//        InitDeferredRendering();

    }

    static void Destroy()
    {
        DestroySwapchain();

        vkDestroySampler(g_Device, g_TextureSampler, nullptr);
        vkDestroyImageView(g_Device, g_TextureImageView, nullptr);
        vkDestroyImage(g_Device, g_TextureImage, nullptr);
        vkFreeMemory(g_Device, g_TextureImageMemory, nullptr);

        vkDestroyDescriptorPool(g_Device, g_DescriptorPool, nullptr);

        vkDestroyDescriptorSetLayout(g_Device, g_DescriptorSetLayout, nullptr);
        for (int i = 0; i < MAX_FRAMES_INFLIGHT; ++i) {
            vkDestroyBuffer(g_Device, g_UniformBuffers[i], nullptr);
            vkFreeMemory(g_Device, g_UniformBuffersMemory[i], nullptr);
        }

        vkDestroyBuffer(g_Device, g_VertexBuffer, nullptr);
        vkFreeMemory(g_Device, g_VertexBufferMemory, nullptr);

        vkDestroyPipeline(g_Device, g_GraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(g_Device, g_PipelineLayout, nullptr);
        vkDestroyRenderPass(g_Device, g_RenderPass, nullptr);

        for (int i = 0; i < MAX_FRAMES_INFLIGHT; ++i) {
            vkDestroySemaphore(g_Device, g_SemaphoreImageAcquired[i], nullptr);
            vkDestroySemaphore(g_Device, g_SemaphoreRenderComplete[i], nullptr);
            vkDestroyFence(g_Device, g_InflightFence[i], nullptr);
        }
        vkDestroyCommandPool(g_Device, g_CommandPool, nullptr);

        vkDestroyDevice(g_Device, nullptr);

        if (g_EnableValidationLayer) {
            extDestroyDebugMessenger(g_Instance, g_DebugMessengerEXT, nullptr);
        }
        vkDestroySurfaceKHR(g_Instance, g_SurfaceKHR, nullptr);
        vkDestroyInstance(g_Instance, nullptr);
    }


    static void CreateVkInstance()
    {
        VkInstanceCreateInfo instInfo{};
        instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

        // Optional Info.
        VkApplicationInfo vkAppInfo{};
        vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        vkAppInfo.pApplicationName = "N/A";
        vkAppInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
        vkAppInfo.pEngineName = "No Engine";
        vkAppInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
        vkAppInfo.apiVersion = VK_API_VERSION_1_0;
        instInfo.pApplicationInfo = &vkAppInfo;


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

        if (g_EnableValidationLayer) {
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
        VkDebugUtilsMessengerCreateInfoEXT debugMessagerInfo;
        if (g_EnableValidationLayer) {
            vkh::CheckValidationLayersSupport(g_ValidationLayers);

            instInfo.enabledLayerCount = g_ValidationLayers.size();
            instInfo.ppEnabledLayerNames = g_ValidationLayers.data();

            debugMessagerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debugMessagerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            debugMessagerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            debugMessagerInfo.pfnUserCallback = debugMessengerCallback;
            debugMessagerInfo.pUserData = nullptr; // Optional
            instInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugMessagerInfo;
        } else {
            instInfo.enabledLayerCount = 0;
            instInfo.pNext = nullptr;
        }


        VkResult err = vkCreateInstance(&instInfo, nullptr, &g_Instance);
        if (err) {
            throw std::runtime_error(Strings::fmt("failed to create vulkan instance. ", err));
        }

        if (g_EnableValidationLayer) {
            // Setup EXT DebugMessenger
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(g_Instance, "vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr) {
                VK_CHECK(func(g_Instance, &debugMessagerInfo, nullptr, &g_DebugMessengerEXT));
            } else {
                throw std::runtime_error("ext DebugMessenger not present.");
            }
        }
    }


    static VKAPI_ATTR VkBool32 VKAPI_CALL debugMessengerCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {

        const char* MSG_SERV = "VERBO";
        if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) MSG_SERV = "INFO";
        else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) MSG_SERV = "WARN";
        else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) MSG_SERV = "ERROR";

        const char* MSG_TYPE = "GENERAL";
        if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) MSG_TYPE = "VALIDATION";
        else if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) MSG_TYPE = "PERFORMANCE";

    //    if (messageSeverity != VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
        std::cerr << Strings::fmt("VkLayer[{}][{}]: ", MSG_SERV, MSG_TYPE) << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
    static void extDestroyDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        assert(func);
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }







    static void CreateSurface(GLFWwindow* glfwWindow)
    {
        if (glfwCreateWindowSurface(g_Instance, glfwWindow, nullptr, &g_SurfaceKHR) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface via glfw.");
        }
    }








    static void PickPhysicalDevice()
    {
        uint32_t gpu_count = 0;
        vkEnumeratePhysicalDevices(g_Instance, &gpu_count, nullptr);

        Log::info("GPUs: ", gpu_count);
        if (gpu_count == 0)
            throw std::runtime_error("failed to find GPU with vulkan support.");

        std::vector<VkPhysicalDevice> gpus(gpu_count);
        vkEnumeratePhysicalDevices(g_Instance, &gpu_count, gpus.data());

        std::multimap<int, VkPhysicalDevice> candidates;
        for (const auto& device : gpus) {
            int score = ratePhysicalDeviceSuitable(device);
            candidates.insert(std::make_pair(score, device));
        }

        if (candidates.cbegin()->first > 0) {
            g_PhysDevice = candidates.cbegin()->second;
        } else {
            throw std::runtime_error("failed to find a suitable GPU.");
        }
    }

    static int ratePhysicalDeviceSuitable(VkPhysicalDevice physicalDevice)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

        if (!deviceFeatures.samplerAnisotropy)
            return 0;

        QueueFamilyIndices queueFamily = findQueueFamilies(physicalDevice);
        if (!queueFamily.isComplete())
            return 0;

        SwapchainSupportDetails swapchainSupport = querySwapchainSupport(physicalDevice, g_SurfaceKHR);
        if (!swapchainSupport.isSwapChainAdequate())
            return 0;

        int score = 0;
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 800;
        }
        score += deviceProperties.limits.maxImageDimension2D;

        Log::info("Device ", deviceProperties.deviceName);
        return score;
    }










    static void CreateLogicalDevice()
    {
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        // Queue Family
        QueueFamilyIndices queueFamily = findQueueFamilies(g_PhysDevice);
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
        //vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
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

        // Device Validation Layer: already deprecated. ValidationLayer only belongs to VkInstance.
        if (g_EnableValidationLayer) {
            createInfo.ppEnabledLayerNames = g_ValidationLayers.data();
            createInfo.enabledLayerCount = g_ValidationLayers.size();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        // VK_KHR_swapchain
        if (vkCreateDevice(g_PhysDevice, &createInfo, nullptr, &g_Device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device.");
        }

        // Get Queue by the way.
        vkGetDeviceQueue(g_Device, queueFamily.m_GraphicsFamily, 0, &g_GraphicsQueue);
        vkGetDeviceQueue(g_Device, queueFamily.m_PresentFamily, 0, &g_PresentQueue);
    }

    static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice)
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
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, g_SurfaceKHR, &supportPresent);
            if (supportPresent)
                queueFamilyIdxs.m_PresentFamily = i;

            if (queueFamilyIdxs.isComplete())
                break;

//        Log::info("QueueFamily[{}]: {} :: Graphics: {}, Compute: {}, Transfer: {}",
//                  i, std::bitset<sizeof(VkQueueFlags)>(queueFamily.queueFlags),
//                  queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT,
//                  queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT,
//                  queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT);

            i++;
        }

        return queueFamilyIdxs;
    }












    static void CreateSwapchainAndImageViews()
    {
        SwapchainSupportDetails swapchainDetails = querySwapchainSupport(g_PhysDevice, g_SurfaceKHR);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainDetails.m_Formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapchainDetails.m_PresentModes);
        VkExtent2D extent = chooseSwapExtent(swapchainDetails.m_Capabilities);
        g_SwapchainExtent = extent;

        // tri buf
        uint32_t imageCount = swapchainDetails.m_Capabilities.minImageCount + 1;
        if (swapchainDetails.m_Capabilities.maxImageCount > 0 && imageCount > swapchainDetails.m_Capabilities.maxImageCount) {
            imageCount = swapchainDetails.m_Capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR swapchainInfo{};
        swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainInfo.surface = g_SurfaceKHR;
        swapchainInfo.minImageCount = imageCount;
        swapchainInfo.imageFormat = surfaceFormat.format;
        swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainInfo.imageExtent = extent;
        swapchainInfo.imageArrayLayers = 1;  // normally 1, except VR.
        swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        g_SwapchainImageFormat = surfaceFormat.format;

        // Image Share Mode. Queue Family.
        QueueFamilyIndices queueFamily = findQueueFamilies(g_PhysDevice);
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

        VkResult err = vkCreateSwapchainKHR(g_Device, &swapchainInfo, nullptr, &g_SwapchainKHR);
        if (err != VK_SUCCESS)
            throw std::runtime_error("failed to create vk swapchain khr.");

        // Get Swapchain Images.
        int expectedImageCount = imageCount;
        vkGetSwapchainImagesKHR(g_Device, g_SwapchainKHR, &imageCount, nullptr);
        assert(expectedImageCount == imageCount);

        g_SwapchainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(g_Device, g_SwapchainKHR, &imageCount, g_SwapchainImages.data());



        // Image Views
        g_SwapchainImageViews.resize(imageCount);
        for (int i = 0; i < imageCount; ++i)
        {
            g_SwapchainImageViews[i] = vkh::CreateImageView(g_SwapchainImages[i], surfaceFormat.format);
        }
    }

    static SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
    {
        SwapchainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.m_Capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
        if (formatCount) {
            details.m_Formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.m_Formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
        if (presentModeCount) {
            details.m_PresentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.m_PresentModes.data());
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
    static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
            return capabilities.currentExtent;

        int width, height;
        glfwGetFramebufferSize(g_WindowHandle, &width, &height);
        VkExtent2D extent = { (uint32_t)width, (uint32_t)height };
        extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        return extent;
    }



    static void DestroySwapchain()
    {
        vkDestroyImage(g_Device, g_DepthImage, nullptr);
        vkDestroyImageView(g_Device, g_DepthImageView, nullptr);
        vkFreeMemory(g_Device, g_DepthImageMemory, nullptr);

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
                                     g_DepthImage, g_DepthImageMemory, g_DepthImageView);
    }





    struct FramebufferAttachment
    {
        VkImage m_Image;
        VkDeviceMemory m_ImageMemory;
        VkImageView m_ImageView;
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
        FramebufferAttachment fbAttach{};

        vkh::CreateImage(w, h, fbAttach.m_Image,fbAttach.m_ImageMemory, format,
                    depth ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

        fbAttach.m_ImageView = vkh::CreateImageView(fbAttach.m_Image, format,
                                               depth ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT);

        return fbAttach;
    }



    static void InitDeferredRendering()
    {
        // ### init Gbuffer's Framebuffer & RenderPass
        int size = 1024;

        VkFormat rgbFormat = VK_FORMAT_R16G16B16_SFLOAT;
        g_Deferred_Gbuffer.gPosition = CreateFramebufferAttachment(size,size, rgbFormat);
        g_Deferred_Gbuffer.gNormal   = CreateFramebufferAttachment(size,size, rgbFormat);
        g_Deferred_Gbuffer.gAlbedo   = CreateFramebufferAttachment(size,size, rgbFormat);

        auto depFormat = vkh::findDepthFormat();
        g_Deferred_Gbuffer.gDepth = CreateFramebufferAttachment(size,size, depFormat, true);

        VkAttachmentDescription attachmentDesc[] = {
                vkh::c_AttachmentDescription(rgbFormat, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL),
                vkh::c_AttachmentDescription(rgbFormat, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL),
                vkh::c_AttachmentDescription(rgbFormat, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL),
                vkh::c_AttachmentDescription(depFormat, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        };


        VkAttachmentReference colorAttachmentRefs[] = {
                {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
                {1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
                {2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
        };
        VkAttachmentReference depthAttachmentRef =
                {3, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = std::size(colorAttachmentRefs);
        subpass.pColorAttachments = colorAttachmentRefs;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        // Use subpass dependencies for attachment layout transitions
        VkSubpassDependency dependencies[2];

        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;


        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = std::size(attachmentDesc);
        renderPassInfo.pAttachments = attachmentDesc;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = std::size(dependencies);
        renderPassInfo.pDependencies = dependencies;

        VK_CHECK(vkCreateRenderPass(g_Device, &renderPassInfo, nullptr, &g_Deferred_Gbuffer.m_RenderPass));


        VkImageView attachmentViews[] = {
                g_Deferred_Gbuffer.gPosition.m_ImageView,
                g_Deferred_Gbuffer.gNormal.m_ImageView,
                g_Deferred_Gbuffer.gAlbedo.m_ImageView,
                g_Deferred_Gbuffer.gDepth.m_ImageView
        };
        VkFramebufferCreateInfo framebufferInfo =
                vkh::c_Framebuffer(size,size, g_Deferred_Gbuffer.m_RenderPass,
                                   std::size(attachmentViews), attachmentViews);
        VK_CHECK(vkCreateFramebuffer(g_Device, &framebufferInfo, nullptr, &g_Deferred_Gbuffer.m_Framebuffer));





        // ### init init Compose's

        VkDescriptorSetLayout descriptorSetLayout = vkh::CreateDescriptorSetLayout({
                // binding 0: VSH uniform
                vkh::c_DescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
                // 1: FSH uniform
                vkh::c_DescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT),
                // 2: FSH gPosition
                vkh::c_DescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT),
                // 3: FSH gNormal
                vkh::c_DescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT),
                // 4: FSH gAlbedo
                vkh::c_DescriptorSetLayoutBinding(4, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT),
        });

        VkPipelineLayout pipelineLayout = vkh::CreatePipelineLayout(1, &descriptorSetLayout);


        // ### Pipelines for GBuffer & Compose

        // VkPipeline:: GBuffer

        VkPipelineVertexInputStateCreateInfo vertexInputState = vkh::c_PipelineVertexInputState();
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = vkh::c_PipelineInputAssemblyState(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        VkPipelineRasterizationStateCreateInfo rasterizator = vkh::c_PipelineRasterizationState();
        VkPipelineViewportStateCreateInfo viewportState = vkh::c_PipelineViewportState(1, 1);
        VkPipelineRasterizationStateCreateInfo rasterizer = vkh::c_PipelineRasterizationState();  // VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE
        VkPipelineMultisampleStateCreateInfo multisampling = vkh::c_PipelineMultisampleState();
        VkPipelineDepthStencilStateCreateInfo depthStencil = vkh::c_PipelineDepthStencilState();
        VkPipelineDynamicStateCreateInfo dynamicState = vkh::c_PipelineDynamicState();

        // for gPosition, gNormal, gAlbedo, if no, as color mask = 0x0, black will be render.
        VkPipelineColorBlendAttachmentState colorBlendAttachments[] = {
                vkh::c_PipelineColorBlendAttachmentState(),
                vkh::c_PipelineColorBlendAttachmentState(),
                vkh::c_PipelineColorBlendAttachmentState()
        };
        VkPipelineColorBlendStateCreateInfo colorBlending = vkh::c_PipelineColorBlendState(3, colorBlendAttachments);

        VkPipelineShaderStageCreateInfo vshGbuffer =
                vkh::LoadShaderStage(VK_SHADER_STAGE_VERTEX_BIT,Loader::fileResolve("shaders/spv/def_gbuffer/vert.spv"));
        VkPipelineShaderStageCreateInfo fshGbuffer =
                vkh::LoadShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT,Loader::fileResolve("shaders/spv/def_gbuffer/frag.spv"));
        VkPipelineShaderStageCreateInfo shaderStages[] = {vshGbuffer, fshGbuffer};

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
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        VK_CHECK(vkCreateGraphicsPipelines(g_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &g_Deferred_Gbuffer.m_Pipeline));



        //  VkPipeline:: COMPOSE


        VkPipelineShaderStageCreateInfo vshCompose = vkh::LoadShaderStage(VK_SHADER_STAGE_VERTEX_BIT,
                                                                          Loader::fileResolve("shaders/deferred_compose.vsh.spv"));
        VkPipelineShaderStageCreateInfo fshCompose = vkh::LoadShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                          Loader::fileResolve("shaders/deferred_compose.fsh.spv"));
        shaderStages[0] = vshCompose;
        shaderStages[1] = fshCompose;
        pipelineInfo.stageCount = std::size(shaderStages);
        pipelineInfo.pStages = shaderStages;

        VkPipelineVertexInputStateCreateInfo emptyVertexInputState = vkh::c_PipelineVertexInputState();

        pipelineInfo.pVertexInputState = &emptyVertexInputState;
        pipelineInfo.renderPass = g_RenderPass;



        VK_CHECK(vkCreateGraphicsPipelines(g_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &g_Deferred_Compose.m_Pipeline));




        vkDestroyShaderModule(g_Device, vshGbuffer.module, nullptr);
        vkDestroyShaderModule(g_Device, fshGbuffer.module, nullptr);

        vkDestroyShaderModule(g_Device, vshCompose.module, nullptr);
        vkDestroyShaderModule(g_Device, fshCompose.module, nullptr);

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

        VkAttachmentDescription colorAttachment =
                vkh::c_AttachmentDescription(g_SwapchainImageFormat, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        // .storeOp: VK_ATTACHMENT_STORE_OP_DONT_CARE
        VkAttachmentDescription depthAttachment =
                vkh::c_AttachmentDescription(vkh::findDepthFormat(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

        std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = attachments.size();
        renderPassInfo.pAttachments = attachments.data();
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

        std::vector<VkDynamicState> _dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicState = vkh::c_PipelineDynamicState(_dynamicStates.size(), _dynamicStates.data());

        VkPipelineColorBlendAttachmentState colorBlendAttachment = vkh::c_PipelineColorBlendAttachmentState();
        VkPipelineColorBlendStateCreateInfo colorBlending = vkh::c_PipelineColorBlendState(1, &colorBlendAttachment);

        g_PipelineLayout = vkh::CreatePipelineLayout(1, &g_DescriptorSetLayout);

//        VkPipelineShaderStageCreateInfo vertShaderStageInfo =
//                vkh::LoadShaderStage(VK_SHADER_STAGE_VERTEX_BIT, Loader::fileResolve("shaders/spv/def_gbuffer/vert.spv"));
//        VkPipelineShaderStageCreateInfo fragShaderStageInfo =
//                vkh::LoadShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, Loader::fileResolve());
        VkPipelineShaderStageCreateInfo shaderStages[2];
        vkh::LoadShaderStages_H(shaderStages, "shaders/spv/def_gbuffer/{}.spv");

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

//        vkDestroyShaderModule(g_Device, fragShaderStageInfo.module, nullptr);
//        vkDestroyShaderModule(g_Device, vertShaderStageInfo.module, nullptr);
        vkh::DestroyShaderModules(shaderStages);
    }


























    static void CreateFramebuffers()
    {
        g_SwapchainFramebuffers.resize(g_SwapchainImageViews.size());

        for (size_t i = 0; i < g_SwapchainImageViews.size(); i++)
        {
            std::array<VkImageView, 2> attachments = { g_SwapchainImageViews[i], g_DepthImageView };

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
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(g_PhysDevice);

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
            imageInfo.imageView = g_TextureImageView;
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
        beginInfo.pInheritanceInfo = nullptr; // Optional
        VK_CHECK(vkBeginCommandBuffer(cmdbuf, &beginInfo));

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = g_RenderPass;
        renderPassInfo.framebuffer = g_SwapchainFramebuffers[imageIdx];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = g_SwapchainExtent;

        std::array<VkClearValue, 2> clearValues = {};
        clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = clearValues.size();
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(cmdbuf, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, g_GraphicsPipeline);


        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = g_SwapchainExtent.width;
        viewport.height = g_SwapchainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(cmdbuf, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = g_SwapchainExtent;
        vkCmdSetScissor(cmdbuf, 0, 1, &scissor);


        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(cmdbuf, 0, 1, &g_VertexBuffer, offsets);

        vkCmdBindDescriptorSets(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, g_PipelineLayout, 0, 1,
                                &g_DescriptorSets[g_CurrentFrameInflight], 0, nullptr);  // i?
        vkCmdDraw(cmdbuf, g_DrawVerts, 1, 0, 0);

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
        VkResult rs = vkAcquireNextImageKHR(g_Device, g_SwapchainKHR, UINT64_MAX, g_SemaphoreImageAcquired[currframe],
                                            VK_NULL_HANDLE, &imageIdx);
        if (rs == VK_ERROR_OUT_OF_DATE_KHR) {
            RecreateSwapchain();
            return;
        } else if (!(rs == VK_SUCCESS || rs == VK_SUBOPTIMAL_KHR)) {
            throw std::runtime_error("failed to acquire swapchain image.");
        }

        vkResetFences(g_Device, 1, &g_InflightFence[currframe]);

        vkResetCommandBuffer(g_CommandBuffers[currframe], 0);
        RecordCommandBuffer(g_CommandBuffers[currframe], imageIdx);

        UpdateUniformBuffer(currframe);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &g_CommandBuffers[currframe];

        VkSemaphore waitSemaphores[] = { g_SemaphoreImageAcquired[currframe] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        VkSemaphore signalSemaphores[] = { g_SemaphoreRenderComplete[currframe] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(g_GraphicsQueue, 1, &submitInfo, g_InflightFence[currframe]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer.");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &g_SwapchainKHR;
        presentInfo.pImageIndices = &imageIdx;

        rs = vkQueuePresentKHR(g_PresentQueue, &presentInfo);
        if (rs == VK_ERROR_OUT_OF_DATE_KHR || rs == VK_SUBOPTIMAL_KHR || g_RecreateSwapchainRequested) {
            g_RecreateSwapchainRequested = false;
            RecreateSwapchain();
        } else if (rs != VK_SUCCESS) {
            throw std::runtime_error("failed to present swapchain image.");
        }

        // vkQueueWaitIdle(g_vkPresentQueue);  // BigWaste on GPU.

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
    }
    return vk;
}
