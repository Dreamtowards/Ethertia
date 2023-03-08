//
// Created by Dreamtowards on 2023/3/8.
//


#include <ethertia/init/ImGuis.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <ethertia/init/Settings.h>
#include <ethertia/Ethertia.h>
#include <ethertia/render/Window.h>


void ImGuis::Init()
{
    BENCHMARK_TIMER;
    Log::info("Init ImGui.. \1");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& imgui_io = ImGui::GetIO();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(Ethertia::getWindow()->m_WindowHandle, true);
    ImGui_ImplOpenGL3_Init("#version 150");  // GL 3.2 + GLSL 150


    ImFontConfig fontConf;
    fontConf.OversampleH = 2;
    fontConf.OversampleV = 2;
    fontConf.GlyphExtraSpacing.x = 1.0f;
//        fontConf.RasterizerMultiply = 2;
    imgui_io.Fonts->AddFontFromFileTTF("./assets/font/menlo.ttf", 14.0f, &fontConf);

//        imgui_io.DisplaySize = ImVec2(1920, 1080);
//        imgui_io.DeltaTime = 1.0f / 60.0f;

    ImGuiStyle& style = ImGui::GetStyle();
    style.GrabMinSize = 7;
    style.FrameBorderSize = 1;
    style.WindowMenuButtonPosition = ImGuiDir_Right;
    style.SeparatorTextBorderSize = 2;
    style.DisplaySafeAreaPadding = {0, 0};
    style.FramePadding = {8, 2};

    style.ScrollbarSize = 10;
    style.ScrollbarRounding = 2;
    style.TabRounding = 2;

    for (int i = 0; i < ImGuiCol_COUNT; ++i)
    {
        ImVec4& col = style.Colors[i];
        float f = Colors::luminance({col.x, col.y, col.z});
        //(col.x + col.y + col.z) / 3.0f;
        col = ImVec4(f,f,f,col.w);
    }


    style.Colors[ImGuiCol_CheckMark] =
    style.Colors[ImGuiCol_SliderGrab] =
            {1.000f, 1.000f, 1.000f, 1.000f};

    style.Colors[ImGuiCol_MenuBarBg] = {0,0,0,0};

    style.Colors[ImGuiCol_HeaderHovered] = {0.051f, 0.431f, 0.992f, 1.000f};
    style.Colors[ImGuiCol_HeaderActive] = {0.071f, 0.388f, 0.853f, 1.000f};

//        style.Colors[ImGuiCol_TitleBg] = {0.297f, 0.297f, 0.298f, 1.000f};
//        style.Colors[ImGuiCol_Button] =
//        style.Colors[ImGuiCol_Header] =
//        style.Colors[ImGuiCol_FrameBg] =
//                {0.322f, 0.322f, 0.322f, 0.540f};
//
//        style.Colors[ImGuiCol_ButtonHovered] =
//        style.Colors[ImGuiCol_HeaderHovered] =
//        style.Colors[ImGuiCol_FrameBgHovered] =
//                {0.626f, 0.626f, 0.626f, 0.400f};
//
//        style.Colors[ImGuiCol_ButtonActive] =
//        style.Colors[ImGuiCol_HeaderActive] =
//                {0.170f, 0.170f, 0.170f, 1.000f};
}

void ImGuis::Destroy()
{

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}






















