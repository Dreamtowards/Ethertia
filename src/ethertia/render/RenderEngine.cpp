//
// Created by Dreamtowards on 2022/8/22.
//

#include "RenderEngine.h"


#include <ethertia/Ethertia.h>
#include <ethertia/render/Window.h>
#include <ethertia/util/BenchmarkTimer.h>
#include <ethertia/util/Log.h>

#include <ethertia/init/ItemTextures.h>
#include <ethertia/init/Settings.h>


#include <ethertia/imgui/Imgui.h>
#include <ethertia/imgui/Imw.h>
#include <ethertia/init/MaterialTextures.h>
#include <ethertia/material/Material.h>


#include <ethertia/world/World.h>
//#include <ethertia/entity/Entity.h>


////    SkyboxRenderer::init();     std::cout << "skybox ";
////    ParticleRenderer::init();   std::cout << "particle ";
// 
////    SSAORenderer::init();
////    ShadowRenderer::init();


#include "renderer/RendererGbuffer.cpp"



static vkx::Image* TEX_WHITE = nullptr;
static vkx::Image* TEX_UVMAP = nullptr;



/*
#include <daxa/daxa.hpp>


#include <GLFW/glfw3.h>
#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#endif
#include <GLFW/glfw3native.h>

auto get_native_window_handle(GLFWwindow* glfw_window_ptr) -> daxa::NativeWindowHandle
{
#if defined(_WIN32)
    return glfwGetWin32Window(glfw_window_ptr);
#elif defined(__linux__)
    return reinterpret_cast<daxa::NativeWindowHandle>(glfwGetX11Window(glfw_window_ptr));
#endif
}

auto get_native_window_platform() -> daxa::NativeWindowPlatform
{
#if defined(_WIN32)
    return daxa::NativeWindowPlatform::WIN32_API;
#elif defined(__linux__)
    return daxa::NativeWindowPlatform::XLIB_API;
#endif
}

void InitDaxa()
{
    daxa::Instance instance = daxa::create_instance({});

    daxa::Device device = instance.create_device({
        .selector = [](daxa::DeviceProperties const& device_props) -> daxa::i32
        {
            daxa::i32 score = 0;
            switch (device_props.device_type)
            {
            case daxa::DeviceType::DISCRETE_GPU: score += 10000; break;
            case daxa::DeviceType::VIRTUAL_GPU: score += 1000; break;
            case daxa::DeviceType::INTEGRATED_GPU: score += 100; break;
            default: break;
            }
            score += static_cast<daxa::i32>(device_props.limits.max_memory_allocation_count / 100000);
            return score;
        },
        .name = "my device",
    });

    std::cout << device.properties().device_name << std::endl;

    daxa::Swapchain swapchain = device.create_swapchain({
        // this handle is given by the windowing API
        .native_window = get_native_window_handle(Window::Handle()),
        // The platform would also be retrieved from the windowing API,
        // or by hard-coding it depending on the OS.
        .native_window_platform = get_native_window_platform(),
        // Here we can supply a user-defined surface format selection
        // function, to rate formats. If you don't care what format the
        // swapchain images are in, then you can just omit this argument
        // because it defaults to `daxa::default_format_score(...)`
        .surface_format_selector = [](daxa::Format format)
        {
            switch (format)
            {
            case daxa::Format::R8G8B8A8_UINT: return 100;
            default: return daxa::default_format_score(format);
            }
        },
        .present_mode = daxa::PresentMode::MAILBOX,
        .image_usage = daxa::ImageUsageFlagBits::TRANSFER_DST,
        .name = "my swapchain",
    });

    daxa::ImageId swapchain_image = swapchain.acquire_next_image();

    daxa::CommandRecorder recorder = device.create_command_recorder({ .name = "my command recorder" });

    recorder.pipeline_barrier_image_transition({
        .waiting_pipeline_access = daxa::AccessConsts::TRANSFER_WRITE,
        .before_layout = daxa::ImageLayout::UNDEFINED,
        .after_layout = daxa::ImageLayout::TRANSFER_DST_OPTIMAL,
        .image_slice = swapchain_image_full_slice,
        .image_id = swapchain_image,
    });
}
*/

