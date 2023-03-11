//
// Created by Dreamtowards on 2023/3/8.
//


#include <ethertia/init/ImGuis.h>

#include <glm/gtc/type_ptr.hpp>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <imguizmo/ImGuizmo.h>
#include <imgui-knobs/imgui-knobs.h>
#include <imgui-imnodes/imnodes.h>


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
    ImGuiIO& io = ImGui::GetIO();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(Ethertia::getWindow()->m_WindowHandle, true);
    ImGui_ImplOpenGL3_Init("#version 150");  // GL 3.2 + GLSL 150

    {

        ImFontConfig fontConf;
        fontConf.OversampleH = 3;
        fontConf.OversampleV = 3;
        fontConf.RasterizerMultiply = 1.6f;
        // fontConf.GlyphExtraSpacing.x = 1.0f;
        io.Fonts->AddFontFromFileTTF("./assets/font/menlo.ttf", 14.0f, &fontConf);


//        imgui_io.DisplaySize = ImVec2(1920, 1080);
//        imgui_io.DeltaTime = 1.0f / 60.0f;

        // Enable Docking.
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

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
            float f = std::max(Colors::luminance({col.x, col.y, col.z}), 0.06f);
            //(col.x + col.y + col.z) / 3.0f;
            col = ImVec4(f,f,f,col.w);
        }


        style.Colors[ImGuiCol_CheckMark] =
        style.Colors[ImGuiCol_SliderGrab] =
                {1.000f, 1.000f, 1.000f, 1.000f};

//    style.Colors[ImGuiCol_MenuBarBg] = {0,0,0,0};

        style.Colors[ImGuiCol_HeaderHovered] = {0.051f, 0.431f, 0.992f, 1.000f};
        style.Colors[ImGuiCol_HeaderActive] = {0.071f, 0.388f, 0.853f, 1.000f};
        style.Colors[ImGuiCol_Header] = {0.106f, 0.298f, 0.789f, 1.000f};  // also for Selectable.

        style.Colors[ImGuiCol_TitleBg] = {0.082f, 0.082f, 0.082f, 0.800f};
        style.Colors[ImGuiCol_TitleBgActive] = {0.082f, 0.082f, 0.082f, 1.000f};


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

    ImNodes::CreateContext();
    ImNodes::GetIO().EmulateThreeButtonMouse.Modifier = &io.KeyShift;

}

void ImGuis::Destroy()
{
    ImNodes::DestroyContext();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}














