
#include "vkx.hpp"

#include <iostream>
#include <sstream>
#include <set>
#include <cmath>


#pragma region base


const vk::Result& vkx::check(const vk::Result& r)
{
    if (r != vk::Result::eSuccess)
    {
        assert(false && "vkx::check() fail.");
    }
    return r;
}
const VkResult& vkx::check(const VkResult& r)
{
    vkx::check((vk::Result)r);
    return r;
}

uint32_t vkx::FormatSize(vk::Format fmt)
{
    switch (fmt) {
    case vk::Format::eR16Sfloat:         
        return 2;
    case vk::Format::eR32Sfloat:          
        return 4;
    case vk::Format::eR16G16B16Sfloat:  
        return 6;
    case vk::Format::eR16G16B16A16Sfloat:
    case vk::Format::eR32G32Sfloat:
        return 8;
    case vk::Format::eR32G32B32Sfloat:    
        return 12;
    case vk::Format::eR32G32B32A32Sfloat:
        return 16;
    default: throw std::runtime_error("vkx::FormatSize Error: Unsupported format");
    }
}

#pragma endregion











#pragma region AllocateMemory, CreateBuffer, CopyBuffer

static uint32_t _FindMemoryType(uint32_t memTypeFilter, vk::MemoryPropertyFlags memPropertiesFlags)
{
    vk::PhysicalDeviceMemoryProperties memProperties = vkx::ctx().PhysDeviceMemoryProperties;

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((memTypeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & memPropertiesFlags) == memPropertiesFlags) {
            return i;
        }
    }
    throw std::runtime_error("failed to find suitable memory type!");
}

vk::DeviceMemory vkx::AllocateMemory(
    vk::MemoryRequirements memRequirements,  // size, alignment, memoryType
    vk::MemoryPropertyFlags memProperties)   // DEVICE_LOCAL, HOST_VISIABLE, etc..
{
    VKX_CTX_device_allocator;

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = _FindMemoryType(memRequirements.memoryTypeBits, memProperties);

    return device.allocateMemory(allocInfo, allocator);
}


vk::Buffer vkx::CreateBuffer(
    VkDeviceSize size,
    vk::DeviceMemory& out_BufferMemory,
    vk::BufferUsageFlags usage,
    vk::MemoryPropertyFlags memProperties)
{
    VKX_CTX_device_allocator;

    vk::BufferCreateInfo bufferInfo{};
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    vk::Buffer buffer = device.createBuffer(bufferInfo, allocator);

    vk::MemoryRequirements memRequirements = device.getBufferMemoryRequirements(buffer);
    out_BufferMemory = vkx::AllocateMemory(memRequirements, memProperties);

    device.bindBufferMemory(buffer, out_BufferMemory, 0);

    return buffer;
}


void vkx::CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
    vkx::SubmitCommandBuffer([&](vk::CommandBuffer cmdbuf)
    {
        vk::BufferCopy copyRegion{};
        copyRegion.size = size;
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        cmdbuf.copyBuffer(srcBuffer, dstBuffer, copyRegion);
    });
}


void* vkx::MapMemory(vk::DeviceMemory deviceMemory, vk::DeviceSize size, vk::DeviceSize offset)
{
    VKX_CTX_device;
    return device.mapMemory(deviceMemory, offset, size, (vk::MemoryMapFlagBits)0);
}

void vkx::UnmapMemory(vk::DeviceMemory deviceMemory)
{
    VKX_CTX_device;
    device.unmapMemory(deviceMemory);
}


vk::Buffer vkx::CreateStagedBuffer(
    const void* bufferData,
    vk::DeviceSize bufferSize,
    vk::DeviceMemory& out_BufferMemory,
    vk::BufferUsageFlags usage)
{
    VKX_CTX_device_allocator;

    vk::DeviceMemory stagingBufferMemory;
    vk::Buffer stagingBuffer = vkx::CreateBuffer(bufferSize, stagingBufferMemory,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    void* mapped_dst = vkx::MapMemory(stagingBufferMemory, bufferSize);
    std::memcpy(mapped_dst, bufferData, bufferSize);
    vkx::UnmapMemory(stagingBufferMemory);

    vk::Buffer buffer = vkx::CreateBuffer(bufferSize, out_BufferMemory,
        vk::BufferUsageFlagBits::eTransferDst | usage,
        vk::MemoryPropertyFlagBits::eDeviceLocal);

    vkx::CopyBuffer(stagingBuffer, buffer, bufferSize);

    device.destroyBuffer(stagingBuffer, allocator);
    device.freeMemory(stagingBufferMemory, allocator);

    return buffer;
}


vkx::VertexBuffer::VertexBuffer(vk::Buffer vb, vk::DeviceMemory vbm, vk::Buffer ib, vk::DeviceMemory ibm, uint32_t vcount)
    : vertexBuffer(vb), vertexBufferMemory(vbm), indexBuffer(ib), indexBufferMemory(ibm), vertexCount(vcount) {}

vkx::VertexBuffer::~VertexBuffer()
{
    VKX_CTX_device_allocator;

    device.destroyBuffer(vertexBuffer, allocator);
    device.freeMemory(vertexBufferMemory, allocator);
    device.destroyBuffer(indexBuffer, allocator);
    device.freeMemory(indexBufferMemory, allocator);
}

//vkx::VertexBuffer* vkx::LoadVertexBuffer(
//    uint32_t vertexCount,
//    vkx_slice_t<const char> vertices,
//    vkx_slice_t<const char> indices)
//{
//    vk::DeviceMemory vtxmem;
//    vk::Buffer vtxbuf = vkx::CreateStagedBuffer(vertices.data(), vertices.size(), vtxmem, vk::BufferUsageFlagBits::eVertexBuffer);
//
//    vk::DeviceMemory idxmem{};
//    vk::Buffer idxbuf{};
//    if (indices.size() > 0)
//    {
//        idxbuf = vkx::CreateStagedBuffer(indices.data(), indices.size(), idxmem, vk::BufferUsageFlagBits::eIndexBuffer);
//    }
//    return new vkx::VertexBuffer(vtxbuf, vtxmem, idxbuf, idxmem, vertexCount);
//}


#pragma endregion


#pragma region Image, ImageView. ImageSampler.

vk::Image vkx::CreateImage(
    int width,
    int height, 
    vk::DeviceMemory& out_ImageMemory,
    vk::Format format,
    vk::ImageUsageFlags usage,
    vk::MemoryPropertyFlags memProperties,
    vk::ImageTiling tiling,
    bool isCubeMap)
{
    VKX_CTX_device_allocator;

    vk::ImageCreateInfo imageInfo{};
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.format = format;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = isCubeMap ? 6 : 1;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage = usage | vk::ImageUsageFlagBits::eSampled;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;
    imageInfo.samples = vk::SampleCountFlagBits::e1;
    imageInfo.tiling = tiling;
    //imageInfo.flags = 0; // Optional

    if (isCubeMap) {
        imageInfo.flags |= vk::ImageCreateFlagBits::eCubeCompatible;
    }

    vk::Image image = device.createImage(imageInfo, allocator);

    vk::MemoryRequirements memRequirements = device.getImageMemoryRequirements(image);
    out_ImageMemory = vkx::AllocateMemory(memRequirements, memProperties);

    device.bindImageMemory(image, out_ImageMemory, 0);

    return image;
}

vk::ImageView vkx::CreateImageView(
    vk::Image image,
    vk::Format format,
    vk::ImageAspectFlags aspectFlags,
    vk::ImageViewType imageViewType)
{
    VKX_CTX_device_allocator;

    vk::ImageViewCreateInfo viewInfo{};
    viewInfo.viewType = imageViewType;
    viewInfo.format = format;
    viewInfo.image = image;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = imageViewType == vk::ImageViewType::eCube ? 6 : 1;  // except CubeMap=6 or VR.
    //viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    //viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    //viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    //viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    return device.createImageView(viewInfo, allocator);
}

vk::Sampler vkx::CreateImageSampler(
    vk::Filter magFilter,
    vk::Filter minFilter,
    vk::SamplerAddressMode addressModeUVW)
{
    VKX_CTX_device_allocator;
    vk::PhysicalDevice physDevice = vkx::ctx().PhysDevice;
    vk::PhysicalDeviceProperties gpuProperties = vkx::ctx().PhysDeviceProperties;

    vk::SamplerCreateInfo samplerInfo{};
    samplerInfo.magFilter = magFilter;
    samplerInfo.minFilter = minFilter;
    samplerInfo.addressModeU = addressModeUVW;
    samplerInfo.addressModeV = addressModeUVW;
    samplerInfo.addressModeW = addressModeUVW;

    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = std::min(4.0f, gpuProperties.limits.maxSamplerAnisotropy);

    samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp  = vk::CompareOp::eAlways;
    samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    return device.createSampler(samplerInfo, allocator);
}



static bool _IsFormatHasStencilComponent(vk::Format format) {
    return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}
static void _TransitionImageLayout(
    vk::Image image,
    vk::Format format,
    vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
    uint32_t layerCount = 1)
{
    vk::ImageMemoryBarrier barrier{};
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = layerCount;
    if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
        if (_IsFormatHasStencilComponent(format)) {
            barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
        }
    } else {
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    }

    vk::PipelineStageFlags srcStage;
    vk::PipelineStageFlags dstStage;

    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits::eNone;
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
        dstStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        srcStage = vk::PipelineStageFlagBits::eTransfer;
        dstStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits::eNone;
        barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

        srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
        dstStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkx::SubmitCommandBuffer([&](vk::CommandBuffer cmd)
    {
        cmd.pipelineBarrier(
            srcStage, dstStage,
            (vk::DependencyFlags)0,
            {},
            {},
            barrier);
    });
}