void RenderEngine::Init()
{
    BENCHMARK_TIMER;
    Log::info("RenderEngine initializing..");

    //InitDaxa();

    vkx::Init(Window::Handle(), false);

    uint32_t vkApiVersion = vkx::ctx().PhysDeviceProperties.apiVersion;
    Log::info("Vulkan {}.{}.{}, {}",
              VK_API_VERSION_MAJOR(vkApiVersion),
              VK_API_VERSION_MINOR(vkApiVersion),
              VK_API_VERSION_PATCH(vkApiVersion),
              (const char*)vkx::ctx().PhysDeviceProperties.deviceName);


    Imgui::Init();


    TEX_WHITE = Loader::LoadImage(BitmapImage(1, 1, new uint32_t[1]{(uint32_t)~0}));
    TEX_UVMAP = Loader::LoadImage("misc/uvmap.png");

    //Materials::registerMaterialItems();  // before items tex load.
    MaterialTextures::Load();
    //MaterialTextures::ATLAS_DIFFUSE = MaterialTextures::ATLAS_NORM = MaterialTextures::ATLAS_DRAM = TEX_UVMAP;


    RendererGbuffer::Init();
    
    RendererCompose::Init(
        RendererGbuffer::gPosition->imageView,
        RendererGbuffer::gNormal->imageView,
        RendererGbuffer::gAlbedo->imageView);



    Log::info("RenderEngine initialized.\1");
}


void RenderEngine::Destroy()
{
    VKX_CTX_device_allocator;
    vkxc.Device.waitIdle();

    MaterialTextures::Destroy();
    ItemTextures::Destroy();

//    RendererGbuffer::deinit();

    delete TEX_WHITE;
    delete TEX_UVMAP;

    Imgui::Destroy();

    vkx::Destroy();
}

void RenderEngine::Render()
{
    /// skip
    if (Window::IsMinimized())  
        return;
    if (Window::isFramebufferResized())
        vkx::RecreateSwapchain();

    VKX_CTX_device_allocator;

    vkx::CommandBuffer cmd{nullptr};
    {
        ET_PROFILE("BeginFrame");
        cmd = vkx::BeginFrame();
    }


    World* world = Ethertia::GetWorld();
    if (world && !s_PauseWorldRender)
    {
        ImwGame::WorldImageView = RendererCompose::rtColor->imageView;
        {
            ET_PROFILE("CmdWorldGbuffer");
            RendererGbuffer::RecordCommand(cmd, world->registry());
        }
        {
            ET_PROFILE("CmdWorldCompose");
            RendererCompose::RecordCommand(cmd);
        }
    }

    vkx::BeginMainRenderPass(cmd);
    {
        ET_PROFILE("GUI");

        Imgui::Render(cmd);
    }
    vkx::EndMainRenderPass(cmd);

    {
        ET_PROFILE("SubmitPresent");
        vkx::SubmitPresent(cmd);
    }
}










//static void renderWorldGeometry(World* world)
//{
//
//    // Geometry of Deferred Rendering
//
//    PROFILE("Geo");
//    auto _ap = GeometryRenderer::fboGbuffer->BeginFramebuffer_Scoped();
//
//    RenderCommand::Clear();
//
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);
//
//    glDisable(GL_BLEND);  // Blending is inhabited in Deferred Rendering.
//
//
//    Dbg::dbg_NumEntityRendered = 0;
//    for (Entity* entity : world->getEntities()) {
//        if (!entity || !entity->m_Model) continue;
//        if (entity->m_Model->vertexCount == 0) // most are Empty Chunks.
//            continue;
//
//        // Frustum Culling
//        if (!Ethertia::getCamera().testFrustum(entity->getAABB())) {
//            continue;
//        }
//
//        if (entity == (void*)Ethertia::getPlayer() && Ethertia::getCamera().len == 0)
//            continue;
//
//
//        entity->onRender();
//
//
//        ++Dbg::dbg_NumEntityRendered;
//
//        // Debug: draw Norm/Border
//        if (Dbg::dbg_EntityGeo) {
//            DebugRenderer::Inst().renderDebugGeo(entity->m_Model, entity->position(), entity->getRotation());
//        }
//        // Debug: draw Entity that Crosshair Hits.
//        if (Dbg::dbg_HitPointEntityGeo && entity == Ethertia::getHitCursor().hitEntity) {
//            DebugRenderer::Inst().renderDebugGeo(entity->m_Model, entity->position(), entity->getRotation(),
//                           Ethertia::getHitCursor().brushSize, Ethertia::getHitCursor().position);
//        }
//        // Debug: draw Every Entity AABB.
//        if (Dbg::dbg_RenderedEntityAABB) {
//            AABB aabb = entity->getAABB();
//            Imgui::RenderAABB(aabb, Colors::RED);
//        }
//    }
//
//    glEnable(GL_BLEND);
//
//}