static void ShowDebugTextOverlay()
{
    World* world = Ethertia::getWorld();
    float dt = Ethertia::getDelta();
    EntityPlayer* player = Ethertia::getPlayer();
    btRigidBody* playerRb = player->m_Rigidbody;
    float meterPerSec = Mth::floor_dn(playerRb->getLinearVelocity().length(), 3);

    std::string cellInfo = "nil";
    std::string chunkInfo = "nil";
    std::string worldInfo = "nil";
    HitCursor& cur = Ethertia::getHitCursor();


    if (world)
    {
        worldInfo = Strings::fmt("{}. inhabited {}s, daytime {}. seed {}",
                                 world->m_WorldInfo.Name,
                                 world->m_WorldInfo.InhabitedTime,
                                 Strings::daytime(world->getDayTime()),
                                 world->getSeed());
        Chunk* hitChunk = world->getLoadedChunk(cur.position);
        if (hitChunk) {
            chunkInfo = Strings::fmt("GenPop: {}, Inhabited: {}s",
                                     hitChunk->m_Populated,
                                     hitChunk->m_InhabitedTime);
        }
        if (cur.cell) {
            Cell* c = cur.cell;
            cellInfo = Strings::fmt("mtl: {}, dens: {}, meta: {} | DiggingTime: {}",
                                    c->mtl ? c->mtl->getRegistryId() : "nil",
                                    c->density,
                                    c->exp_meta,
                                    cur.cell_breaking_time);
        }
    }

    std::string str = Strings::fmt(
            "cam p: {}, len: {}, spd {}mps {}kph. ground: {}, CPs {}.\n"
            "Entity: {}/{}, LoadedChunks: {}\n"
            "MeshInvalid Chunks: {}\n"
            "task {}, async {}\n"
            "dt: {}, {}fps\n"
            "World: {}\n"
            "HitChunk: {}\n"
            "HitCell: {}\n",
            Ethertia::getCamera()->position, Ethertia::getCamera()->len,
            meterPerSec, meterPerSec * 3.6f,
            player->m_OnGround, player->m_NumContactPoints,

            Settings::dbgEntitiesRendered, world ? world->getEntities().size() : 0, world ? world->getLoadedChunks().size() : 0,

            ChunkMeshProc::dbg_NumChunksMeshInvalid,
            Ethertia::getScheduler()->numTasks(), Ethertia::getAsyncScheduler()->numTasks(),
            dt, Mth::floor(1.0f/dt),
            worldInfo,
            chunkInfo,
            cellInfo);

    ImGui::SetNextWindowPos({0, 32});
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (ImGui::Begin("DebugTextOverlay", &ImGuis::dbg_Text, window_flags)) {
        ImGui::Text("%s", str.c_str());
    }
    ImGui::End();
    ImGui::PopStyleVar();
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
            ImGuis::g_ShowNewWorld = true;
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
    if (ImGui::BeginMenu("Resource Packs")) {
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

    if (ImGui::MenuItem("About", Ethertia::Version::version_name().c_str())) {}

    ImGui::Separator();

    if (ImGui::MenuItem("Terminate")) {
        Ethertia::shutdown();
    }
}

void ImGuis::ShowMainMenuBar()
{
    if (ImGui::BeginMenuBar())
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

            ImGui::Checkbox("DbgText", &dbg_Text);
            ImGui::Checkbox("DbgWorldBasis", &dbg_WorldBasis);
            ImGui::Checkbox("DbgViewBasis", &dbg_ViewBasis);

            ImGui::Checkbox("DbgAllEntityAABB", &dbg_AllEntityAABB);
            ImGui::Checkbox("DbgAllChunkAABB", &dbg_AllChunkAABB);
            ImGui::Checkbox("DbgHitEntityAABB", &GuiDebugV::dbg_CursorPt);
            ImGui::Checkbox("DbgNearChunkBoundAABB", &GuiDebugV::dbgChunkBoundAABB);

            ImGui::Checkbox("DbgCursorNearCellsInfo", &GuiDebugV::dbgCursorRangeInfo);

            ImGui::Checkbox("NoChunkSave", &Settings::dbg_NoChunkSave);
            ImGui::Checkbox("NoChunkLoad", &Settings::dbg_NoChunkLoad);

            ImGui::Separator();

            ImGui::SliderInt("World GridSize", &g_WorldGrids, 0, 500);
            ImGui::Checkbox("ViewManipulation Gizmo", &g_GizmoViewManipulation);

            // ImGui::Checkbox("Hit Entity AABB", &gAllChunkAABB);

            ImGui::Checkbox("ImGui Demo Window", &ImGuis::g_ShowImGuiDemo);

            ImGui::SeparatorText("Controlling");

            ImGui::Checkbox("Hit Tracking", &Ethertia::getHitCursor().keepTracking);
            ImGui::SliderFloat("BrushSize", &Ethertia::getHitCursor().brushSize, 0, 16);
//    ImGui::Checkbox("BlockMode", &Settings::dbgPolyLine);

            Camera& cam = *Ethertia::getCamera();
            ImGui::SliderFloat("Cam Smooth", &cam.m_Smoothness, 0, 5);
            ImGui::SliderFloat("Cam Roll", &cam.eulerAngles.z, -3.14, 3.14);

            ImGui::SeparatorText("Rendering");
            ImGui::Checkbox("PauseWorldRender", &Settings::dbg_PauseWorldRender);
            ImGui::Checkbox("GBuffers", &dbg_Gbuffer);
            ImGui::Checkbox("Border/Norm", &RenderEngine::dbg_EntityGeo);
            ImGui::Checkbox("HitEntityGeo", &RenderEngine::dbg_HitPointEntityGeo);
            ImGui::Checkbox("Polygon Line", &Settings::dbg_PolyLine);

            ImGui::Checkbox("NoVegetable", &RenderEngine::dbg_NoVegetable);


            static float knobVal = 10;
            ImGuiKnobs::Knob("Test2", &knobVal, -100, 100, 0.1f, "%.1fdB", ImGuiKnobVariant_Tick);

//    if (ImGui::Button("Click Sound")) {
//        Log::info("PlaySoundClick");
//        Sounds::AS_GUI->UnqueueAllBuffers();
//        Sounds::AS_GUI->QueueBuffer(Sounds::GUI_CLICK->m_BufferId);
//        Sounds::AS_GUI->play();
//    }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Rendering"))
        {
            ImGui::SliderFloat("FOV", &Ethertia::getCamera()->fov, 0, 180);
            ImGui::SliderFloat("ViewDistance", &RenderEngine::viewDistance, 0, 16);

            ImGui::Checkbox("SSAO", &Settings::g_SSAO);
            ImGui::Checkbox("Shadow Mapping", &Settings::g_ShadowMapping);

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            ImGui::Checkbox("Game", &g_Game);
            ImGui::Checkbox("Entities", &g_ShowLoadedEntities);
            ImGui::Checkbox("Entity Inspector", &g_ShowEntityInsp);
            ImGui::Checkbox("ShaderProgram Inspector", &g_ShowShaderProgramInsp);

            ImGui::Separator();
            ImGui::Checkbox("NodeEditor", &g_ShowNodeEditor);
            ImGui::Checkbox("MessageList", &g_ShowMessageBox);

            ImGui::Checkbox("Profiler", &GuiDebugV::dbgDrawFrameProfiler);

            ImGui::EndMenu();
        }

        ImGui::SameLine(ImGui::GetWindowWidth() - 90);
        ImGui::SmallButton("Profile");

        ImGui::EndMenuBar();
    }

}


