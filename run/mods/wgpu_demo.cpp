// TRIANGLE

#include <glfw3webgpu.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


#define WEBGPU_CPP_IMPLEMENTATION
#include <webgpu/webgpu.hpp>

#include <ethertia/util/Log.h>
#include <ethertia/util/BenchmarkTimer.h>

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(640, 480, "WebGPU", NULL, NULL);

    wgpu::Instance instance = wgpu::createInstance({});
    assert(instance);


    wgpu::Surface surface = glfwGetWGPUSurface(instance, window);
    assert(surface);


    wgpu::Adapter adapter = nullptr;
    instance.requestAdapter(
        wgpu::RequestAdapterOptions{ {
            .compatibleSurface = surface,
            .powerPreference = wgpu::PowerPreference::HighPerformance,
            .forceFallbackAdapter = false
        } },
        [&adapter](wgpu::RequestAdapterStatus status, wgpu::Adapter adap, char const* message) {
            assert(status == wgpu::RequestAdapterStatus::Success);
            adapter = adap;
        });
    assert(adapter);


    //int n = adapter.enumerateFeatures(nullptr);
    //std::vector<WGPUFeatureName> f;
    //f.resize(n);
    //adapter.enumerateFeatures((wgpu::FeatureName*)f.data());
    //for (auto f_ : f)
    //{
    //    Log::info("f {}", (int)f_);
    //}


    wgpu::Device device = adapter.requestDevice(
        wgpu::DeviceDescriptor{ {
            .label = "Device",
            .requiredFeaturesCount = 0,
            .requiredFeatures = 0,
            .requiredLimits = 0,
            .defaultQueue = {
                .nextInChain = nullptr,
                .label = "DDefault Queue"
            }
        //.deviceLostCallback
        //.deviceLostUserdata
    } });
    assert(device);

    device.setUncapturedErrorCallback(
        [](wgpu::ErrorType type, char const* message) {
            Log::warn("WGPU Device UncapturedError: {}", message);
        });

    wgpu::Queue queue = device.getQueue();



    wgpu::TextureFormat swapchainFormat = surface.getPreferredFormat(adapter);
    wgpu::SwapChain swapchain = device.createSwapChain(surface,
        wgpu::SwapChainDescriptor{ {
            .usage = WGPUTextureUsage_RenderAttachment,
            .format = swapchainFormat,
            .width = 640,
            .height = 480,
            .presentMode = WGPUPresentMode_Fifo,
        } });

    const char* shaderSource = R"(
    @vertex
    fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4f {
        var p = vec2f(0.0, 0.0);
        if (in_vertex_index == 0u) {
            p = vec2f(-0.5, -0.5);
        } else if (in_vertex_index == 1u) {
            p = vec2f(0.5, -0.5);
        } else {
            p = vec2f(0.0, 0.5);
        }
        return vec4f(p, 0.0, 1.0);
    }

    @fragment
    fn fs_main() -> @location(0) vec4f {
        return vec4f(0.0, 0.4, 1.0, 1.0);
    }
    )";

    wgpu::ShaderModuleWGSLDescriptor shaderCodeDesc;
    shaderCodeDesc.chain.next = nullptr;
    shaderCodeDesc.chain.sType = wgpu::SType::ShaderModuleWGSLDescriptor;
    shaderCodeDesc.code = shaderSource;

    wgpu::ShaderModuleDescriptor shaderDesc;
    shaderDesc.nextInChain = &shaderCodeDesc.chain;
    shaderDesc.label = "SDR2";
    shaderDesc.hintCount = 0;
    shaderDesc.hints = nullptr;

    wgpu::ShaderModule shaderModule = device.createShaderModule(shaderDesc);

    wgpu::BlendState blendState{};
    blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
    blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
    blendState.color.operation = wgpu::BlendOperation::Add;
    blendState.alpha.srcFactor = wgpu::BlendFactor::Zero;
    blendState.alpha.dstFactor = wgpu::BlendFactor::One;
    blendState.alpha.operation = wgpu::BlendOperation::Add;

    wgpu::ColorTargetState colorTarget{ WGPUColorTargetState{
        .format = swapchainFormat,
        .blend = &blendState,
        .writeMask = wgpu::ColorWriteMask::All,
    } };
    wgpu::FragmentState frag{ WGPUFragmentState{
        .module = shaderModule,
        .entryPoint = "fs_main",
        .constantCount = 0,
        .constants = nullptr,
        .targetCount = 1,
        .targets = &colorTarget,
    } };

    wgpu::RenderPipeline pipeline = device.createRenderPipeline({ WGPURenderPipelineDescriptor{
        .label = "Pipe2",
        .layout = nullptr,
        .vertex = {
            .module = shaderModule,
            .entryPoint = "vs_main",
            .constantCount = 0,
            .constants = nullptr,
            .bufferCount = 0,
            .buffers = nullptr,
        },
        .primitive = {
            .topology = wgpu::PrimitiveTopology::TriangleList,
            .stripIndexFormat = wgpu::IndexFormat::Undefined,
            .frontFace = wgpu::FrontFace::CCW,
            .cullMode = wgpu::CullMode::None,
        },
        .depthStencil = nullptr,
        .multisample = {
            .count = 1,
            .mask = ~0u,
            .alphaToCoverageEnabled = false,
        },
        .fragment = &frag,
    } });



    while (!glfwWindowShouldClose(window))
    {
        BENCHMARK_TIMER;
        glfwPollEvents();


        wgpu::TextureView tex = swapchain.getCurrentTextureView();
        if (!tex) {
            Log::info("Outdated");
        }

        wgpu::RenderPassColorAttachment colorAttachment{ WGPURenderPassColorAttachment  {
                .view = tex,
                .resolveTarget = nullptr,
                .loadOp = wgpu::LoadOp::Clear,
                .storeOp = wgpu::StoreOp::Store,
                .clearValue = wgpu::Color(0, 0, 1, 1),
            } };

        wgpu::CommandEncoder encoder = device.createCommandEncoder({ {.label = "CmdEnc1" } });
        wgpu::RenderPassEncoder renderPass = encoder.beginRenderPass(
            wgpu::RenderPassDescriptor{ {
                .label = "RenderPass1",
                .colorAttachmentCount = 1,
                .colorAttachments = &colorAttachment,
                .depthStencilAttachment = nullptr,
            } });

        renderPass.setPipeline(pipeline);

        renderPass.draw(3, 1, 0, 0);

        renderPass.end();


        tex.release();

        queue.submit(encoder.finish({}));

        swapchain.present();
    }

    swapchain.release();
    device.release();
    adapter.release();
    surface.release();
    instance.release();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}