//static void renderWorldCompose(World* world)
//{
//    // Compose of Deferred Rendering
//
//    PROFILE("Cmp");
//    auto _ap = ComposeRenderer::fboCompose->BeginFramebuffer_Scoped();
//
//    glClearColor(0,0,0, 1);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
////    glEnable(GL_FRAMEBUFFER_SRGB);  // built-in Gamma Correction
//
//    float daytime = Ethertia::getWorld()->getDayTime();
//
//    SkyboxRenderer::renderWorldSkybox(daytime);
//
//    ParticleRenderer::renderSunMoonTex(daytime);
//
//    std::vector<Light*> renderLights;  // lights to be render.
////    {
////        static Light l{};
////        l.position = Ethertia::getCamera()->position;
////        l.color = glm::vec3(0.4, 1, 0.8);
////        l.direction = Ethertia::getCamera()->direction;
////        l.attenuation = {0,0,0};
////        renderLights.push_back(&l);
////    }
//    {
//        static Light sunLight;
//
//        // CNS 这是不准确的，不能直接取反 否则月亮位置可能会错误
//        sunLight.position = Ethertia::getCamera().position + (-RenderEngine::SunlightDir(daytime) * 100.0f);
//
//
//        float dayBrightness = 1.0 - abs(daytime-0.5) * 2.0;
//        dayBrightness = 0.4 + dayBrightness * 0.6;
//        glm::vec3 SunColor = Dbg::dbg_WorldSunColor * dayBrightness * Dbg::dbg_WorldSunColorBrightnessMul;
//        sunLight.color = SunColor;
//
//        sunLight.attenuation = {1, 0, 0};
//
//        renderLights.push_back(&sunLight);
//    }
//    //fixme: Do it later
////    for (Entity* e : world->m_Entities)
////    {
////        if (EntityLantern* lat = dynamic_cast<EntityLantern*>(e))
////        {
////            renderLights.push_back(lat->getLights());
////        }
////
////        if (EntityTorch* lat = dynamic_cast<EntityTorch*>(e))
////        {
////            renderLights.push_back(lat->getLights());
////
////            ParticleRenderer::m_Particles.push_back(lat->genParticles());
////        }
////    }
//
//
//    // CNS. 让接下来Alpha!=1.0的地方的颜色 添加到背景颜色中 为了接下来的天空颜色叠加
//    // Blend: addictive color when src.alpha != 1.0, for sky background add.
//    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
//
//    Texture** gbufferTexs = GeometryRenderer::fboGbuffer->texColor;
//    ComposeRenderer::renderCompose(gbufferTexs[0], gbufferTexs[1], gbufferTexs[2],
//                                   SSAORenderer::fboSSAO->texColor[0],
//                                   ShadowRenderer::fboDepthMap->texDepth, ShadowRenderer::matShadowSpace,
//                                   renderLights);
//
//    GlState::blendMode(GlState::ALPHA);
//
//    for (Entity* e : world->getEntities())
//    {
//        // todo: EntityDroppedItem :: onRenderCompose();
//        if (EntityDroppedItem* eDroppedItem = dynamic_cast<EntityDroppedItem*>(e))
//        {
//            const ItemStack& stack = eDroppedItem->m_DroppedItem;
//
//            float i = Item::REGISTRY.getOrderId(stack.item());
//            float n = Item::REGISTRY.size();
//
//            ParticleRenderer::render(ItemTextures::ITEM_ATLAS, eDroppedItem->position(), 0.3f,
//                                       {i/n, 0},
//                                       {1/n, 1});
//
//            if (stack.amount() > 1)
//            {
//                // fixme: tmp draw count.
////                Gui::drawWorldpoint(eDroppedItem->position(), [&](glm::vec2 p) {
////                    Gui::drawString(p.x, p.y, std::to_string(stack.amount()));
////                });
//            }
//        }
//    }
//
//    glEnable(GL_DEPTH_TEST);
//
//    ParticleRenderer::renderAll();
//
//}




// void RenderEngine::RenderWorld()
//    GeometryRenderer::PrepareFrame();
//
//    if (Dbg::dbg_PauseWorldRender) return;
//
//    RenderCommand::CheckError("Begin World Render");
//
//    ParticleRenderer::updateAll(Ethertia::getDelta());
//
//    // Heavy
//    renderWorldGeometry(world);
//
//
//    float CurrTime = Ethertia::getPreciseTime();
//    static float s_LastTimeRenderShadowDepthMap = 0;
//    if ((s_LastTimeRenderShadowDepthMap + Settings::gInterval_ShadowDepthMap) <= CurrTime)
//    {
//        PROFILE("ShadowDepthMap");
//
//        s_LastTimeRenderShadowDepthMap = CurrTime;
//        ShadowRenderer::renderDepthMap(world->m_Entities, SunlightDir(world->getDayTime()), !Settings::g_ShadowMapping);
//    }
//
//    {
//        PROFILE("SSAO");
//
//        SSAORenderer::renderSSAO(GeometryRenderer::fboGbuffer->texColor[0], GeometryRenderer::fboGbuffer->texColor[1], !Settings::g_SSAO);
//    }
//
//
//    renderWorldCompose(world);
//
//
////    static Model* model = Loader::loadModel(100, {
////        {3, pos},
////        {2, uv},
////        {3, norm},
////        {3, joint_ids},
////        {3, joint_weights}});
////    AnimRenderer::Inst()->render();
//
//
//    RenderCommand::CheckError("End World Render");
//}