void ShowNewWorldWindow()
{
    ImGui::Begin("New World", &ImGuis::g_ShowNewWorld);

    static char _WorldName[128];
    ImGui::InputText("World Name", _WorldName, 128);
    std::string save_path = Strings::fmt("./saves/{}", _WorldName);
    ImGui::TextDisabled("Will save as: %s", save_path.c_str());

    static char _WorldSeed[128];
    ImGui::InputText("Seed", _WorldSeed, 128);
    uint64_t seed = WorldInfo::ofSeed(_WorldSeed);
    ImGui::TextDisabled("Actual u64 seed: %llu", seed);


    if (ImGui::Button("Create"))
    {
        WorldInfo worldinfo{
                .Seed = seed,
                .Name = _WorldName
        };
        Log::info("Creating world '{}' seed {}.", worldinfo.Name, worldinfo.Seed);
        Ethertia::loadWorld(save_path, &worldinfo);
        ImGuis::g_ShowNewWorld = false;
    }

    ImGui::End();
}

static void ShowShaderProgramInsp()
{
    ImGui::Begin("ShaderProgram", &ImGuis::g_ShowShaderProgramInsp);


    ShaderProgram* shader = ImGuis::g_InspShaderProgram;
    if (ImGui::BeginCombo("###Shaders", shader ? shader->m_SourceLocation.c_str() : nullptr))
    {
        for (auto& it : ShaderProgram::REGISTRY)
        {
            if (ImGui::Selectable(it.first.c_str(), ImGuis::g_InspShaderProgram == it.second)) {
                ImGuis::g_InspShaderProgram = it.second;
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();
    ImGui::Button("+");

    ImGui::Separator();

    if (!shader) {
        ImGui::TextDisabled("No ShaderProgram selected.");
        ImGui::End();
        return;
    }

    if (ImGui::Button("Reload Shader")) {
        shader->reloadSources_();
        Log::info("Shader {} reloaded.", shader->m_SourceLocation);
    }
    ImGui::SameLine();
    ImGui::Text("#%i",(int)shader->m_ProgramId);

    ImGui::TextDisabled("%i Uniforms:", (int)shader->m_Uniforms.size());
    for (auto& it : shader->m_Uniforms)
    {
        auto& unif = it.second;
        const char* name = it.first;
        if (!unif.value_ptr) {
            ImGui::TextDisabled("uniform %s has no lvalue", name);
            continue;
        }
        switch (unif.type)
        {
            case ShaderProgram::Uniform::INT:
                ImGui::DragInt(name, (int*)unif.value_ptr, -100, 100);
                break;
            case ShaderProgram::Uniform::FLOAT:
                ImGui::DragFloat(name, (float*)unif.value_ptr, -100, 100);
                break;
            case ShaderProgram::Uniform::VEC3:
                ImGui::DragFloat3(name, (float*)unif.value_ptr, -100, 100);
                break;
            case ShaderProgram::Uniform::VEC4:
                ImGui::DragFloat4(name, (float*)unif.value_ptr, -100, 100);
                break;
            case ShaderProgram::Uniform::MAT3:
                ImGui::Text("Mat3");
                break;
            case ShaderProgram::Uniform::MAT4:
                ImGui::Text("Mat4");
                break;
            default:
                ImGui::Text("Unknown Uniform Type");
                break;
        }
    }

    ImGui::End();
}

static void ShowEntityInsp()
{
    ImGui::Begin("Entity", &ImGuis::g_ShowEntityInsp);

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
        ImGui::DragFloat3("Rotation", entity->pos());
        ImGui::DragFloat3("Scale", entity->pos());

        ImGui::Separator();
        ImGui::TextDisabled("Gizmo:");

        static ImGuizmo::OPERATION gizmoOp   = ImGuizmo::ROTATE;
        static ImGuizmo::MODE      gizmoMode = ImGuizmo::WORLD;
        if (ImGui::IsKeyPressed(ImGuiKey_T)) gizmoOp = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(ImGuiKey_R)) gizmoOp = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(ImGuiKey_S)) gizmoOp = ImGuizmo::SCALE;

        if (ImGui::RadioButton("Translate", gizmoOp == ImGuizmo::TRANSLATE)) gizmoOp = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", gizmoOp == ImGuizmo::ROTATE)) gizmoOp = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", gizmoOp == ImGuizmo::SCALE)) gizmoOp = ImGuizmo::SCALE;