static vk::Format _FindSupportedFormat(
    const std::vector<vk::Format>& candidates,
    vk::ImageTiling tiling,
    vk::FormatFeatureFlags features)
{
    for (vk::Format format : candidates) {
        vk::FormatProperties props = vkx::ctx().PhysDevice.getFormatProperties(format);
        if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
            return format;
        }
        else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    throw std::runtime_error("failed to find supported format.");
}
static vk::Format _FindDepthFormat() {
    return _FindSupportedFormat(
        { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}


vkx::Image::Image(vk::Image image, vk::DeviceMemory imageMemory, vk::Format format, int width, int height, vk::ImageView imageView) :
    image(image), imageMemory(imageMemory), format(format), width(width), height(height), imageView(imageView) {}

vkx::Image::~Image()
{
    VKX_CTX_device_allocator;
    device.destroyImage(image, allocator);
    device.freeMemory(imageMemory, allocator);
    device.destroyImageView(imageView, allocator);
}


vkx::Image* vkx::CreateColorImage(
    vk::Extent2D wh,
    vk::Format format,
    vk::ImageUsageFlags usage,
    vk::ImageAspectFlags aspectFlags)
{
    vk::DeviceMemory imageMemory;
    vk::Image image = vkx::CreateImage(wh.width, wh.height, imageMemory, format, usage);

    vk::ImageView imageView = vkx::CreateImageView(image, format, aspectFlags);

    return new vkx::Image(image, imageMemory, format, wh.width, wh.height, imageView);
}

vkx::Image* vkx::CreateDepthImage(vk::Extent2D wh, vk::Format depthFormat)
{
    if (depthFormat == vk::Format::eUndefined)
        depthFormat = _FindDepthFormat();

    vkx::Image* img = vkx::CreateColorImage(wh, depthFormat, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth);

    _TransitionImageLayout(img->image, depthFormat,
        vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);

    return img;
}


static void _CopyBufferToImage(
    vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height,
    int regionCount = 1, int regionOffsetSize = 0)  // for CubeMaps
{
    vkx::SubmitCommandBuffer([&](vk::CommandBuffer cmd)
    {
        std::vector<vk::BufferImageCopy> regions(regionCount);
        uint32_t offset = 0;
        for (int i = 0; i < regionCount; ++i) {
            vk::BufferImageCopy& region = regions[i] = {};
            region.bufferOffset = offset;
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;
            region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = i;  // face_i
            region.imageSubresource.layerCount = 1;
            region.imageOffset = { 0, 0, 0 };
            region.imageExtent = { width, height, 1 };
            offset += regionOffsetSize;
        }

        cmd.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, regions);
    });
}

vkx::Image* vkx::CreateStagedImage(
    uint32_t width, uint32_t height, void* pixels)
{
    VKX_CTX_device_allocator;
    vk::DeviceSize imageSize = width * height * 4;
    vk::Format format = vk::Format::eR8G8B8A8Unorm;

    vk::DeviceMemory stagingBufferMemory;
    vk::Buffer stagingBuffer = vkx::CreateBuffer(imageSize, stagingBufferMemory,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    void* mapped = vkx::MapMemory(stagingBufferMemory, imageSize);
    std::memcpy(mapped, pixels, imageSize);
    vkx::UnmapMemory(stagingBufferMemory);

    vk::DeviceMemory imageMemory;
    vk::Image image = vkx::CreateImage(width, height, imageMemory, format, 
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled);

    _TransitionImageLayout(image, format,
        vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

    _CopyBufferToImage(stagingBuffer, image, width, height);

    _TransitionImageLayout(image, format, 
        vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

    device.destroyBuffer(stagingBuffer, allocator);
    device.freeMemory(stagingBufferMemory, allocator);

    vk::ImageView imageView = vkx::CreateImageView(image, format);

    return new vkx::Image(image, imageMemory, format, width, height, imageView);
}


#pragma endregion


#pragma region CommandBuffer, QueueSubmit

void vkx::AllocateCommandBuffers(
    int numCmdbuf,
    vk::CommandBuffer* pCmdbufs,
    vk::CommandBufferLevel level,
    vk::CommandPool commandPool,
    vk::Device device)
{
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.commandPool = commandPool;
    allocInfo.level = level;
    allocInfo.commandBufferCount = numCmdbuf;

    VKX_CHECK(device.allocateCommandBuffers(&allocInfo, pCmdbufs));
}

void vkx::SubmitCommandBuffer(
    const std::function<void(vk::CommandBuffer)>& fn_record,
    vk::Queue queue,
    vk::CommandPool commandPool)
{
    VKX_CTX_device;

    vk::CommandBuffer cmdbuf;
    vkx::AllocateCommandBuffers(1, &cmdbuf, vk::CommandBufferLevel::ePrimary);

    cmdbuf.begin(vk::CommandBufferBeginInfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

    fn_record(cmdbuf);

    cmdbuf.end();

    vk::SubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdbuf;
    queue.submit(submitInfo);
    queue.waitIdle();

    device.freeCommandBuffers(commandPool, cmdbuf);
}


void vkx::QueueSubmit(
    vk::Queue queue,
    vkx_slice_t<vk::CommandBuffer> cmdbufs,
    vkx_slice_t<vk::Semaphore> waits,
    vkx_slice_t<vk::PipelineStageFlags> waitStages,  // vk::PipelineStageFlagBits::eColorAttachmentOutput
    vkx_slice_t<vk::Semaphore> signals,
    vk::Fence fence)
{
    vk::SubmitInfo submitInfo{};
    submitInfo.commandBufferCount = cmdbufs.size();
    submitInfo.pCommandBuffers = cmdbufs.data();

    assert(waits.size() == waitStages.size());
    submitInfo.waitSemaphoreCount = waits.size();
    submitInfo.pWaitSemaphores = waits.data();
    submitInfo.pWaitDstStageMask = waitStages.data();

    submitInfo.signalSemaphoreCount = signals.size();
    submitInfo.pSignalSemaphores = signals.data();

    queue.submit(submitInfo, fence);
}


vk::Result vkx::QueuePresentKHR(
    vk::Queue presentQueue,
    vkx_slice_t<vk::Semaphore> waitSemaphores,
    vkx_slice_t<vk::SwapchainKHR> swapchains,
    vkx_slice_t<uint32_t> imageIndices)
{
    vk::PresentInfoKHR presentInfo{};
    presentInfo.waitSemaphoreCount = waitSemaphores.size();
    presentInfo.pWaitSemaphores = waitSemaphores.data();
    presentInfo.swapchainCount = swapchains.size();
    presentInfo.pSwapchains = swapchains.data();
    presentInfo.pImageIndices = imageIndices.data();

    return presentQueue.presentKHR(&presentInfo);
}


vkx::CommandBuffer::CommandBuffer(vk::CommandBuffer cmdbuf)
    : cmd(cmdbuf) {}

vkx::CommandBuffer::operator VkCommandBuffer() {
    return cmd;
}
vkx::CommandBuffer::operator vk::CommandBuffer() {
    return cmd;
}

void vkx::CommandBuffer::Reset()
{
    cmd.reset();
}

void vkx::CommandBuffer::Begin(vk::CommandBufferUsageFlags usageFlags)
{
    vk::CommandBufferBeginInfo beginInfo{ .flags = usageFlags };
    cmd.begin(beginInfo);
}

void vkx::CommandBuffer::End()
{
    cmd.end();
}


void vkx::CommandBuffer::BeginRenderPass(
    vk::RenderPass renderPass,
    vk::Framebuffer framebuffer,
    vk::Extent2D renderArea,
    vkx_slice_t<vk::ClearValue> clearValues,
    vk::SubpassContents subpassContents)
{
    vk::RenderPassBeginInfo beginInfo{};
    beginInfo.renderPass = renderPass;
    beginInfo.framebuffer = framebuffer;
    beginInfo.renderArea.offset = { 0, 0 };
    beginInfo.renderArea.extent = renderArea;
    beginInfo.clearValueCount = clearValues.size();
    beginInfo.pClearValues = clearValues.data();

    cmd.beginRenderPass(beginInfo, subpassContents);
}

void vkx::CommandBuffer::EndRenderPass()
{
    cmd.endRenderPass();
}

void vkx::CommandBuffer::BindDescriptorSets(vk::PipelineLayout pipelineLayout, vkx_slice_t<vk::DescriptorSet> descriptorSets, uint32_t firstSet)
{
    cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, firstSet, descriptorSets, {});
}

void vkx::CommandBuffer::BindGraphicsPipeline(vk::Pipeline graphicsPipeline)
{
    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);
}

void vkx::CommandBuffer::SetViewport(
    vk::Offset2D offset, vk::Extent2D extent,
    float minDepth, float maxDepth)
{
    vk::Viewport viewport{
       .x = (float)offset.x,
       .y = (float)offset.y,
       .width = (float)extent.width,
       .height = (float)extent.height,
       .minDepth = minDepth,
       .maxDepth = maxDepth
    };
#ifdef VKX_VIEWPORT_NEG_HEIGHT
    viewport.y = (float)offset.y + (float)extent.height;
    viewport.height = -(float)extent.height,
#endif // VKX_VIEWPORT_NEG_HEIGHT
        cmd.setViewport(0, viewport);
}

// negative viewport heights?
void vkx::CommandBuffer::SetScissor(
    vk::Offset2D offset, vk::Extent2D extent)
{
    cmd.setScissor(0, vk::Rect2D{ .offset = offset, .extent = extent });
}

void vkx::CommandBuffer::BindVertexBuffers(
    vkx_slice_t<vk::Buffer> buffers,
    vkx_slice_t<vk::DeviceSize> offsets,
    uint32_t firstBinding)
{
    static vk::DeviceSize _zeros[8] = {};

    cmd.bindVertexBuffers(firstBinding, buffers, offsets.size() == 0 ? vkx_slice_t{buffers.size(), _zeros} : offsets);
}

void vkx::CommandBuffer::BindIndexBuffer(vk::Buffer buffer, vk::DeviceSize offset, vk::IndexType indexType)
{
    cmd.bindIndexBuffer(buffer, offset, indexType);
}

void vkx::CommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    cmd.draw(vertexCount, instanceCount, firstVertex, firstInstance);
}

void vkx::CommandBuffer::DrawIndexed(uint32_t vertexCount)
{
    cmd.drawIndexed(vertexCount, 1, 0, 0, 0);
}

#pragma endregion


#pragma region RenderPass, Framebuffer

vk::RenderPass vkx::CreateRenderPass(
    vkx_slice_t<const vk::AttachmentDescription> attachments,
    vkx_slice_t<const vk::SubpassDescription> subpasses,
    vkx_slice_t<const vk::SubpassDependency> dependencies)
{
    VKX_CTX_device_allocator;

    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.attachmentCount = attachments.size();
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = subpasses.size();
    renderPassInfo.pSubpasses = subpasses.data();
    renderPassInfo.dependencyCount = dependencies.size();
    renderPassInfo.pDependencies = dependencies.data();

    return device.createRenderPass(renderPassInfo, allocator);
}


vk::AttachmentDescription vkx::IAttachmentDesc(
    vk::Format format,
    vk::ImageLayout finalLayout)
{
    vk::AttachmentDescription d{};
    d.format = format;
    d.samples = vk::SampleCountFlagBits::e1;
    d.loadOp  = vk::AttachmentLoadOp::eClear;
    d.storeOp = vk::AttachmentStoreOp::eStore;
    d.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    d.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    d.initialLayout  = vk::ImageLayout::eUndefined;
    d.finalLayout = finalLayout;  // VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL / VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    return d;
}

vk::AttachmentReference vkx::IAttachmentRef(
    uint32_t attachment,
    vk::ImageLayout layout)
{
    vk::AttachmentReference a{};
    a.attachment = attachment;
    a.layout = layout;
    return a;
}

vk::SubpassDescription vkx::IGraphicsSubpass(
    vkx_slice_t<const vk::AttachmentReference> colorAttachmentRefs,
    const vk::AttachmentReference& depthStencilAttachment)
{
    vk::SubpassDescription subpass{};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = colorAttachmentRefs.size();
    subpass.pColorAttachments = colorAttachmentRefs.data();

    if (depthStencilAttachment.attachment) {  // valid
        subpass.pDepthStencilAttachment = &depthStencilAttachment;
    }
    return subpass;
}


vk::SubpassDependency vkx::ISubpassDependency(
    uint32_t srcSubpass, uint32_t dstSubpass,
    vk::PipelineStageFlags srcStageMask, vk::PipelineStageFlags dstStageMask,
    vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask,
    vk::DependencyFlags dependencyFlags)
{
    vk::SubpassDependency dependency{};
    dependency.srcSubpass = srcSubpass;
    dependency.dstSubpass = dstSubpass;
    dependency.srcStageMask = srcStageMask;
    dependency.dstStageMask = dstStageMask;
    dependency.srcAccessMask = srcAccessMask;
    dependency.dstAccessMask = dstAccessMask;
    dependency.dependencyFlags = dependencyFlags;
    
    return dependency;
}

vk::ClearValue vkx::ClearValueColor(float r, float g, float b, float a)
{
    return vk::ClearValue{ .color = {{{r,g,b,a}}} };
}
vk::ClearValue vkx::ClearValueDepthStencil(float depth, uint32_t stencil)
{
    return vk::ClearValue{ .depthStencil = {depth, stencil} };
}


vk::Framebuffer vkx::CreateFramebuffer(
    vk::Extent2D wh,
    vk::RenderPass renderPass, 
    vkx_slice_t<const vk::ImageView> attachments)
{
    VKX_CTX_device_allocator;

    vk::FramebufferCreateInfo framebufferInfo{};
    framebufferInfo.width = wh.width;
    framebufferInfo.height = wh.height;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = attachments.size();
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.layers = 1;

    return device.createFramebuffer(framebufferInfo, allocator);
}

#pragma endregion


#pragma region Graphics Pipeline, Descriptor


vk::PipelineVertexInputStateCreateInfo IPipelineVertexInputState(
    vkx_slice_t<vk::Format> attribFormats,
    uint32_t attribBinding,
    vk::VertexInputBindingDescription& bindingDesc,
    std::vector<vk::VertexInputAttributeDescription>& attribsDesc)
{
    attribsDesc.resize(attribFormats.size());
    uint32_t i = 0;
    uint32_t offset = 0;
    for (vk::Format fmt : attribFormats)
    {
        attribsDesc[i] = vk::VertexInputAttributeDescription{
                .location = i,
                .binding = attribBinding,
                .format = fmt,
                .offset = offset
        };
        offset += vkx::FormatSize(fmt);
        ++i;
    }

    bindingDesc = {};
    bindingDesc.binding = attribBinding;
    bindingDesc.stride = offset;
    bindingDesc.inputRate = vk::VertexInputRate::eVertex;

    vk::PipelineVertexInputStateCreateInfo vertexInputState{};
    vertexInputState.vertexBindingDescriptionCount = attribFormats.size() ? 1 : 0;  // if no attribs info, this is an empty vertex input, so set 0.
    vertexInputState.pVertexBindingDescriptions = &bindingDesc;
    vertexInputState.vertexAttributeDescriptionCount    = attribsDesc.size();
    vertexInputState.pVertexAttributeDescriptions       = attribsDesc.data();

    return vertexInputState;
}
vk::PipelineColorBlendAttachmentState vkx::IPipelineColorBlendAttachment(
    bool blendEnable,
    vk::BlendFactor srcColorBlendFactor,
    vk::BlendFactor dstColorBlendFactor,
    vk::BlendOp colorBlendOp,
    vk::BlendFactor srcAlphaBlendFactor,
    vk::BlendFactor dstAlphaBlendFactor,
    vk::BlendOp alphaBlendOp,
    vk::ColorComponentFlags colorWriteMask)
{
    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.blendEnable = blendEnable;
    colorBlendAttachment.srcColorBlendFactor = srcColorBlendFactor;
    colorBlendAttachment.dstColorBlendFactor = dstColorBlendFactor;
    colorBlendAttachment.colorBlendOp = colorBlendOp;
    colorBlendAttachment.srcAlphaBlendFactor = srcAlphaBlendFactor;
    colorBlendAttachment.dstAlphaBlendFactor = dstAlphaBlendFactor;
    colorBlendAttachment.alphaBlendOp = alphaBlendOp;
    colorBlendAttachment.colorWriteMask = colorWriteMask;
    return colorBlendAttachment;
}

static vk::PipelineShaderStageCreateInfo _CreateShaderModule_IPipelineShaderStage(
    std::span<const char> code,
    vk::ShaderStageFlagBits stage)
{
    VKX_CTX_device_allocator;

    vk::ShaderModuleCreateInfo shaderInfo{};
    shaderInfo.codeSize = code.size();
    shaderInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    vk::ShaderModule shaderModule = device.createShaderModule(shaderInfo, allocator);

    vk::PipelineShaderStageCreateInfo stageInfo{};
    stageInfo.stage = stage;
    stageInfo.module = shaderModule;
    stageInfo.pName = "main";

    return stageInfo;
}


vkx::GraphicsPipeline* vkx::CreateGraphicsPipeline(
    vkx_slice_t<std::pair<std::span<const char>, vk::ShaderStageFlagBits>> shaderStageSources,
    std::initializer_list<vk::Format> vertexInputAttribFormats,
    vk::DescriptorSetLayout descriptorSetLayout,
    vkx_slice_t<vk::PushConstantRange> pushConstantRanges,
    vkx::FnArg_CreateGraphicsPipeline args,
    std::vector<vk::DescriptorSet>* pDescriptorSets,
    vk::RenderPass renderPass,
    uint32_t subpass)
{
    VKX_CTX_device_allocator;

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
    for (auto& it : shaderStageSources) 
    {
        shaderStages.push_back(_CreateShaderModule_IPipelineShaderStage(it.first, it.second));
    }

    vk::VertexInputBindingDescription                   _VertexInput_BindingDesc;
    std::vector<vk::VertexInputAttributeDescription>    _VertexInput_AttribDesc;
    vk::PipelineVertexInputStateCreateInfo vertexInputState = 
        IPipelineVertexInputState(vertexInputAttribFormats, 0, _VertexInput_BindingDesc, _VertexInput_AttribDesc);


    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState{};
    inputAssemblyState.topology = args.topology;
    inputAssemblyState.primitiveRestartEnable = VK_FALSE;

    //vk::PipelineTessellationStateCreateInfo tessellationState{};
    //tessellationState.patchControlPoints = ;

    vk::PipelineViewportStateCreateInfo viewportState{};
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    vk::PipelineRasterizationStateCreateInfo rasterizationState{};
    rasterizationState.polygonMode = vk::PolygonMode::eFill;
    rasterizationState.cullMode = vk::CullModeFlagBits::eBack;
    rasterizationState.frontFace = vk::FrontFace::eCounterClockwise;
    rasterizationState.depthBiasEnable = VK_FALSE;
    rasterizationState.depthClampEnable = VK_FALSE;
    rasterizationState.rasterizerDiscardEnable = VK_FALSE;
    rasterizationState.lineWidth = 1.0f;

    vk::PipelineMultisampleStateCreateInfo multisampleState{};
    multisampleState.sampleShadingEnable = VK_FALSE;
    multisampleState.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multisampleState.minSampleShading = 1.0f; // Optional
    multisampleState.pSampleMask = nullptr; // Optional
    multisampleState.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampleState.alphaToOneEnable = VK_FALSE; // Optional

    vk::PipelineDepthStencilStateCreateInfo depthStencilState{};
    depthStencilState.depthTestEnable = VK_TRUE;
    depthStencilState.depthWriteEnable = VK_TRUE;
    depthStencilState.depthCompareOp = vk::CompareOp::eLess;
    depthStencilState.depthBoundsTestEnable = VK_FALSE;
    depthStencilState.minDepthBounds = 0.0f;
    depthStencilState.maxDepthBounds = 1.0f;
    depthStencilState.stencilTestEnable = VK_FALSE;
    depthStencilState.front = {};
    depthStencilState.back = {};

    vk::PipelineColorBlendStateCreateInfo colorBlendState{};
    colorBlendState.logicOpEnable = VK_FALSE;
    colorBlendState.logicOp = vk::LogicOp::eCopy;
    colorBlendState.attachmentCount = args.colorBlendAttachments.size();
    colorBlendState.pAttachments = args.colorBlendAttachments.data();  // for each Framebuffer
    colorBlendState.blendConstants[0] = 0.0f; // Optional
    colorBlendState.blendConstants[1] = 0.0f; 
    colorBlendState.blendConstants[2] = 0.0f; 
    colorBlendState.blendConstants[3] = 0.0f; 

    vk::PipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.dynamicStateCount = args.dynamicStates.size();
    dynamicState.pDynamicStates    = args.dynamicStates.data();


    vk::PipelineLayout pipelineLayout = vkx::CreatePipelineLayout(descriptorSetLayout, pushConstantRanges);


    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.stageCount         = shaderStages.size();
    pipelineInfo.pStages            = shaderStages.data();
    pipelineInfo.pVertexInputState  = &vertexInputState;
    pipelineInfo.pInputAssemblyState= &inputAssemblyState;
    pipelineInfo.pTessellationState = nullptr;
    pipelineInfo.pViewportState     = &viewportState;
    pipelineInfo.pRasterizationState= &rasterizationState;
    pipelineInfo.pMultisampleState  = &multisampleState;
    pipelineInfo.pDepthStencilState = &depthStencilState;
    pipelineInfo.pColorBlendState   = &colorBlendState;
    pipelineInfo.pDynamicState      = &dynamicState;

    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = subpass;
    pipelineInfo.basePipelineHandle = nullptr;
    pipelineInfo.basePipelineIndex  = 0;

    vk::Pipeline pipeline = vkx::check(device.createGraphicsPipeline(nullptr, pipelineInfo, allocator));

    for (auto& it : shaderStages)
    {
        device.destroyShaderModule(it.module, allocator);
    }

    vkx::GraphicsPipeline* gp = new vkx::GraphicsPipeline();
    gp->Pipeline = pipeline;
    gp->PipelineLayout = pipelineLayout;
    gp->DescriptorSetLayout = descriptorSetLayout;

    if (pDescriptorSets)
    {
        const int fif = vkxc.InflightFrames;
        pDescriptorSets->resize(fif);
        std::vector<vk::DescriptorSetLayout> layouts(fif, descriptorSetLayout);
        vkx::AllocateDescriptorSets(fif, pDescriptorSets->data(), layouts.data());
    }

    return gp;
}


vk::PipelineLayout vkx::CreatePipelineLayout(
    vkx_slice_t<vk::DescriptorSetLayout> setLayouts,
    vkx_slice_t<vk::PushConstantRange> pushConstantRanges)
{
    VKX_CTX_device_allocator;

    vk::PipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.setLayoutCount = setLayouts.size();
    layoutInfo.pSetLayouts = setLayouts.data();
    layoutInfo.pushConstantRangeCount = pushConstantRanges.size();
    layoutInfo.pPushConstantRanges = pushConstantRanges.data();

    return device.createPipelineLayout(layoutInfo, allocator);
}




//vkx::GraphicsPipeline::GraphicsPipeline(vk::Pipeline pipeline, vk::PipelineLayout pipelineLayout, vk::DescriptorSetLayout descriptorSetLayout, const std::vector<vk::DescriptorSet>& descriptorSets) 
//    : Pipeline(pipeline), PipelineLayout(pipelineLayout), DescriptorSetLayout(descriptorSetLayout), DescriptorSets(descriptorSets) {}

vkx::GraphicsPipeline::~GraphicsPipeline()
{
    VKX_CTX_device_allocator;

    device.destroyPipeline(Pipeline, allocator);
    device.destroyPipelineLayout(PipelineLayout, allocator);

    device.destroyDescriptorSetLayout(DescriptorSetLayout, allocator);
}






vkx::UniformBuffer::UniformBuffer(vk::Buffer buf, vk::DeviceMemory mem, void* p, size_t n)
    : buffer(buf), bufferMemory(mem), mapped(p), size(n) {}

vkx::UniformBuffer::~UniformBuffer()
{
    VKX_CTX_device_allocator;
    device.destroyBuffer(buffer, allocator);
    device.freeMemory(bufferMemory, allocator);
}

void vkx::UniformBuffer::Upload(void* data)
{
    std::memcpy(mapped, data, size);
}

vkx::UniformBuffer* vkx::CreateUniformBuffer(vk::DeviceSize size)
{
    vk::DeviceMemory mem;
    vk::Buffer buffer = vkx::CreateBuffer(size, mem,
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    void* mapped = vkx::MapMemory(mem, size);

    return new vkx::UniformBuffer(buffer, mem, mapped, size);
}






vk::DescriptorSetLayout vkx::CreateDescriptorSetLayout(
    vkx_slice_t<std::pair<vk::DescriptorType, vk::ShaderStageFlags>> bindings, uint32_t firstBinding)
{
    VKX_CTX_device_allocator;

    std::vector<vk::DescriptorSetLayoutBinding> ls;
    ls.reserve(bindings.size());

    uint32_t bindingIdx = firstBinding;
    for (auto& it : bindings) {
        ls.push_back(vk::DescriptorSetLayoutBinding{
            .binding = bindingIdx,
            .descriptorType = it.first,
            .descriptorCount = 1,
            .stageFlags = it.second,
            .pImmutableSamplers = nullptr
        });
        ++bindingIdx;
    }

    vk::DescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.bindingCount = ls.size();
    layoutInfo.pBindings = ls.data();

    return device.createDescriptorSetLayout(layoutInfo, allocator);
}


void vkx::AllocateDescriptorSets(
    uint32_t descriptorSetCount,
    vk::DescriptorSet* out_DescriptorSets, 
    vk::DescriptorSetLayout* descriptorSetLayouts)
{
    VKX_CTX_device_allocator;

    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.descriptorPool = vkxc.DescriptorPool;
    allocInfo.descriptorSetCount = descriptorSetCount;
    allocInfo.pSetLayouts = descriptorSetLayouts;

    VKX_CHECK(device.allocateDescriptorSets(&allocInfo, out_DescriptorSets));
}



vk::DescriptorBufferInfo vkx::IDescriptorBuffer(const vkx::UniformBuffer* ub)
{
    vk::DescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = ub->buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = ub->size;
    return bufferInfo;
}

vk::DescriptorImageInfo vkx::IDescriptorImage(vk::ImageView imageView, vk::Sampler sampler)
{
    vk::DescriptorImageInfo imageInfo{};
    imageInfo.sampler = sampler;
    imageInfo.imageView = imageView;
    imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    return imageInfo;
}


void vkx::WriteDescriptorSet(
    vk::DescriptorSet descriptorSet, 
    std::initializer_list<vkx::FnArg_WriteDescriptor> writeDescriptors,
    uint32_t firstBinding)
{
    VKX_CTX_device;

    std::vector<vk::WriteDescriptorSet> ls;
    ls.reserve(writeDescriptors.size());

    int bindingIdx = firstBinding;
    for (auto& it : writeDescriptors)
    {
        vk::WriteDescriptorSet descriptorWrite{};
        descriptorWrite.dstSet = descriptorSet;
        descriptorWrite.dstBinding = bindingIdx;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorCount = 1;

        assert((it.buffer.buffer != 0) != (it.image.imageView != 0) && "requires only one is valid.");

        if (it.buffer.buffer)
        {
            descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
            descriptorWrite.pBufferInfo = &it.buffer;
        } 
        else
        {
            descriptorWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
            descriptorWrite.pImageInfo = &it.image;
        }

        ls.push_back(descriptorWrite);
        ++bindingIdx;
    }

    device.updateDescriptorSets(ls, {});
}


vk::PushConstantRange vkx::IPushConstantRange(vk::ShaderStageFlags shaderStageFlags, uint32_t size, uint32_t offset)
{
    vk::PushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = shaderStageFlags;
    pushConstantRange.offset = offset;
    pushConstantRange.size = size;
    return pushConstantRange;
}


#pragma endregion
































#pragma region Create: Instance & DebugMessenger, ValidationLayer

inline static VkDebugUtilsMessengerEXT g_DebugUtilsMessengerEXT = nullptr;
inline static std::vector<const char*> g_ValidationLayers = {
        "VK_LAYER_KHRONOS_validation"
};

static void _EXT_DestroyDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    assert(func);
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL _DebugMessengerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType, 
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
    void* pUserData) 
{
    //const char* MSG_SERV = "VERBO";
    //if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) MSG_SERV = "INFO";
    //else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) MSG_SERV = "WARN";
    //else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) MSG_SERV = "ERROR";
    //
    //const char* MSG_TYPE = "GENERAL";
    //if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) MSG_TYPE = "VALIDATION";
    //else if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) MSG_TYPE = "PERFORMANCE";
    //
    //if (messageSeverity != VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    //{
    //    std::cerr << "VkLayer[" << MSG_SERV << "][" << MSG_TYPE  << "]: " << pCallbackData->pMessage << std::endl;
    //
    //    std::cerr.flush();
    //}

    vkx::ctx().DebugMessengerCallback(
        static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity), 
        static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageType),
        pCallbackData);

    return VK_FALSE;
}


