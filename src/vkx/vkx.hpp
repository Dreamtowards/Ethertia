
#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS  // use Designated Initializers
//#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>


#define vkx_slice_t vk::ArrayProxy
#define vkx_optional_t vk::Optional

#define VKX_CHECK(result) vkx::check(result)

#define VKX_CTX_device vk::Device& device = vkx::ctx().Device;
#define VKX_CTX_device_allocator const vk::Device& device = vkx::ctx().Device; \
    const vk::AllocationCallbacks* allocator = vkx::ctx().Allocator; \
    auto& vkxc = vkx::ctx();

#define VKX_BACKEND_eGLFW 1
#define VKX_BACKEND VKX_BACKEND_eGLFW

// works via vkx::CommandBuffer::SetViewport. origin still keep LeftTop (vulkan original). 
// only actually affects { gl_Position.y -> -.y } (y-inversion of the clip-space to framebuffer-space transform.)
#define VKX_VIEWPORT_NEG_HEIGHT 1


#include <functional>
#include <iostream>   // for default DebugMessengerCallback impl.


namespace vkx
{

	#pragma region base

	const vk::Result&	check(const vk::Result& r);
	const VkResult&		check(const VkResult& r);

	template<typename T>
	const T& check(const vk::ResultValue<T>& r)
	{
		vkx::check(r.result);
		return r.value;
	}

	uint32_t FormatSize(vk::Format fmt);

	#pragma endregion
	
	#pragma region ctx

	class Image
	{
	public:
		vk::Image image;
		vk::DeviceMemory imageMemory;
		vk::ImageView imageView;

		vk::Format format;
		int width;
		int height;


		Image(vk::Image image = {}, vk::DeviceMemory imageMemory = {},
			vk::Format format = {}, int width = 0, int height = 0, vk::ImageView imageView = {});

		~Image();
	};

	struct QueueFamilyIndices
	{
		uint32_t GraphicsFamily = UINT32_MAX;
		uint32_t PresentFamily	= UINT32_MAX;  //Surface Present.

		bool isComplete() const { return GraphicsFamily != UINT32_MAX && PresentFamily != UINT32_MAX; }
	};

	class InstanceContext
	{
	public:
		vk::AllocationCallbacks* Allocator = nullptr;

		vk::Instance Instance;
		vk::SurfaceKHR SurfaceKHR;

		vk::PhysicalDevice PhysDevice;
		vk::PhysicalDeviceProperties	PhysDeviceProperties;
		vk::PhysicalDeviceFeatures		PhysDeviceFeatures;
		vk::PhysicalDeviceMemoryProperties PhysDeviceMemoryProperties;

		vk::Device Device;
		vk::Queue GraphicsQueue;
		vk::Queue PresentQueue;
		vkx::QueueFamilyIndices QueueFamily;

		vk::CommandPool CommandPool;
		vk::DescriptorPool DescriptorPool;
		
		vk::SwapchainKHR			SwapchainKHR;
		vk::Extent2D				SwapchainExtent;
		std::vector<vk::Image>		SwapchainImages;
		std::vector<vk::ImageView>	SwapchainImageViews;
		std::vector<vk::Framebuffer>SwapchainFramebuffers;
		vkx::Image*					SwapchainDepthImage;
		vk::SurfaceFormatKHR		SwapchainSurfaceFormat;  // dependnt by CreateSwapchain and CreateMainRenderPass
		vk::Format 					SwapchainDepthImageFormat;

		vk::RenderPass				MainRenderPass;

		vk::Sampler ImageSampler;  // default sampler. VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT

		uint32_t CurrentSwapchainImage = 0;

		const uint32_t InflightFrames = 3;  // Frames InFlights
		uint32_t CurrentInflightFrame = 0;

		std::vector<vk::CommandBuffer>	CommandBuffers;
		std::vector<vk::Fence>			CommandBufferFences;
		std::vector<vk::Semaphore> SemaphoreImageAcquired;
		std::vector<vk::Semaphore> SemaphoreRenderComplete;

		std::function<void(
			vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
			vk::DebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
		)> DebugMessengerCallback = [](auto messageSeverity, auto messageType, auto pCallbackData)
			{
				const char* sSERV = "VERBO";
				if (messageSeverity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo) sSERV = "INFO";
				else if (messageSeverity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) sSERV = "WARN";
				else if (messageSeverity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError) sSERV = "ERROR";

				const char* sTYPE = "GENERAL";
				if (messageType == vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation) sTYPE = "VALIDATION";
				else if (messageType == vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance) sTYPE = "PERFORMANCE";
				else if (messageType == vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding) sTYPE = "DeviceAddressBinding";

				if (messageSeverity != vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose &&
					messageType		!= vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral)
				{
					std::cerr << "\n============================ VkMSG ["<<sSERV<<"] ["<<sTYPE<<"] @"<<pCallbackData->pMessageIdName<<" ============================\n";
					std::cerr << "VkDebugMessenger[" << sSERV << "][" << sTYPE << "]: " << pCallbackData->pMessage << "\n";
					std::cerr <<   "===============================================================\n\n";
					std::cerr.flush();
					if (messageSeverity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
					{
						throw 4;
					}
				}
			};
	};