//        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
//        ImGuizmo::DecomposeMatrixToComponents(&RenderEngine::matView[0][0], matrixTranslation, matrixRotation, matrixScale);
//        ImGui::InputFloat3("Tr", matrixTranslation);
//        ImGui::InputFloat3("Rt", matrixRotation);
//        ImGui::InputFloat3("Sc", matrixScale);
//        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &RenderEngine::matView[0][0]);

        if (gizmoOp != ImGuizmo::SCALE)
        {
            if (ImGui::RadioButton("Local", gizmoMode == ImGuizmo::LOCAL)) gizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", gizmoMode == ImGuizmo::WORLD)) gizmoMode = ImGuizmo::WORLD;
        }

        static bool _Snap = false;
        bool snap = _Snap || ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftSuper);
        if (ImGui::Checkbox("Snap", &snap)) {
            _Snap = snap;
        }

        glm::vec3 snapValue{0.5};
        ImGui::DragFloat3("Snap value", &snapValue[0], 0.5f);

        static bool _Bound = false;
        ImGui::Checkbox("Bound", &_Bound);

        glm::vec3 boundSnapValue{0.5f};
        ImGui::DragFloat3("Bound Snap value", &snapValue[0], 0.5f);


        {
            EntityComponentTransform* ct = entity->getComponent<EntityComponentTransform>();

            glm::mat4 matModel = Mth::matModel(entity->getPosition(),
                                               entity->getRotation(),
                                               {1, 1, 1});

            static float bounds[]     = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
            static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };

            ImGuizmo::Manipulate(glm::value_ptr(Ethertia::getCamera()->matView),
                                 glm::value_ptr(Ethertia::getCamera()->matProjection),
                                 gizmoOp, gizmoMode,
                                 glm::value_ptr(matModel),
                                 nullptr,
                                 snap ? &snapValue[0] : nullptr,
                                 _Bound ? bounds : nullptr, _Bound ? boundsSnap : nullptr);
            if (ImGuizmo::IsUsing()) {
                // Decompose Transformation

                glm::vec3 pos, scl;
                glm::mat3 rot;
                Mth::decomposeTransform(matModel, pos, rot, scl);

                entity->setPosition(pos);
                *(glm::mat3*)entity->rot() = rot;
            }
        }
    }

    if (ImGui::CollapsingHeader("Diffuse Map")) {
        if (entity->m_DiffuseMap)
        {
            ImGui::Image(entity->m_DiffuseMap->texId_ptr(), {64, 64});
        }
    }




    ImGui::End();
}