// for VkInstance, not VkDevice.
static void _CheckValidationLayersSupport(const std::vector<const char*>& validationLayers) 
{
    std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

    for (const char* layerName : validationLayers) {
        bool found = false;
        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            std::stringstream ss;
            for (const auto& layerProperties : availableLayers) {
                ss << layerProperties.layerName << ", ";
            }
            throw std::runtime_error(std::format("Required validation layer '{}' is not available. There are {} layers available: {}", layerName, availableLayers.size(), ss.str()));
        }
    }
}

static vk::Instance _CreateInstance(
    bool enableValidationLayer, 
    const std::vector<const char*>& in_extensions)
{
    vk::ApplicationInfo appInfo{
        .pApplicationName = "N/A",
        .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(0, 0, 1),
        .apiVersion = VK_API_VERSION_1_0
    };

    vk::InstanceCreateInfo instInfo{};
    instInfo.pApplicationInfo = &appInfo;


    // Instance Extensions

    std::vector<const char*> extensions = in_extensions;

#ifdef __APPLE__
    // for prevents VK_ERROR_INCOMPATIBLE_DRIVER err on MacOS MoltenVK sdk.
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    instInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    // for supports VK_KHR_portability_subset logical-device-extension on MacOS.
    extensions.push_back("VK_KHR_get_physical_device_properties2");
#endif

    if (enableValidationLayer) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    instInfo.enabledExtensionCount = extensions.size();
    instInfo.ppEnabledExtensionNames = extensions.data();


    vk::DebugUtilsMessengerCreateInfoEXT debugMessengerInfo{};
    if (enableValidationLayer)
    {
        _CheckValidationLayersSupport(g_ValidationLayers);

        instInfo.enabledLayerCount = g_ValidationLayers.size();
        instInfo.ppEnabledLayerNames = g_ValidationLayers.data();

        debugMessengerInfo.messageSeverity =
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | 
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
        debugMessengerInfo.messageType =
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;

        debugMessengerInfo.pfnUserCallback = _DebugMessengerCallback;
        debugMessengerInfo.pUserData = nullptr; // Optional
        instInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessengerInfo;
    }

    vk::Instance instance = vk::createInstance(instInfo, vkx::ctx().Allocator);

    if (enableValidationLayer)
    {
        // Setup EXT DebugMessenger
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        assert(func != nullptr);
        vkx::check(func(instance, (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessengerInfo, nullptr, &g_DebugUtilsMessengerEXT));
    }


    return instance;
}
#pragma endregion

#pragma region Create: PhysDevice, Device, Queue


static VkPhysicalDevice _PickPhysicalDevice(
    vk::Instance inst,
    vk::PhysicalDeviceProperties* out_pProps, 
    vk::PhysicalDeviceFeatures* out_pFeats,
    vk::PhysicalDeviceMemoryProperties* out_pMemProps)
{
    std::vector<vk::PhysicalDevice> gpus = inst.enumeratePhysicalDevices();
    vk::PhysicalDevice gpu = gpus.front();

    *out_pProps = gpu.getProperties();
    *out_pFeats = gpu.getFeatures();
    *out_pMemProps = gpu.getMemoryProperties();
    return gpu;
}



static vkx::QueueFamilyIndices _FindQueueFamilies(vk::PhysicalDevice& physDevice, vk::SurfaceKHR& surfaceKHR)
{
    std::vector<vk::QueueFamilyProperties> queueFamilies = physDevice.getQueueFamilyProperties();

    vkx::QueueFamilyIndices queueFamilyIdxs;

    // todo: Find the BestFit queue for specific QueueFlag. https://stackoverflow.com/a/57210037
    int i = 0;
    for (const auto& queueFamily : queueFamilies) 
    {
        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            queueFamilyIdxs.GraphicsFamily = i;
        }

        uint32_t supportPresent = physDevice.getSurfaceSupportKHR(i, surfaceKHR);
        if (supportPresent) {
            queueFamilyIdxs.PresentFamily = i;
        }

        if (queueFamilyIdxs.isComplete())
            break;
        i++;
    }

    return queueFamilyIdxs;
}