	// Instance Context. available once vkx::Init() initialized.
	vkx::InstanceContext& ctx();

	std::vector<const char*>	_Glfw_GetRequiredInstanceExtensions();
	vk::SurfaceKHR				_Glfw_CreateSurfaceKHR(vk::Instance inst, const void* wnd);

	void Init(
		const void* surfaceWindowHandle,
		bool enableValidationLayer = true);

	void Destroy();


	void RecreateSwapchain(bool destroy = true, bool create = true);

	void BeginMainRenderPass(vk::CommandBuffer cmdbuf);
	
	void EndMainRenderPass(vk::CommandBuffer cmdbuf);

	vk::CommandBuffer BeginFrame();

	void SubmitPresent(vk::CommandBuffer cmd);

	#pragma endregion




	#pragma region AllocateMemory, Buffer

	vk::DeviceMemory AllocateMemory(
		vk::MemoryRequirements memRequirements,  // size, alignment, memoryType
		vk::MemoryPropertyFlags memProperties);


	vk::Buffer CreateBuffer(
		vk::DeviceSize size,
		vk::DeviceMemory& out_BufferMemory,
		vk::BufferUsageFlags usage,
		vk::MemoryPropertyFlags memProperties);


	// SubmitCommandBuffer();
	void CopyBuffer(
		vk::Buffer srcBuffer,
		vk::Buffer dstBuffer, 
		vk::DeviceSize size);


	vk::Buffer CreateStagedBuffer(
		const void* bufferData,
		vk::DeviceSize bufferSize,
		vk::DeviceMemory& out_BufferMemory,
		vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eVertexBuffer);


	void* MapMemory(vk::DeviceMemory deviceMemory, vk::DeviceSize size, vk::DeviceSize offset = 0);

	void UnmapMemory(vk::DeviceMemory deviceMemory);


	class VertexBuffer
	{
	public:
		vk::Buffer			vertexBuffer;
		vk::DeviceMemory	vertexBufferMemory;
		vk::Buffer			indexBuffer;
		vk::DeviceMemory	indexBufferMemory;
		uint32_t			vertexCount;  // useful when vkCmdDraw()

		VertexBuffer(vk::Buffer vb, vk::DeviceMemory vbm, vk::Buffer ib, vk::DeviceMemory ibm, uint32_t vcount);

		~VertexBuffer();
	};

	// way too high-leveled. no reason/usage exists now. (use external loader instead). and should name as LoadVertexData() ?
	//vkx::VertexBuffer* LoadVertexBuffer(
	//	uint32_t vertexCount,
	//	vkx_slice_t<const char> vertices,
	//	vkx_slice_t<const char> indices = {});


	#pragma endregion


	#pragma region CommandBuffer, QueueSubmit

	void AllocateCommandBuffers(
		int numCmdbuf,
		vk::CommandBuffer* pCmdbufs,
		vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary,
		vk::CommandPool commandPool = vkx::ctx().CommandPool,
		vk::Device device = vkx::ctx().Device);


	// Allocate & Record & Submit Onetime CommandBuffer
	void SubmitCommandBuffer(
		const std::function<void(vk::CommandBuffer)>& fn_record,
		vk::Queue queue = vkx::ctx().GraphicsQueue,
		vk::CommandPool commandPool = vkx::ctx().CommandPool);

	void QueueSubmit(
		vk::Queue queue,
		vkx_slice_t<vk::CommandBuffer> cmdbufs,
		vkx_slice_t<vk::Semaphore> waits = {},
		vkx_slice_t<vk::PipelineStageFlags> waitStages = {},  // vk::PipelineStageFlagBits::eColorAttachmentOutput
		vkx_slice_t<vk::Semaphore> signals = {},
		vk::Fence fence = {});

	vk::Result QueuePresentKHR(
		vk::Queue presentQueue,
		vkx_slice_t<vk::Semaphore> waitSemaphores,
		vkx_slice_t<vk::SwapchainKHR> swapchains,
		vkx_slice_t<uint32_t> imageIndices);

	class CommandBuffer
	{
	public:
		vk::CommandBuffer cmd;

		CommandBuffer(vk::CommandBuffer cmdbuf);

		operator VkCommandBuffer();

		operator vk::CommandBuffer();

		void Reset();

		void Begin(vk::CommandBufferUsageFlags usageFlags);

		void End();