static void ShowEntities()
{
    ImGui::Begin("Entities", &ImGuis::g_ShowLoadedEntities);

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


void ShowNodeEditor()
{
    ImGui::Begin("NodeEdit", &ImGuis::g_ShowNodeEditor);

    // id of Attrib/Pin
    static std::vector<std::pair<int, int>> g_Links;

    ImNodes::BeginNodeEditor();

    for (int i = 0; i < 4; ++i) {
        ImNodes::BeginNode(i);

        ImNodes::BeginNodeTitleBar();
        ImGui::Text("Title");
        ImNodes::EndNodeTitleBar();

        ImNodes::BeginInputAttribute(i*5+1);

        ImGui::Text("Input3");

        ImNodes::EndInputAttribute();

        ImNodes::BeginOutputAttribute(i*5+2, ImNodesPinShape_Triangle);
        static bool bl;
        ImGui::Checkbox("SthCheck", &bl);
        ImNodes::EndOutputAttribute();

        ImNodes::BeginStaticAttribute(i*5+3);
        ImGui::Text("Static");
        ImNodes::EndStaticAttribute();

        ImNodes::EndNode();
    }

    {
        int i = 0;
        for (auto& lk : g_Links) {
            ImNodes::Link(++i, lk.first, lk.second);
        }
    }
    ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomLeft);  // call before ImNodes::EndNodeEditor();
    ImNodes::EndNodeEditor();

    {
        int attr_beg, attr_end;
        if (ImNodes::IsLinkCreated(&attr_beg, &attr_end)) {  // call after ImNodes::EndNodeEditor();
            g_Links.emplace_back(attr_beg, attr_end);
        }
    }


    ImGui::End();
}

void Dbg_DrawGbuffers(float x, float y) {
    auto* gbuffer = RenderEngine::fboGbuffer;

    float h = Gui::maxHeight() / 6;
    float w = h * 1.5f;

    Gui::drawRect(x, y, w, h, {
            .tex = gbuffer->texColor[0],
            .channel_mode = Gui::DrawRectArgs::C_RGB
    });
    Gui::drawString(x,y, "Pos");

    Gui::drawRect(x+w, y, w, h, {
            .tex = gbuffer->texColor[0],
            .channel_mode = Gui::DrawRectArgs::C_AAA
    });
    Gui::drawString(x+w,y, "Dep");

    Gui::drawRect(x, y+h, w, h, gbuffer->texColor[1]);
    Gui::drawString(0,h, "Norm");

    Gui::drawRect(x, y+h*2, w, h, {
            .tex = gbuffer->texColor[2],
            .channel_mode = Gui::DrawRectArgs::C_RGB
    });
    Gui::drawString(x,y+h*2, "Albedo");

    Gui::drawRect(x+w, y+h*2, w, h, {
            .tex = gbuffer->texColor[2],
            .channel_mode = Gui::DrawRectArgs::C_AAA
    });
    Gui::drawString(x+w,y+h*2, "Roug");

    Gui::drawRect(x, y+h*3, w, h, {
            .tex = SSAORenderer::fboSSAO->texColor[0],
            .channel_mode = Gui::DrawRectArgs::C_RGB
    });
    Gui::drawString(x,y+h*3, "SSAO");


    Gui::drawRect(x, y+h*4, w, h, {
            .tex = ShadowRenderer::fboDepthMap->texDepth,
            .channel_mode = Gui::DrawRectArgs::C_RGB
    });
    Gui::drawString(x,y+h*4, "Shadow");

//            Gui::drawRect(x+w, y+h*4, w, h, {
//                    .tex = ShadowRenderer::fboDepthMap->texColor[0],
//                    .channel_mode = Gui::DrawRectArgs::C_RGB
//            });
//            Gui::drawString(x+w,y+h*4, "Shadow Col");
}