static vk::Device _CreateLogicalDevice(
    const vk::PhysicalDevice& physDevice,
    const vkx::QueueFamilyIndices& queueFamily,
    vk::Queue* out_GraphicsQueue,
    vk::Queue* out_PresentQueue)
{
    vk::DeviceCreateInfo deviceInfo{};

    // Queue Family
    float queuePriority = 1.0f;  // 0.0-1.0

    std::set<uint32_t> uniqQueueFamilyIdx = { queueFamily.GraphicsFamily, queueFamily.PresentFamily };
    std::vector<vk::DeviceQueueCreateInfo> arrQueueCreateInfo;
    for (uint32_t queueFamilyIdx : uniqQueueFamilyIdx) {
        vk::DeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.pQueuePriorities = &queuePriority;

        queueCreateInfo.queueFamilyIndex = queueFamilyIdx;
        queueCreateInfo.queueCount = 1;
        arrQueueCreateInfo.push_back(queueCreateInfo);
    }

    deviceInfo.pQueueCreateInfos = arrQueueCreateInfo.data();
    deviceInfo.queueCreateInfoCount = arrQueueCreateInfo.size();

    // Device Features
    vk::PhysicalDeviceFeatures deviceFeatures = vkx::ctx().PhysDeviceFeatures;
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceInfo.pEnabledFeatures = &deviceFeatures;


    // pNext extensions
    void* pExtNext = nullptr;

#ifdef VKX_EXT_BARYCENTRIC

    vk::PhysicalDeviceFragmentShaderBarycentricFeaturesKHR extBaryCoord{};
    extBaryCoord.fragmentShaderBarycentric = true;

    //extBaryCoord.pNext = std::exchange(pExtNext, &extBaryCoord);
    extBaryCoord.pNext = pExtNext;
    pExtNext = &extBaryCoord;

#endif // VKX_EXT_BARYCENTRIC

    deviceInfo.pNext = pExtNext;


    // Device Extensions  (needs check is supported?)
    std::vector<const char*> deviceExtensions = {
            "VK_KHR_swapchain", //VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#ifdef VKX_EXT_BARYCENTRIC
            "VK_KHR_fragment_shader_barycentric", // VK_KHR_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME
#endif
#ifdef VKX_VIEWPORT_NEG_HEIGHT
            // VK_KHR_maintenance1 is required for using negative viewport heights
		    // Note: This is core as of Vulkan 1.1. So if you target 1.1 you don't have to explicitly enable this
            VK_KHR_MAINTENANCE1_EXTENSION_NAME,
#endif
#ifdef __APPLE__
            "VK_KHR_portability_subset"
#endif
    };

    deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();
    deviceInfo.enabledExtensionCount = deviceExtensions.size();


    vk::Device device = physDevice.createDevice(deviceInfo, vkx::ctx().Allocator);

    *out_GraphicsQueue = device.getQueue(queueFamily.GraphicsFamily, 0);
    *out_PresentQueue  = device.getQueue(queueFamily.PresentFamily,  0);
    return device;
}