static void _MenuDebug()
{
    ImGui::SliderFloat("FOV", &RenderEngine::fov, 0, 180);
    ImGui::SliderFloat("ViewDistance", &RenderEngine::viewDistance, 0, 16);
    ImGui::SliderFloat("BrushSize", &Ethertia::getHitCursor().brushSize, 0, 16);
    ImGui::Checkbox("BrushTracking", &Ethertia::getHitCursor().keepTracking);
//    ImGui::Checkbox("BlockMode", &Settings::dbgPolyLine);

//    if (ImGui::Button("Reload Shader")) {
//        ComposeRenderer::initShader();
//    }
//    if (ImGui::Button("Remesh Chunks")) {
//        for (auto it : Ethertia::getWorld()->getLoadedChunks()) {
//            it.second->requestRemodel();
//        }
//    }
//    if (ImGui::Button("Reset Profilers")) {
//        Ethertia::getProfiler().laterClearRootSection();
//
//        ChunkMeshProc::gp_MeshGen.laterClearRootSection();
//        ChunkGenProc::gp_ChunkGen.laterClearRootSection();
//    }

    ImGui::Checkbox("Text Info", &Settings::dbgTextInf);
//    ImGui::Checkbox("ViewBasis", &GuiDebugV::dbgBasis);
//    ImGui::Checkbox("WorldBasis", &GuiDebugV::dbgWorldBasis);
//    ImGui::Checkbox("CursorRangeInfo", &GuiDebugV::dbgCursorRangeInfo);
//    ImGui::Checkbox("Profiler", &GuiDebugV::dbgDrawFrameProfiler);
//
//    ImGui::SeparatorText("Bounding Box");
//    ImGui::Checkbox("LoadedEntityAABB", &GuiDebugV::dbgAllEntityAABB);
//    ImGui::Checkbox("LoadedChunkAABB", &GuiDebugV::dbgChunkAABB);
//    ImGui::Checkbox("NearChunkBound", &GuiDebugV::dbgChunkBoundAABB);
//    ImGui::Checkbox("HitEntityAABB", &GuiDebugV::dbg_CursorPt);


//    ImGui::SeparatorText("Rendering");
//    if (ImGui::MenuItem("GBuffers", nullptr, &GuiDebugV::dbgGBuffers)) {}
//    if (ImGui::MenuItem("Border/Norm", nullptr, &RenderEngine::dbg_EntityGeo)) {}
//    if (ImGui::MenuItem("HitEntityGeo", nullptr, &RenderEngine::dbg_HitPointEntityGeo)) {}
    if (ImGui::MenuItem("Polygon Line", nullptr, &Settings::dbgPolyLine)) {}
//
//    ImGui::Checkbox("NoVegetable", &RenderEngine::dbg_NoVegetable);
//
//    Camera& cam = *Ethertia::getCamera();
//    ImGui::SliderFloat("Cam Smooth", &cam.smoothness, 0, 5);
//    ImGui::SliderFloat("Cam Roll", &cam.eulerAngles.z, -3.14, 3.14);
//
//    ImGui::SliderFloat("FogDensity", &ComposeRenderer::fogDensity, 0, 0.2f);
//    ImGui::SliderFloat("FogGradient",&ComposeRenderer::fogGradient, 0, 5);
//
//
//    ImGui::SeparatorText("etc");
//
//    if (ImGui::MenuItem("ImGui Demo Window", nullptr, &g_ShowImGuiDemoWindow)) {}
//
//    if (ImGui::Button("Click Sound")) {
//        Log::info("PlaySoundClick");
//        Sounds::AS_GUI->UnqueueAllBuffers();
//        Sounds::AS_GUI->QueueBuffer(Sounds::GUI_CLICK->m_BufferId);
//        Sounds::AS_GUI->play();
//    }

}