		void BeginRenderPass(
			vk::RenderPass renderPass,
			vk::Framebuffer framebuffer,
			vk::Extent2D renderArea,
			vkx_slice_t<vk::ClearValue> clearValues,
			vk::SubpassContents subpassContents = vk::SubpassContents::eInline);

		void EndRenderPass();

		void BindDescriptorSets(vk::PipelineLayout pipelineLayout, vkx_slice_t<vk::DescriptorSet> descriptorSets, uint32_t firstSet = 0);

		template<typename ValueType>
		void PushConstants(vk::PipelineLayout pipelineLayout, vk::ShaderStageFlags stageFlags, ValueType values, uint32_t offset = 0)
		{
			cmd.pushConstants(pipelineLayout, stageFlags, offset, sizeof(values), &values);
		}

		void BindGraphicsPipeline(vk::Pipeline graphicsPipeline);

		void SetViewport(vk::Offset2D offset, vk::Extent2D extent, float minDepth = 0.0f, float maxDepth = 1.0f);

		void SetScissor(vk::Offset2D offset, vk::Extent2D extent);

		void BindVertexBuffers(vkx_slice_t<vk::Buffer> buffers, vkx_slice_t<vk::DeviceSize> offsets = {}, uint32_t firstBinding = 0);

		void BindIndexBuffer(vk::Buffer buffer, vk::DeviceSize offset = 0, vk::IndexType indexType = vk::IndexType::eUint32);

		void Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0);

		void DrawIndexed(uint32_t vertexCount);

	};

	#pragma endregion


	#pragma region Image, ImageView.

	vk::Image CreateImage(
		int width, int height,
		vk::DeviceMemory& out_ImageMemory,  // out
		vk::Format format = vk::Format::eB8G8R8A8Unorm,  // ?UNorm SRGB
		vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
		vk::MemoryPropertyFlags memProperties = vk::MemoryPropertyFlagBits::eDeviceLocal,
		vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
		bool isCubeMap = false);

	vk::ImageView CreateImageView(
		vk::Image image,
		vk::Format format = vk::Format::eR8G8B8A8Unorm,  // ?? SRGB or UNORM
		vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlagBits::eColor,
		vk::ImageViewType imageViewType = vk::ImageViewType::e2D);

	vk::Sampler CreateImageSampler(
		vk::Filter magFilter = vk::Filter::eNearest,
		vk::Filter minFilter = vk::Filter::eNearest,
		vk::SamplerAddressMode addressModeUVW = vk::SamplerAddressMode::eRepeat);


	// for Framebuffer attachments

	vkx::Image* CreateColorImage(
		vk::Extent2D wh,
		vk::Format format = vk::Format::eB8G8R8A8Unorm,
		vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eColorAttachment,
        vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlagBits::eColor);

	vkx::Image* CreateDepthImage(
		vk::Extent2D wh, 
		vk::Format depthFormat = vk::Format::eUndefined);  // eUndefined: _FindDepthFormat. 



	vkx::Image* CreateStagedImage(
		uint32_t width, uint32_t height, void* pixels);

	#pragma endregion


	#pragma region RenderPass, Framebuffer

	vk::RenderPass CreateRenderPass(
		vkx_slice_t<const vk::AttachmentDescription> attachments,
		vkx_slice_t<const vk::SubpassDescription> subpasses,
		vkx_slice_t<const vk::SubpassDependency> dependencies = {});

	vk::AttachmentDescription IAttachmentDesc(
		vk::Format format,
		vk::ImageLayout finalLayout);

	vk::AttachmentReference IAttachmentRef(
		uint32_t attachment,
		vk::ImageLayout layout);

	vk::SubpassDescription IGraphicsSubpass(
		vkx_slice_t<const vk::AttachmentReference> colorAttachmentRefs,
		const vk::AttachmentReference& depthStencilAttachment = {});

	vk::SubpassDependency ISubpassDependency(
		uint32_t srcSubpass, uint32_t dstSubpass,
		vk::PipelineStageFlags srcStageMask, vk::PipelineStageFlags dstStageMask,
		vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask,
		vk::DependencyFlags dependencyFlags = {});

	// ICmd
	//vk::RenderPassBeginInfo IRenderPassBegin(
	//	vk::RenderPass renderPass,
	//	vk::Framebuffer framebuffer,
	//	vk::Extent2D renderArea,
	//	vkx_slice_t<vk::ClearValue> clearValues);

	vk::ClearValue ClearValueColor(float r = 0, float g = 0, float b = 0, float a = 1);

	vk::ClearValue ClearValueDepthStencil(float depth = 1.0f, uint32_t stencil = 0);

	vk::Framebuffer CreateFramebuffer(
		vk::Extent2D wh,
		vk::RenderPass renderPass,
		vkx_slice_t<const vk::ImageView> attachments);

	#pragma endregion