#pragma endregion

#pragma region Create: CommandPool, DescriptorPool, SyncObjects(Semaphores, Fences), Main-RenderPass, etc.

static vk::CommandPool _CreateCommandPool(
    uint32_t queueFamily)
{
    VKX_CTX_device_allocator;

    return device.createCommandPool(
        vk::CommandPoolCreateInfo{
            .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            .queueFamilyIndex = queueFamily
        }, allocator);
}

static vk::DescriptorPool _CreateDescriptorPool(uint32_t n = 1000)
{
    VKX_CTX_device_allocator;
    // tho kinda oversize.
    vk::DescriptorPoolSize pool_sizes[] =
    {
            { vk::DescriptorType::eSampler,              n },
            { vk::DescriptorType::eCombinedImageSampler, n },
            { vk::DescriptorType::eSampledImage,         n },
            { vk::DescriptorType::eStorageImage,         n },
            { vk::DescriptorType::eUniformTexelBuffer,   n },
            { vk::DescriptorType::eStorageTexelBuffer,   n },
            { vk::DescriptorType::eUniformBuffer,        n },
            { vk::DescriptorType::eStorageBuffer,        n },
            { vk::DescriptorType::eUniformBufferDynamic, n },
            { vk::DescriptorType::eStorageBufferDynamic, n },
            { vk::DescriptorType::eInputAttachment,      n }
    };

    return device.createDescriptorPool(
        vk::DescriptorPoolCreateInfo{
            .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
            .maxSets = n,
            .poolSizeCount = std::size(pool_sizes),
            .pPoolSizes = pool_sizes
        }, allocator);
}