void ImGuis::InnerRender()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});

    // Purple 0.373, 0.157, 0.467, Origen0.741, 0.345, 0.133
    glm::vec4 _col = Settings::ForceNotIngame ? glm::vec4{0.373, 0.157, 0.467, 1.0} : glm::vec4{0,0,0,0.6};
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(_col.x, _col.y, _col.z, _col.w));

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
                                    | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                                    | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
    ImGui::Begin("MainDockSpaceWindow", nullptr, window_flags);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();

    ImGui::DockSpace(ImGui::GetID("MainDockSpace"), {0, 0}, ImGuiDockNodeFlags_PassthruCentralNode);

    ImGuis::ShowMainMenuBar();

    ImGui::End();



    ImGuizmo::BeginFrame();
    ImGuizmo::SetOrthographic(false);
    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);


    glPolygonMode(GL_FRONT_AND_BACK, Settings::dbg_PolyLine ? GL_LINE : GL_FILL);

    if (g_ShowImGuiDemo) {
        ImGui::ShowDemoWindow(&g_ShowImGuiDemo);
    }

    if (g_ShowNewWorld) {
        ShowNewWorldWindow();
    }
    if (g_ShowLoadedEntities) {
        ShowEntities();
    }
    if (g_ShowEntityInsp) {
        ShowEntityInsp();
        if (g_InspectorEntity) {
            RenderEngine::drawLineBox(ImGuis::g_InspectorEntity->getAABB(), Colors::YELLOW);
        }
    }
    if (g_ShowShaderProgramInsp) {
        ShowShaderProgramInsp();
    }


    {

        if (g_WorldGrids > 0)
        {
            glm::mat4 iden(1.0f);
            ImGuizmo::DrawGrid(glm::value_ptr(Ethertia::getCamera()->matView), glm::value_ptr(Ethertia::getCamera()->matProjection),
                               glm::value_ptr(iden), (float)ImGuis::g_WorldGrids);
        }

        if (g_GizmoViewManipulation)
        {
            static float camLen = 10.0f;
            ImGuizmo::ViewManipulate(glm::value_ptr(Ethertia::getCamera()->matView), camLen,
                                     ImVec2(ImGui::GetIO().DisplaySize.x-128-24, 20+24), ImVec2(128, 128),
                                     0x10101010);
        }

        if (g_Game)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
            ImGui::Begin("Game", &ImGuis::g_Game);
            ImGui::PopStyleVar();

            ImVec2 wmx = ImGui::GetWindowContentRegionMax();
            ImVec2 wmn = ImGui::GetWindowContentRegionMin();
            ImGui::Image(ComposeRenderer::fboCompose->texColor[0]->texId_ptr(),
                         {wmx.x - wmn.x, wmx.y - wmn.y},
                         {0,1}, {1,0});

            ImGui::End();
        }
    }

    if (dbg_Text) {
        ShowDebugTextOverlay();
    }
    if (dbg_ViewBasis) {
        DebugRenderer::Inst().renderDebugBasis();
    }
    if (dbg_WorldBasis) {
        DebugRenderer::Inst().renderDebugWorldBasis();
    }
    if (dbg_Gbuffer) {
        Dbg_DrawGbuffers(0, 32);
    }

    World* world = Ethertia::getWorld();
    if (world) {
        if (dbg_AllEntityAABB) {
            for (Entity* e : world->m_Entities) {
                if (RenderEngine::testFrustum(e->getAABB()))
                    RenderEngine::drawLineBox(e->getAABB(), Colors::RED);
            }
        }
        if (dbg_AllChunkAABB) {
            world->forLoadedChunks([&](Chunk* chunk){
                RenderEngine::drawLineBox(chunk->position, glm::vec3{16.0f}, Colors::RED);
            });
        }
    }


    if (g_ShowNodeEditor) {
        ShowNodeEditor();
    }

    if (g_ShowMessageBox)
    {
        ImGui::Begin("MessageBox", &g_ShowMessageBox);
        ImGui::BeginChild("###MsgTextList", {0, -ImGui::GetFrameHeightWithSpacing()});
        for (auto& str : g_MessageBox) {
            ImGui::Text("%s", str.c_str());
        }
        ImGui::EndChild();
        static char InputBuf[128];

        if (ImGui::InputText("###MsgBoxInput", InputBuf, 128,
                             ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CtrlEnterForNewLine))
        {
            if (InputBuf[0]) {
                Ethertia::dispatchCommand(InputBuf);
                InputBuf[0] = 0;  // clear.
            }
        }
        // keeping auto focus on the input box
        if (ImGui::IsItemHovered() || (ImGui::IsWindowFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

        ImGui::End();
    }


}

void ImGuis::Render()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    glDisable(GL_DEPTH_TEST);

    ImGuis::InnerRender();

    glEnable(GL_DEPTH_TEST);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}