//
// Created by Dreamtowards on 2023/3/24.
//


vkx::Image::Image(VkImage image, VkDeviceMemory imageMemory, VkImageView imageView)
        : m_Image(image), m_ImageMemory(imageMemory), m_ImageView(imageView) {}
vkx::Image::~Image()
{
    vkDestroyImage(vkh::g_Device, m_Image, nullptr);
    vkFreeMemory(vkh::g_Device, m_ImageMemory, nullptr);
    vkDestroyImageView(vkh::g_Device, m_ImageView, nullptr);
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
    vkDestroyBuffer(vkh::g_Device, m_VertexBuffer, nullptr);
    vkFreeMemory(vkh::g_Device, m_VertexBufferMemory, nullptr);

    vkDestroyBuffer(vkh::g_Device, m_IndexBuffer, nullptr);
    vkFreeMemory(vkh::g_Device, m_IndexBufferMemory, nullptr);
}



// ============== CommandBuffer ==============

void vkx::CommandBuffer::BeginCommandBuffer(VkCommandBufferUsageFlags usage)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = usage;
    VK_CHECK(vkBeginCommandBuffer(m_CommandBuffer, &beginInfo));
}

void vkx::CommandBuffer::EndCommandBuffer()
{
    VK_CHECK(vkEndCommandBuffer(m_CommandBuffer));
}

// no: when pClearValues=nullptr, color/depthStencil will be {0,0,0,1}, {1, 0}
void vkx::CommandBuffer::CmdBeginRenderPass(VkRenderPass renderPass, VkFramebuffer framebuffer,
        VkExtent2D renderAreaExtent, int numClearValues, VkClearValue* pClearValues)
{
    vkh::CmdBeginRenderPass(m_CommandBuffer, renderPass, framebuffer, renderAreaExtent, numClearValues, pClearValues);
}
void vkx::CommandBuffer::CmdEndRenderPass()
{
    vkCmdEndRenderPass(m_CommandBuffer);
}

void vkx::CommandBuffer::CmdSetViewport(VkExtent2D wh, float x, float y, float minDepth, float maxDepth)
{
    vkh::CmdSetViewport(m_CommandBuffer, wh, x, y, minDepth, maxDepth);
}

void vkx::CommandBuffer::CmdSetScissor(VkExtent2D extent, VkOffset2D offset)
{
    vkh::CmdSetScissor(m_CommandBuffer, extent, offset);
}

void vkx::CommandBuffer::CmdBindVertexBuffer(VkBuffer vtxbuf)
{
    vkh::CmdBindVertexBuffer(m_CommandBuffer, vtxbuf);
}

void vkx::CommandBuffer::CmdBindIndexBuffer(VkBuffer idxbuf)
{
    vkh::CmdBindIndexBuffer(m_CommandBuffer, idxbuf);
}

void vkx::CommandBuffer::CmdBindGraphicsPipeline(VkPipeline graphics_pipeline)
{
    vkh::CmdBindGraphicsPipeline(m_CommandBuffer, graphics_pipeline);
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


void vkx::SubmitCommandBuffer(const std::function<void(VkCommandBuffer)>& fn_record,
                              VkDevice device, VkCommandPool commandPool, VkQueue queue)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer cmdbuf;
    vkAllocateCommandBuffers(device, &allocInfo, &cmdbuf);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmdbuf, &beginInfo);

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

void vkx::SubmitCommandBuffer(const std::function<void(VkCommandBuffer)>& fn_record)
{
    vkx::SubmitCommandBuffer(fn_record, vkh::g_Device, vkh::g_CommandPool, vkh::g_GraphicsQueue);
}








// ================== Descriptor ==================

void vkx::AllocateDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool, int descriptorSetCount, VkDescriptorSetLayout* descriptorSetLayouts, VkDescriptorSet* out_descriptorSets)
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = descriptorSetCount;
    allocInfo.pSetLayouts = descriptorSetLayouts;

    VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, out_descriptorSets));
}