static void _CreateSyncObjects()
{
    VKX_CTX_device_allocator;

    vk::SemaphoreCreateInfo semaphoreInfo{};

    vk::FenceCreateInfo fenceInfo{};
    fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

    const int fif = vkxc.InflightFrames;
    vkxc.SemaphoreImageAcquired.resize(fif);
    vkxc.SemaphoreRenderComplete.resize(fif);
    vkxc.CommandBufferFences.resize(fif);
    for (int i = 0; i < fif; ++i)
    {
        vkxc.SemaphoreImageAcquired[i]  = device.createSemaphore(semaphoreInfo, allocator);
        vkxc.SemaphoreRenderComplete[i] = device.createSemaphore(semaphoreInfo, allocator);
        vkxc.CommandBufferFences[i] = device.createFence(fenceInfo, allocator);
    }
}

static void _CreateCommandBuffers()
{
    auto& vkxc = vkx::ctx();
    int fif = vkxc.InflightFrames;
    vkxc.CommandBuffers.resize(fif);
    vkx::AllocateCommandBuffers(fif, vkxc.CommandBuffers.data(), vk::CommandBufferLevel::ePrimary);
}


static vk::RenderPass _CreateMainRenderPass(
    vk::Format imageFormat  = vkx::ctx().SwapchainSurfaceFormat.format,
    vk::Format depthFormat  = vkx::ctx().SwapchainDepthImageFormat)
{
    vk::SubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;// | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;// | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dependency.srcAccessMask = vk::AccessFlagBits::eNone;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;// | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

    return vkx::CreateRenderPass(
        {
            vkx::IAttachmentDesc(imageFormat, vk::ImageLayout::ePresentSrcKHR)//,
            //vkx::IAttachmentDesc(depthFormat, vk::ImageLayout::eDepthStencilAttachmentOptimal)
        },
        vkx::IGraphicsSubpass(
            vkx::IAttachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal)//,           // Color
            //vkx::IAttachmentRef(1, vk::ImageLayout::eDepthStencilAttachmentOptimal)   // Depth
        ),
        dependency);
}