static void _MenuSystem()
{
//        if (ImGui::MenuItem(Ethertia::Version::name().c_str())) {}
//        if (ImGui::MenuItem("ethertia.com")) {
//            Loader::openURL("https://ethertia.com");
//        }
//        ImGui::Separator();
//        ImGui::MenuItem("Developer / Contributors:", nullptr, nullptr, false);
//        if (ImGui::MenuItem("Eldrine Le Prismarine")) {}
//        if (ImGui::MenuItem("Elytra Corporation")) {}
//        if (ImGui::MenuItem("Thaumstrail")) {}
//
//        ImGui::EndMenu();
//    ImGui::Separator();

    if (ImGui::BeginMenu("Servers"))
    {
        if (ImGui::MenuItem("Connect to server..")) {
        }
        ImGui::Separator();
        ImGui::TextDisabled("Servers:");

        if (ImGui::SmallButton("+")) {
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Add server");

        ImGui::EndMenu();
    }
    ImGui::Separator();

    if (ImGui::BeginMenu("Open World"))
    {
        if (ImGui::MenuItem("New World..")) {
            ImGuis::g_ShowNewWorldWindow = true;
        }
        if (ImGui::MenuItem("Open World..")) {
            const char* filename = Loader::openFolderDialog("Open World..", "./saves/");
            Log::info("Open world: ", filename);

            Ethertia::loadWorld(filename);
        }

        //ImGui::SeparatorText("Saves");
        ImGui::Separator();
        ImGui::TextDisabled("Saves:");

        for (const auto& savedir : std::filesystem::directory_iterator("saves/"))
        {
            std::string time_str = Strings::time_fmt(std::chrono::duration_cast<std::chrono::seconds>(
                    savedir.last_write_time().time_since_epoch()).count());

//            std::string size_str = Strings::size_str(Loader::calcDirectorySize(savedir.path()));
            if (ImGui::MenuItem(savedir.path().filename().c_str(), time_str.c_str()))
            {
                Ethertia::loadWorld(savedir.path());
            }
        }

        ImGui::EndMenu();
    }

    bool worldvalid = Ethertia::getWorld();
    if (ImGui::MenuItem("Edit World..", nullptr, false, worldvalid)) {}
    if (ImGui::MenuItem("Save World", nullptr, false, worldvalid)) {}

    if (ImGui::MenuItem("Close World", nullptr, false, worldvalid)) {
        Ethertia::unloadWorld();
    }

    ImGui::Separator();

    if (ImGui::BeginMenu("Mods")) {
        for (std::string& mod : Settings::MODS) {
            ImGui::MenuItem(mod.c_str());
        }
        ImGui::EndMenu();
    }
//    if (ImGui::BeginMenu("Shaders")) {
//        ImGui::EndMenu();
//    }
    if (ImGui::BeginMenu("ResourcePacks")) {
        ImGui::EndMenu();
    }
    ImGui::Separator();

    if (ImGui::BeginMenu("Settings..")) {

        if (ImGui::MenuItem("Profile")) {}
        if (ImGui::MenuItem("Graphics")) {}
        if (ImGui::MenuItem("Sounds & Music")) {}
        if (ImGui::MenuItem("Controls")) {}
        if (ImGui::MenuItem("Language")) {}

        ImGui::EndMenu();
    }

    if (ImGui::MenuItem("About")) {}

    ImGui::Separator();

    if (ImGui::MenuItem("Terminate")) {
        Ethertia::shutdown();
    }
}


void ShowNewWorldWindow()
{
    ImGui::Begin("New World", &ImGuis::g_ShowNewWorldWindow);

    static char _WorldName[128];
    ImGui::InputText("World Name", _WorldName, 128);

    static char _WorldSeed[128];
    ImGui::InputText("Seed", _WorldSeed, 128);

    if (ImGui::Button("Create"))
    {

    }

    ImGui::End();
}



static void ShowInspector(Entity* entity)
{
    ImGui::Begin("Inspector", &ImGuis::g_ShowInspectorWindow);

//    ImGui::BeginChild();


    ImGui::End();
}

static void ShowEntitiesWindow()
{
    ImGui::Begin("Loaded Entities", &ImGuis::g_ShowLoadedEntitiesWindow);

    World* world = Ethertia::getWorld();
    if (!world) {
        ImGui::TextDisabled("World not loaded.");
        ImGui::End();
        return;
    }
    ImGui::TextDisabled("%i entities loaded.", (int)world->m_Entities.size());


    ImGui::End();
}


void ImGuis::ShowMainMenuBar()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.6));

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("System"))
        {
            _MenuSystem();
            ImGui::EndMenu();
        }
//        if (ImGui::BeginMenu("Edit"))
//        {
//            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
//            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
//            ImGui::Separator();
//            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
//            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
//            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
//            ImGui::EndMenu();
//        }
        if (ImGui::BeginMenu("Debug"))
        {
            _MenuDebug();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Rendering"))
        {
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window"))
        {
            if (ImGui::MenuItem("Inspector", nullptr, &g_ShowInspectorWindow)) {}
            if (ImGui::MenuItem("Entity", nullptr, &g_ShowLoadedEntitiesWindow)) {}
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();




    glPolygonMode(GL_FRONT_AND_BACK, Settings::dbgPolyLine ? GL_LINE : GL_FILL);

    if (g_ShowImGuiDemoWindow)
        ImGui::ShowDemoWindow(&g_ShowImGuiDemoWindow);

    if (g_ShowNewWorldWindow)
        ShowNewWorldWindow();
    if (g_ShowLoadedEntitiesWindow)
        ShowEntitiesWindow();
    if (g_ShowInspectorWindow)
        ShowInspector(g_InspectorEntity);
}


void ImGuis::Render()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    // ImGui::SetCursorScreenPos();
    ImGuis::ShowMainMenuBar();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}