	#pragma region GraphicsPipeline, Descriptor


	class GraphicsPipeline
	{
	public:
		vk::Pipeline		Pipeline;

		// Actually these separate vulkan objects can apply on multiple Pipelines instead of just belong to one.
		// redesign this (seprate them) later when needs.

		// Shader Inputs (DescriptorSetLayouts, PushConstants)
		// although PipelineLayout is a separate/independent vulkan object, But it is highly related to the pipeline, 
		// so it is managed together to increase cohesion.
		vk::PipelineLayout	PipelineLayout;

		// although DescriptorSetLayout is a separate vulkan object, you can use it with multiple pipelines and manage them independently 
		// for added flexibility, But that will increase the coupling and complexity of the system. so it is managed together to increase cohesion.
		vk::DescriptorSetLayout DescriptorSetLayout;


		//GraphicsPipeline(vk::Pipeline pipeline, vk::PipelineLayout pipelineLayout, vk::DescriptorSetLayout descriptorSetLayout,
		//	const std::vector<vk::DescriptorSet>& descriptorSets) : Pipeline(pipeline), PipelineLayout(pipelineLayout),
		//	DescriptorSetLayout(descriptorSetLayout), DescriptorSets(descriptorSets) {}

		~GraphicsPipeline();
	};


	vk::PipelineColorBlendAttachmentState IPipelineColorBlendAttachment(
		bool blendEnable = true,
		vk::BlendFactor srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
		vk::BlendFactor dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
		vk::BlendOp colorBlendOp = vk::BlendOp::eAdd,
		vk::BlendFactor srcAlphaBlendFactor = vk::BlendFactor::eOne,
		vk::BlendFactor dstAlphaBlendFactor = vk::BlendFactor::eZero,
		vk::BlendOp alphaBlendOp = vk::BlendOp::eAdd,
		vk::ColorComponentFlags colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);


	struct FnArg_CreateGraphicsPipeline
	{
		vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList;
		// RasterizationState
		vk::PolygonMode		polygonMode = vk::PolygonMode::eFill;
		vk::CullModeFlags	cullMode = vk::CullModeFlagBits::eNone; // todo: CullBack
		vk::FrontFace		frontFace = vk::FrontFace::eCounterClockwise;
		std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments = { vkx::IPipelineColorBlendAttachment() };
		std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
	};

	vkx::GraphicsPipeline* CreateGraphicsPipeline(
		vkx_slice_t<std::pair<std::span<const char>, vk::ShaderStageFlagBits>> shaderStageSources,
		std::initializer_list<vk::Format> vertexInputAttribFormats,
		vk::DescriptorSetLayout descriptorSetLayout,
		vkx_slice_t<vk::PushConstantRange> pushConstantRanges,
		FnArg_CreateGraphicsPipeline args = {},
		std::vector<vk::DescriptorSet>* pDescriptorSets = nullptr,
		vk::RenderPass renderPass = {},
		uint32_t subpass = 0);

	vk::PipelineLayout CreatePipelineLayout(
		vkx_slice_t<vk::DescriptorSetLayout> setLayouts,
		vkx_slice_t<vk::PushConstantRange> pushConstantRanges = {});



	class UniformBuffer
	{
	public:
		vk::Buffer buffer;
		vk::DeviceMemory bufferMemory;
		void* mapped;
		size_t size;

		UniformBuffer(vk::Buffer buf, vk::DeviceMemory mem, void* mapped, size_t size);

		~UniformBuffer();

		void Upload(void* data);
	};

	vkx::UniformBuffer* CreateUniformBuffer(vk::DeviceSize size);




	vk::DescriptorSetLayout CreateDescriptorSetLayout(
		vkx_slice_t<std::pair<vk::DescriptorType, vk::ShaderStageFlags>> bindings, 
		uint32_t firstBinding = 0);


	void AllocateDescriptorSets(
		uint32_t descriptorSetCount,
		vk::DescriptorSet* out_DescriptorSets,
		vk::DescriptorSetLayout* descriptorSetLayouts);




	vk::DescriptorBufferInfo IDescriptorBuffer(const vkx::UniformBuffer* ub);

	vk::DescriptorImageInfo IDescriptorImage(vk::ImageView imageView, vk::Sampler sampler = vkx::ctx().ImageSampler);

	struct FnArg_WriteDescriptor
	{
		vk::DescriptorBufferInfo buffer{};
		vk::DescriptorImageInfo  image{};
	};

	void WriteDescriptorSet(
		vk::DescriptorSet descriptorSet,
		std::initializer_list<FnArg_WriteDescriptor> writeDescriptors,
		uint32_t firstBinding = 0);



	vk::PushConstantRange IPushConstantRange(vk::ShaderStageFlags shaderStageFlags, uint32_t size, uint32_t offset = 0);


	#pragma endregion








}