#pragma endregion

#pragma region Create: Swapchain


static void _CreateSwapchain(
    const vk::PhysicalDevice&	physDevice                  = vkx::ctx().PhysDevice,
    const vk::SurfaceKHR&		surfaceKHR                  = vkx::ctx().SurfaceKHR,
    const vkx::QueueFamilyIndices& queueFamily              = vkx::ctx().QueueFamily,
    vk::RenderPass              renderPass                  = vkx::ctx().MainRenderPass,
    vk::SurfaceFormatKHR        surfaceFormat               = vkx::ctx().SwapchainSurfaceFormat,
    vk::SwapchainKHR&		    out_SwapchainKHR            = vkx::ctx().SwapchainKHR,
    vk::Extent2D&				out_SwapchainExtent         = vkx::ctx().SwapchainExtent,
    std::vector<vk::Image>&		out_SwapchainImages         = vkx::ctx().SwapchainImages,
    std::vector<vk::ImageView>&	out_SwapchainImageViews     = vkx::ctx().SwapchainImageViews,
    vkx::Image*&                out_SwapchainDepthImage     = vkx::ctx().SwapchainDepthImage,
    std::vector<vk::Framebuffer>& out_SwapchainFramebuffers = vkx::ctx().SwapchainFramebuffers)
{
    VKX_CTX_device_allocator;

    vk::SurfaceCapabilitiesKHR surfaceCapabilities = physDevice.getSurfaceCapabilitiesKHR(surfaceKHR);

    out_SwapchainExtent = surfaceCapabilities.currentExtent;
    assert(surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() && "VkxError: invalid VkSurfaceCapabilitiesKHR.currentExtent.width");
    // 0x0 extent may happens when Window Minimized, and 0x0 will cause fail to create swapchain.
    //out_SwapchainExtent.width  = std::max(1, (int)out_SwapchainExtent.width);
    //out_SwapchainExtent.height = std::max(1, (int)out_SwapchainExtent.height);
    assert(out_SwapchainExtent.width > 0 && out_SwapchainExtent.height > 0);

    vk::PresentModeKHR surfacePresentMode = vk::PresentModeKHR::eFifo;  // FIFO is vk guaranteed available.
//    {
//        for (auto mode : physDevice.getSurfacePresentModesKHR(surfaceKHR)) {
//            if (mode == vk::PresentModeKHR::eMailbox)  // MAILBOX avoid tearing while still maintaining a fairly low latency by rendering new images that are as up-to-date as possible right until the vertical blank.
//                surfacePresentMode = vk::PresentModeKHR::eMailbox;
//        }
//    }

    uint32_t swapchainImageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && swapchainImageCount > surfaceCapabilities.maxImageCount) {
        swapchainImageCount = surfaceCapabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR swapchainInfo{};
    swapchainInfo.surface = surfaceKHR;
    swapchainInfo.minImageCount = swapchainImageCount;
    swapchainInfo.imageFormat = surfaceFormat.format;
    swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainInfo.imageExtent = out_SwapchainExtent;
    swapchainInfo.imageArrayLayers = 1;  // normally 1, except VR.
    swapchainInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

    // Image Share Mode. Queue Family.
    uint32_t queueFamilyIdxs[] = { queueFamily.GraphicsFamily, queueFamily.PresentFamily };
    if (queueFamily.GraphicsFamily != queueFamily.PresentFamily) {
        swapchainInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        swapchainInfo.queueFamilyIndexCount = 2;
        swapchainInfo.pQueueFamilyIndices = queueFamilyIdxs;
    } else {
        swapchainInfo.imageSharingMode = vk::SharingMode::eExclusive;  // most performance.
    }

    swapchainInfo.preTransform = surfaceCapabilities.currentTransform;  // Non transform
    swapchainInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;  // Non Alpha.
    swapchainInfo.presentMode = surfacePresentMode;
    swapchainInfo.clipped = VK_TRUE;  // true: not care the pixels behind other windows for vk optims.  but may cause problem when we read the pixels.
    swapchainInfo.oldSwapchain = nullptr;

    out_SwapchainKHR = device.createSwapchainKHR(swapchainInfo, allocator);

    // Get Swapchain Images.
    out_SwapchainImages = device.getSwapchainImagesKHR(out_SwapchainKHR);  // ?ImageAllocator
    assert(swapchainImageCount == out_SwapchainImages.size());

    // Swapchain ImageViews
    out_SwapchainImageViews.resize(swapchainImageCount);
    for (uint32_t i = 0; i < swapchainImageCount; ++i)
    {
        out_SwapchainImageViews[i] = vkx::CreateImageView(out_SwapchainImages[i], surfaceFormat.format);
    }

    // Swapchain Depth Image
    out_SwapchainDepthImage = vkx::CreateDepthImage(out_SwapchainExtent);

    // Swapchain Framebuffers
    out_SwapchainFramebuffers.resize(swapchainImageCount);
    for (size_t i = 0; i < swapchainImageCount; i++)
    {
        out_SwapchainFramebuffers[i] = vkx::CreateFramebuffer(out_SwapchainExtent, renderPass,
            { out_SwapchainImageViews[i] /*, out_SwapchainDepthImage->imageView */ });
    }
}

