//
// Created by Dreamtowards on 2023/3/8.
//


#include <ethertia/init/ImGuis.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glm/gtc/type_ptr.hpp>

#include <ImGuizmo.h>

#include <ethertia/init/Settings.h>
#include <ethertia/Ethertia.h>
#include <ethertia/render/Window.h>
#include <ethertia/render/RenderEngine.h>
#include <ethertia/gui/screen/GuiDebugV.h>


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
    style.Colors[ImGuiCol_Header] = {0.106f, 0.298f, 0.789f, 1.000f};  // also for Selectable.

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
    ImGui::Checkbox("ViewBasis", &GuiDebugV::dbgBasis);
    ImGui::Checkbox("WorldBasis", &GuiDebugV::dbgWorldBasis);
    ImGui::Checkbox("CursorRangeInfo", &GuiDebugV::dbgCursorRangeInfo);
    ImGui::Checkbox("Profiler", &GuiDebugV::dbgDrawFrameProfiler);

    ImGui::SeparatorText("Bounding Box");
    ImGui::Checkbox("LoadedEntityAABB", &GuiDebugV::dbgAllEntityAABB);
    ImGui::Checkbox("LoadedChunkAABB", &GuiDebugV::dbgChunkAABB);
    ImGui::Checkbox("NearChunkBound", &GuiDebugV::dbgChunkBoundAABB);
    ImGui::Checkbox("HitEntityAABB", &GuiDebugV::dbg_CursorPt);


    ImGui::SeparatorText("Rendering");
    if (ImGui::MenuItem("GBuffers", nullptr, &GuiDebugV::dbgGBuffers)) {}
    if (ImGui::MenuItem("Border/Norm", nullptr, &RenderEngine::dbg_EntityGeo)) {}
    if (ImGui::MenuItem("HitEntityGeo", nullptr, &RenderEngine::dbg_HitPointEntityGeo)) {}
    ImGui::Checkbox("Polygon Line", &Settings::dbgPolyLine);
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
    if (ImGui::MenuItem("ImGui Demo Window", nullptr, &ImGuis::g_ShowImGuiDemoWindow)) {}
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
            const char* filename = Loader::openFolderDialog("Open World..", "./saves/");  //std::filesystem::current_path().append("/saves/").string().c_str());
            if (filename) {
                Log::info("Open world: ", filename);
                Ethertia::loadWorld(filename);
            }
        }

        //ImGui::SeparatorText("Saves");
        ImGui::Separator();
        ImGui::TextDisabled("Saves:");

        if (Loader::fileExists("saves/"))
        {
            for (const auto& savedir : std::filesystem::directory_iterator("saves/"))
            {
//            std::string size_str = Strings::size_str(Loader::calcDirectorySize(savedir.path()));

                float epoch = std::chrono::duration_cast<std::chrono::seconds>(savedir.last_write_time().time_since_epoch()).count();
                if (epoch < 0)  epoch = 0;  // Error on Windows.
                std::string time_str = Strings::time_fmt(epoch);

                auto filename = savedir.path().filename();
                if (ImGui::MenuItem((const char*)filename.c_str(), time_str.c_str()))
                {
                    Ethertia::loadWorld(savedir.path().string());
                }
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



static void ShowInspector()
{
    ImGui::Begin("Inspector", &ImGuis::g_ShowInspectorWindow);

    Entity* entity = ImGuis::g_InspectorEntity;
    if (!entity) {
        ImGui::TextDisabled("No entity selected.");
        ImGui::End();
        return;
    }
//    ImGui::BeginChild();

    ImGui::TextDisabled("%i components", (int)entity->m_Components.size());

    if (ImGui::CollapsingHeader("Transform")) {
        ImGui::DragFloat3("Position", entity->pos());
    }

    if (ImGui::CollapsingHeader("Diffuse Map")) {
        if (entity->m_DiffuseMap)
        {
            ImGui::Image((void*)(intptr_t)entity->m_DiffuseMap->texId, {64, 64});
        }
    }



    ImGui::End();
}

static void ShowEntities()
{
    ImGui::Begin("Entities", &ImGuis::g_ShowLoadedEntitiesWindow);

    World* world = Ethertia::getWorld();
    if (!world) {
        ImGui::TextDisabled("World not loaded.");
        ImGui::End();
        return;
    }
    auto& entities = world->m_Entities;

    if (ImGui::Button(" + ")) {

    }

    ImGui::SameLine();

    static bool _ShowOnlyInFrustum = true;
    static bool _SortByDistance = false;
    static int _ListLimit = 100;
    static bool _KeepSelectHitEntity = false;
    static bool _IgnoreChunkProxyEntities = true;
    if (ImGui::Button("..."))
    {
        ImGui::OpenPopup("entities_ops");
    }
    if (ImGui::BeginPopup("entities_ops"))
    {
        ImGui::Checkbox("List only In-Frustum", &_ShowOnlyInFrustum);
        ImGui::Checkbox("Sort by Distance", &_SortByDistance);
        ImGui::Checkbox("Keep Select HitEntity", &_KeepSelectHitEntity);
        ImGui::Checkbox("Ignore chunk proxy entities", &_IgnoreChunkProxyEntities);
        ImGui::SliderInt("List Limit", &_ListLimit, 0, 5000);


        ImGui::TextDisabled("%i rendered / %i loaded.", Settings::dbgEntitiesRendered, (int)entities.size());

        if (ImGui::Button("Unselect")) {
            ImGuis::g_InspectorEntity = nullptr;
        }

        ImGui::EndPopup();
    }

    if (_KeepSelectHitEntity) {
        auto& cur = Ethertia::getHitCursor();
        if (cur.hitEntity) {
            ImGuis::g_InspectorEntity = cur.hitEntity;
        }
    }

//    ImGui::SameLine();
//    if (ImGui::TreeNode(".."))
//    {
//        ImGui::Checkbox("Show only In Frustum", &_ShowOnlyInFrustum);
//        ImGui::Checkbox("Sort by Distance", &_SortByDistance);
//        ImGui::SliderInt("List Limit", &_ListLimit, 0, 5000);
//
//        ImGui::TextDisabled("%i rendered / %i loaded.", Settings::dbgEntitiesRendered, (int)entities.size());
//
//        ImGui::TreePop();
//    }


    ImGui::Separator();

    {
        int i = 0;
        for (Entity* e : entities)
        {
            if (_ListLimit != 0 && i > _ListLimit)
                break;
            if (_ShowOnlyInFrustum && !RenderEngine::testFrustum(e->getAABB()))
                continue;
            if (_IgnoreChunkProxyEntities && dynamic_cast<EntityMesh*>(e))
                continue;

            char buf[32];

            sprintf(buf, "#%.3i | %s", i, typeid(*e).name());
            if (ImGui::Selectable(buf, ImGuis::g_InspectorEntity == e)) {
                ImGuis::g_InspectorEntity = e;
            }
            ++i;
        }
    }


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
        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Entities", nullptr, &g_ShowLoadedEntitiesWindow)) {}
            if (ImGui::MenuItem("Inspector", nullptr, &g_ShowInspectorWindow)) {}

            ImGui::Separator();

            ImGui::SliderInt("World GridSize", &g_WorldGrids, 0, 500);
            ImGui::Checkbox("Gizmo ViewManipulation", &g_GizmoViewManipulation);

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
    if (g_ShowLoadedEntitiesWindow) {
        ImGui::SetNextWindowPos({0, 18});
        ImGui::SetNextWindowSize({320, 330});
        ShowEntities();
    }
    if (g_ShowInspectorWindow) {
        ImGui::SetNextWindowPos({0, 18+330});
        ImGui::SetNextWindowSize({320, 400});

        ShowInspector();
    }
    if (ImGuis::g_InspectorEntity) {
        RenderEngine::drawLineBox(g_InspectorEntity->getAABB(), Colors::YELLOW);
    }



    {
        ImGuizmo::BeginFrame();
        ImGuizmo::SetOrthographic(false);

        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);


        float* pmView = glm::value_ptr(RenderEngine::matView);
        float* pmProj = glm::value_ptr(RenderEngine::matProjection);

        glm::mat4 iden(1.0f);
        float* pmIden = glm::value_ptr(iden);

        if (g_GizmoViewManipulation)
        {
            ImGui::Begin("Gizmo");
            static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
            static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
            if (ImGui::IsKeyPressed(ImGuiKey_G)) mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
            if (ImGui::IsKeyPressed(ImGuiKey_R)) mCurrentGizmoOperation = ImGuizmo::ROTATE;
            if (ImGui::IsKeyPressed(ImGuiKey_S)) mCurrentGizmoOperation = ImGuizmo::SCALE;

            if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
                mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
            ImGui::SameLine();
            if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
                mCurrentGizmoOperation = ImGuizmo::ROTATE;
            ImGui::SameLine();
            if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
                mCurrentGizmoOperation = ImGuizmo::SCALE;

//        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
//        ImGuizmo::DecomposeMatrixToComponents(&RenderEngine::matView[0][0], matrixTranslation, matrixRotation, matrixScale);
//        ImGui::InputFloat3("Tr", matrixTranslation);
//        ImGui::InputFloat3("Rt", matrixRotation);
//        ImGui::InputFloat3("Sc", matrixScale);
//        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &RenderEngine::matView[0][0]);

            if (mCurrentGizmoOperation != ImGuizmo::SCALE)
            {
                if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                    mCurrentGizmoMode = ImGuizmo::LOCAL;
                ImGui::SameLine();
                if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                    mCurrentGizmoMode = ImGuizmo::WORLD;
            }

            {
                static glm::mat4 matCube(1.0f);

                ImGuizmo::DrawCubes(pmView, pmProj, &matCube[0][0], 1);


                static float bounds[]     = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
                static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
                ImGuizmo::Manipulate(pmView, pmProj,
                                     mCurrentGizmoOperation, mCurrentGizmoMode,
                                     &matCube[0][0], nullptr, nullptr, bounds, boundsSnap);
            }
            ImGui::End();
        }

        if (g_WorldGrids > 0)
        {
            ImGuizmo::DrawGrid(pmView, pmProj, pmIden, g_WorldGrids);
        }

        if (g_GizmoViewManipulation)
        {
            static float camLen = 10.0f;
            ImGuizmo::ViewManipulate(pmView, camLen,
                                     ImVec2(io.DisplaySize.x-24-128, 20+24), ImVec2(128, 128),
                                     0x10101010);
        }
    }
}


void ImGuis::Render()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    // ImGui::SetCursorScreenPos();

    glDisable(GL_DEPTH_TEST);
    ImGuis::ShowMainMenuBar();
    glEnable(GL_DEPTH_TEST);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}