void vkx::UniformBuffer::Create(VkDevice device, VkDeviceSize bufferSize)
{
    vkx::CreateBuffer(device,
                      bufferSize,
                      &m_Buffer,
                      &m_BufferMemory,
                      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    vkMapMemory(device, m_BufferMemory, 0, bufferSize, 0, &m_BufferMapped);
}

void vkx::UniformBuffer::Destroy(VkDevice device)
{
    vkDestroyBuffer(device, m_Buffer, nullptr);
    vkFreeMemory(device, m_BufferMemory, nullptr);
}

void vkx::UniformBuffer::MemCpy(void* src_ptr, size_t size)
{
    std::memcpy(m_BufferMapped, src_ptr, size);
}







// ================ low level ================

static uint32_t _findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vkh::g_PhysDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("failed to find suitable memory type!");
}
//VkMemoryRequirements memRequirements / VkDeviceSize size, uint32_t memoryTypeBits,
VkDeviceMemory vkx::AllocateMemory(VkDevice device,
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


void vkx::CreateBuffer(VkDevice device, VkDeviceSize size, VkBuffer* pBuffer, VkDeviceMemory* pBufferMemory, VkBufferUsageFlags usage,
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

    *pBufferMemory = vkx::AllocateMemory(device, memRequirements, memProperties);

    VK_CHECK(vkBindBufferMemory(device, *pBuffer, *pBufferMemory, 0));
}







void vkx::CreateImage(VkDevice device, int imgWidth, int imgHeight, VkImage* pImage, VkDeviceMemory* pImageMemory, VkFormat format,
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

    *pImageMemory = vkx::AllocateMemory(device, memRequirements, memProperties);

    VK_CHECK(vkBindImageMemory(device, *pImage, *pImageMemory, 0));
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

void vkx::CreateStagedImage(VkDevice device, int imgWidth, int imgHeight, void* pixels,
                            VkImage* out_image,
                            VkDeviceMemory* out_imageMemory,
                            VkImageView* out_imageView)
{

    VkDeviceSize imageSize = imgWidth * imgHeight * 4;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    vkx::CreateBuffer(device, imageSize, &stagingBuffer, &stagingBufferMemory,
                      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, imageSize);
    vkUnmapMemory(device, stagingBufferMemory);


    vkx::CreateImage(device, imgWidth, imgHeight,
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

    *out_imageView = vkh::CreateImageView(*out_image, VK_FORMAT_R8G8B8A8_SRGB);
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

VkInstance vkx::CreateInstance(bool enableValidationLayer)
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
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo;
    if (enableValidationLayer) {
        CheckValidationLayersSupport(g_ValidationLayers);

        instInfo.enabledLayerCount = g_ValidationLayers.size();
        instInfo.ppEnabledLayerNames = g_ValidationLayers.data();

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




vkx::Instance::~Instance()
{
    vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
    vkDestroyDevice(m_Device, nullptr);
    vkDestroySurfaceKHR(m_Instance, m_SurfaceKHR, nullptr);

    if (m_EnabledValidationLayer) {
        extDestroyDebugMessenger(m_Instance, g_DebugMessengerEXT, nullptr);
    }
    vkDestroyInstance(m_Instance, nullptr);
}


vkx::Instance* vkx::Init(GLFWwindow* glfwWindow, bool enableValidationLayer)
{
    vkx::Instance* inst = new vkx::Instance();
    inst->m_EnabledValidationLayer=enableValidationLayer;

    VkInstance vkInst = vkx::CreateInstance(enableValidationLayer);
    inst->m_Instance = vkInst;
    inst->m_SurfaceKHR = CreateSurface(inst->m_Instance, glfwWindow);

    inst->m_PhysDevice = PickPhysicalDevice(vkInst);
    QueueFamilyIndices queueFamily = vkh::findQueueFamilies(inst->m_PhysDevice, inst->m_SurfaceKHR);
    inst->m_Device = CreateLogicalDevice(inst->m_PhysDevice, queueFamily,
                                         &inst->m_GraphicsQueue, &inst->m_PresentQueue);

    inst->m_CommandPool = CreateCommandPool(inst->m_Device, queueFamily.m_GraphicsFamily);




    return inst;
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






QueueFamilyIndices vkh::findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surfaceKHR)
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


void vkh::CreateSwapchain(VkPhysicalDevice physDevice, VkSurfaceKHR surfaceKHR, GLFWwindow *glfwWindow,
                          VkSwapchainKHR &out_SwapchainKHR, VkExtent2D &out_SwapchainExtent,
                          VkFormat &out_SwapchainImageFormat, std::vector<VkImage> &out_SwapchainImages,
                          std::vector<VkImageView> &out_SwapchainImageViews)
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
    QueueFamilyIndices queueFamily = findQueueFamilies(g_PhysDevice, surfaceKHR);
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

    VK_CHECK_MSG(vkCreateSwapchainKHR(g_Device, &swapchainInfo, nullptr, &out_SwapchainKHR),
                 "failed to create vk swapchain khr.");

    // Get Swapchain Images.
    int expectedImageCount = imageCount;
    vkGetSwapchainImagesKHR(g_Device, out_SwapchainKHR, &imageCount, nullptr);
    assert(expectedImageCount == imageCount);

    out_SwapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(g_Device, out_SwapchainKHR, &imageCount, out_SwapchainImages.data());

    // Image Views
    out_SwapchainImageViews.resize(imageCount);
    for (int i = 0; i < imageCount; ++i)
    {
        out_SwapchainImageViews[i] = CreateImageView(out_SwapchainImages[i], surfaceFormat.format);
    }
}



    class GPU
    {
    public:
        bool isDiscrete;

        VkPhysicalDeviceProperties m_Properties;
        VkPhysicalDeviceFeatures m_Features;
    };


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



















VkShaderModule vkh::LoadShaderModule(const void *data, size_t size)
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

VkPipelineShaderStageCreateInfo vkh::LoadShaderStage(VkShaderStageFlagBits stage, const std::string& spv_filename)
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

void vkh::LoadShaderStages_H(VkPipelineShaderStageCreateInfo* dst, const std::string& spv_filename_pat)
{
    dst[0] = LoadShaderStage(VK_SHADER_STAGE_VERTEX_BIT,
                             Loader::fileResolve(Strings::fmt(spv_filename_pat, "vert")));
    dst[1] = LoadShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT,
                             Loader::fileResolve(Strings::fmt(spv_filename_pat, "frag")));
}

void vkh::DestroyShaderModules(VkPipelineShaderStageCreateInfo *dst)
{
    vkDestroyShaderModule(g_Device, dst[0].module, nullptr);
    vkDestroyShaderModule(g_Device, dst[1].module, nullptr);
}














VkPipelineVertexInputStateCreateInfo vkh::c_PipelineVertexInputState(int numVertBinding,
                                                                     VkVertexInputBindingDescription *pVertBinding,
                                                                     int numVertAttr,
                                                                     VkVertexInputAttributeDescription *pVertAttr)
{
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = numVertBinding;
    vertexInputInfo.pVertexBindingDescriptions = pVertBinding;
    vertexInputInfo.vertexAttributeDescriptionCount = numVertAttr;
    vertexInputInfo.pVertexAttributeDescriptions = pVertAttr;
    return vertexInputInfo;
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

VkPipelineVertexInputStateCreateInfo vkh::c_PipelineVertexInputState_H(std::initializer_list<VkFormat> attribs, int binding)
{
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
        offset += FormatSize(attr_format);
    }
    static VkVertexInputBindingDescription _bindingDesc{};
    _bindingDesc.binding = binding;
    _bindingDesc.stride = offset;
    _bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return c_PipelineVertexInputState(1, &_bindingDesc, _attrDesc.size(), _attrDesc.data());
}

VkPipelineInputAssemblyStateCreateInfo vkh::c_PipelineInputAssemblyState(VkPrimitiveTopology topology)
{
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = topology;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    return inputAssembly;
}

VkPipelineRasterizationStateCreateInfo vkh::c_PipelineRasterizationState(VkPolygonMode polygonMode,
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

VkPipelineViewportStateCreateInfo vkh::c_PipelineViewportState(int viewportCount, int scissorCount)
{
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = viewportCount;
    viewportState.scissorCount = scissorCount;
    return viewportState;
}

VkPipelineDepthStencilStateCreateInfo vkh::c_PipelineDepthStencilState()
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

VkPipelineMultisampleStateCreateInfo vkh::c_PipelineMultisampleState()
{
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    return multisampling;
}

VkPipelineDynamicStateCreateInfo vkh::c_PipelineDynamicState(int numStates, VkDynamicState *pStates)
{
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = numStates;
    dynamicState.pDynamicStates = pStates;
    return dynamicState;
}

VkPipelineDynamicStateCreateInfo vkh::c_PipelineDynamicState_H_ViewportScissor() {
    static std::vector<VkDynamicState> _dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    return c_PipelineDynamicState(_dynamicStates.size(), _dynamicStates.data());
}


VkPipelineColorBlendAttachmentState vkh::c_PipelineColorBlendAttachmentState()
{
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    return colorBlendAttachment;
}

VkPipelineColorBlendStateCreateInfo vkh::c_PipelineColorBlendState(int numAttach,
                                                                   VkPipelineColorBlendAttachmentState *pAttach)
{
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = numAttach;
    colorBlending.pAttachments = pAttach;
    return colorBlending;
}

VkAttachmentDescription vkh::c_AttachmentDescription(VkFormat format, VkImageLayout finalLayout)
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

VkFramebufferCreateInfo vkh::c_Framebuffer(int w, int h, VkRenderPass renderPass, int attchCount,
                                           VkImageView *pAttach)
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
VkDescriptorSetLayoutBinding vkh::c_DescriptorSetLayoutBinding(int bind, VkDescriptorType descType,
                                                               VkShaderStageFlags shaderStageFlags)
{
    VkDescriptorSetLayoutBinding binding{};
    binding.binding = bind;
    binding.descriptorType = descType;
    binding.descriptorCount = 1;
    binding.stageFlags = shaderStageFlags;
    return binding;
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


VkDescriptorSetLayout vkh::CreateDescriptorSetLayout(std::initializer_list<VkDescriptorSetLayoutBinding> bindings)
{
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = bindings.size();
    layoutInfo.pBindings = bindings.begin();

    VkDescriptorSetLayout descriptorSetLayout;
    VK_CHECK_MSG(vkCreateDescriptorSetLayout(g_Device, &layoutInfo, nullptr, &descriptorSetLayout), "failed to create descriptor set layout.");
    return descriptorSetLayout;
}

VkPipelineLayout vkh::CreatePipelineLayout(int numSetLayouts, VkDescriptorSetLayout* pSetLayouts)
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    pipelineLayoutInfo.setLayoutCount = numSetLayouts;
    pipelineLayoutInfo.pSetLayouts = pSetLayouts;

    VkPipelineLayout pipelineLayout;
    VK_CHECK_MSG(vkCreatePipelineLayout(g_Device, &pipelineLayoutInfo, nullptr, &pipelineLayout), "failed to create pipeline layout!");
    return pipelineLayout;
}









VkImageView vkh::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
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
    VK_CHECK_MSG(vkCreateImageView(g_Device, &viewInfo, nullptr, &imageView), "failed to create texture image view.");

    return imageView;
}










VkSampler vkh::CreateTextureSampler()
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







void vkh::CreateVertexBuffer(const void *in_data, size_t size, VkBuffer &out_buffer, VkDeviceMemory &out_bufferMemory,
                             bool bufferUsageIndexBuffer)
{
    VkDeviceSize bufferSize = size;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    vkx::CreateBuffer(g_Device, bufferSize, &stagingBuffer, &stagingBufferMemory,
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* mapped_dst;
    vkMapMemory(g_Device, stagingBufferMemory, 0, bufferSize, 0, &mapped_dst);
    memcpy(mapped_dst, in_data, bufferSize);
    vkUnmapMemory(g_Device, stagingBufferMemory);

    vkx::CreateBuffer(g_Device, bufferSize, &out_buffer, &out_bufferMemory,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | (bufferUsageIndexBuffer ? VK_BUFFER_USAGE_INDEX_BUFFER_BIT : VK_BUFFER_USAGE_VERTEX_BUFFER_BIT),
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    CopyBuffer(stagingBuffer, out_buffer, bufferSize);

    vkDestroyBuffer(g_Device, stagingBuffer, nullptr);
    vkFreeMemory(g_Device, stagingBufferMemory, nullptr);
}







static VkFormat _findSupportedFormat(const std::vector<VkFormat>& candidates,
                                     VkImageTiling tiling,
                                     VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(vkh::g_PhysDevice, format, &props);
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    throw std::runtime_error("failed to find supported format.");
}

VkFormat vkh::findDepthFormat() {
    return _findSupportedFormat(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}
void vkh::CreateDepthTextureImage(int w, int h, VkImage& depthImage, VkDeviceMemory& depthImageMemory, VkImageView& depthImageView)
{
    VkFormat depthFormat = findDepthFormat();

    vkx::CreateImage(g_Device, w,h, &depthImage, &depthImageMemory,
                     depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

    depthImageView = CreateImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    TransitionImageLayout(depthImage, depthFormat,
                               VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}


void vkh::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
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














void vkh::QueueSubmit(VkQueue queue, VkCommandBuffer cmdbuf, VkSemaphore wait, VkSemaphore signal, VkFence fence)
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

// no: when pClearValues=nullptr, color/depthStencil will be {0,0,0,1}, {1, 0}
void vkh::CmdBeginRenderPass(VkCommandBuffer cmdbuf,
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

void vkh::CmdSetViewport(VkCommandBuffer cmdbuf, VkExtent2D wh, float x, float y, float minDepth, float maxDepth)
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

void vkh::CmdSetScissor(VkCommandBuffer cmdbuf, VkExtent2D extent, VkOffset2D offset)
{
    VkRect2D scissor{};
    scissor.offset = offset;
    scissor.extent = extent;
    vkCmdSetScissor(cmdbuf, 0, 1, &scissor);
}

void vkh::CmdBindVertexBuffer(VkCommandBuffer cmdbuf, const VkBuffer vbuf)
{
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(cmdbuf, 0, 1, &vbuf, offsets);
}

void vkh::CmdBindIndexBuffer(VkCommandBuffer cmdbuf, const VkBuffer idx_buf)
{
    vkCmdBindIndexBuffer(cmdbuf, idx_buf, 0, VK_INDEX_TYPE_UINT32);
}

void vkh::CmdBindGraphicsPipeline(VkCommandBuffer cmdbuf, VkPipeline graphicsPipeline)
{
    vkCmdBindPipeline(cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
}