static void _DestroySwapchain(
    const std::vector<vk::Framebuffer>& swapchainFramebuffers = vkx::ctx().SwapchainFramebuffers,
    const std::vector<vk::ImageView>& swapchainImageViews = vkx::ctx().SwapchainImageViews,
    vkx::Image*& depthImage = vkx::ctx().SwapchainDepthImage,
    vk::SwapchainKHR swapchainKHR = vkx::ctx().SwapchainKHR)
{
    VKX_CTX_device_allocator;

    delete depthImage;

    for (auto fb : swapchainFramebuffers) {
        device.destroyFramebuffer(fb, allocator);
    }
    for (auto imageView : swapchainImageViews) {
        device.destroyImageView(imageView, allocator);
    }
    device.destroySwapchainKHR(swapchainKHR, allocator);
}

void vkx::RecreateSwapchain(bool destroy, bool create)
{
    vkx::ctx().Device.waitIdle();

    if (destroy)
    {
        _DestroySwapchain();
    }

    if (create)
    {
        _CreateSwapchain();
    }
}

void vkx::BeginMainRenderPass(vk::CommandBuffer cmdbuf)
{
    VKX_CTX_device_allocator;

    vkx::CommandBuffer(cmdbuf).BeginRenderPass(
        vkxc.MainRenderPass,
        vkxc.SwapchainFramebuffers[vkxc.CurrentSwapchainImage],
        vkxc.SwapchainExtent,
        {
            vkx::ClearValueColor(0, 0, 0, 1),
            vkx::ClearValueDepthStencil(1, 0)
        },
        vk::SubpassContents::eInline);
}

void vkx::EndMainRenderPass(vk::CommandBuffer cmdbuf)
{
    vkx::CommandBuffer(cmdbuf).EndRenderPass();
}


#pragma endregion


#pragma region vkx::Init(), vkx::Destroy()


vkx::InstanceContext& vkx::ctx()
{
    static vkx::InstanceContext _ctx{};
    return _ctx;
}

void vkx::Init(
    const void* surfaceWindowHandle,
    bool enableValidationLayer)
{
    auto& i = vkx::ctx();


    i.Instance = 
    _CreateInstance(enableValidationLayer, vkx::_Glfw_GetRequiredInstanceExtensions());
    i.SurfaceKHR =
    vkx::_Glfw_CreateSurfaceKHR(i.Instance, surfaceWindowHandle);

    i.PhysDevice =
    _PickPhysicalDevice(i.Instance, &i.PhysDeviceProperties, &i.PhysDeviceFeatures, &i.PhysDeviceMemoryProperties);

    i.QueueFamily = _FindQueueFamilies(i.PhysDevice, i.SurfaceKHR);
    i.Device =
    _CreateLogicalDevice(i.PhysDevice, i.QueueFamily, &i.GraphicsQueue, &i.PresentQueue);

    i.CommandPool =
    _CreateCommandPool(i.QueueFamily.GraphicsFamily);
    i.DescriptorPool =
    _CreateDescriptorPool();

    i.ImageSampler = vkx::CreateImageSampler();
    _CreateSyncObjects();
    _CreateCommandBuffers();

    // dependent by CreateMainRenderPass
    // auto surfaceFormats = i.PhysDevice.getSurfaceFormatsKHR(i.SurfaceKHR).front();
    i.SwapchainSurfaceFormat = { vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
    i.SwapchainDepthImageFormat = _FindDepthFormat();

    // dependent by CreateSwapchinFramebuffers
    i.MainRenderPass = _CreateMainRenderPass();

    // Create Swapchain
    vkx::RecreateSwapchain(false, true);
}

void vkx::Destroy()
{
    VKX_CTX_device_allocator;

    //  Destroy SyncObjects
    for (uint32_t i = 0; i < vkxc.InflightFrames; i++) {
        device.destroySemaphore(vkxc.SemaphoreImageAcquired[i],     allocator);
        device.destroySemaphore(vkxc.SemaphoreRenderComplete[i],    allocator);
        device.destroyFence(vkxc.CommandBufferFences[i], allocator);
    }

    // Destroy Swapchain
    vkx::RecreateSwapchain(true, false);

    device.destroyRenderPass(vkxc.MainRenderPass, allocator);

    device.destroyDescriptorPool(vkxc.DescriptorPool, allocator);
    device.destroyCommandPool(vkxc.CommandPool, allocator);
    device.destroySampler(vkxc.ImageSampler, allocator);

    device.destroy(allocator);
    vkxc.Instance.destroySurfaceKHR(vkxc.SurfaceKHR, allocator);

    if (g_DebugUtilsMessengerEXT) {
        _EXT_DestroyDebugMessenger(vkxc.Instance, g_DebugUtilsMessengerEXT, (VkAllocationCallbacks*)allocator);
    }
    vkxc.Instance.destroy(allocator);
}

vk::CommandBuffer vkx::BeginFrame()
{
    VKX_CTX_device_allocator;
    int fif_i = vkxc.CurrentInflightFrame;
    vkx::CommandBuffer cmd{ vkxc.CommandBuffers[fif_i] };

    // blocking until the CommandBuffer has finished executing
    VKX_CHECK(device.waitForFences(vkxc.CommandBufferFences[fif_i], true, UINT64_MAX));

    // acquire swapchain image, and signal SemaphoreImageAcquired[i] when acquired. (when the presentation engine is finished using the image)
    vkxc.CurrentSwapchainImage =
            vkx::check(device.acquireNextImageKHR(vkxc.SwapchainKHR, UINT64_MAX, vkxc.SemaphoreImageAcquired[fif_i]));

    device.resetFences(vkxc.CommandBufferFences[fif_i]);  // reset the fence to the unsignaled state

    cmd.Reset();
    cmd.Begin(vk::CommandBufferUsageFlagBits::eSimultaneousUse);
    return cmd;
}

void vkx::SubmitPresent(vk::CommandBuffer cmd)
{
    cmd.end();

    VKX_CTX_device_allocator;
    int fif_i = vkxc.CurrentInflightFrame;

    // Submit the CommandBuffer.
    // Submission is VerySlow. try Batch Submit as much as possible, and Submit in another Thread
    vkx::QueueSubmit(vkxc.GraphicsQueue, cmd,
                     vkxc.SemaphoreImageAcquired[fif_i], { vk::PipelineStageFlagBits::eColorAttachmentOutput },
                     vkxc.SemaphoreRenderComplete[fif_i],
                     vkxc.CommandBufferFences[fif_i]);

    vkx::QueuePresentKHR(vkxc.PresentQueue,
                         vkxc.SemaphoreRenderComplete[fif_i], vkxc.SwapchainKHR, vkxc.CurrentSwapchainImage);

    //    vkQueueWaitIdle(vkx::ctx().PresentQueue);  // BigWaste on GPU.

    vkxc.CurrentInflightFrame = (vkxc.CurrentInflightFrame + 1) % vkxc.InflightFrames;
}

#pragma endregion






#pragma region GLFW external

#if VKX_BACKEND == VKX_BACKEND_eGLFW

#include <GLFW/glfw3.h>

std::vector<const char*> vkx::_Glfw_GetRequiredInstanceExtensions()
{
    uint32_t glfwRequiredInstExtensionsCount = 0;
    const char** glfwRequiredInstExtensions = glfwGetRequiredInstanceExtensions(&glfwRequiredInstExtensionsCount);
    
    std::vector<const char*> extensions;
    for (uint32_t i = 0; i < glfwRequiredInstExtensionsCount; ++i)
    {
        extensions.push_back(glfwRequiredInstExtensions[i]);
    }
    return extensions;
}

vk::SurfaceKHR vkx::_Glfw_CreateSurfaceKHR(vk::Instance inst, const void* wnd)
{
    VkSurfaceKHR surfaceKHR;
    glfwCreateWindowSurface(inst, (GLFWwindow*)wnd, (VkAllocationCallbacks*)vkx::ctx().Allocator, &surfaceKHR);
    return surfaceKHR;
}

#endif // VKX_BACKEND == VKX_BACKEND_eGLFW

#pragma endregion