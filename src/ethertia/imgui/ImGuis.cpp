//
// Created by Dreamtowards on 2023/3/8.
//

#define IMGUI_DEFINE_MATH_OPERATORS
#include <ethertia/imgui/ImGuis.h>

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
//#include <ethertia/gui/screen/GuiDebugV.h>

#include <ethertia/init/DebugStat.h>
#include <ethertia/init/Controls.h>

#include <ethertia/entity/player/EntityPlayer.h>
#include <ethertia/world/Chunk.h>
#include <ethertia/render/ssao/SSAORenderer.h>
#include <ethertia/render/shadow/ShadowRenderer.h>
#include <ethertia/render/debug/DebugRenderer.h>
#include <ethertia/render/deferred/GeometryRenderer.h>

void ImGuis::Init()
{
    BENCHMARK_TIMER;
    Log::info("Init ImGui.. \1");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(Ethertia::getWindow().m_WindowHandle, true);
    ImGui_ImplOpenGL3_Init("#version 150");  // GL 3.2 + GLSL 150

    {
        ImFontConfig fontConf;
        fontConf.OversampleH = 3;
        fontConf.OversampleV = 3;
        fontConf.RasterizerMultiply = 1.6f;
        // fontConf.GlyphExtraSpacing.x = 1.0f;
        io.Fonts->AddFontFromFileTTF("./assets/font/menlo.ttf", 14.0f, &fontConf);

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










static bool
        w_ImGuiDemo = false,
        w_NewWorld = false,
        w_Entities = false,
        w_EntityInsp = false,
        w_ShaderProgramInsp = false,
        w_MessageBox = false,
        w_NodeEditor = false,
        w_TitleScreen = false,
        w_Singleplayer = false,
        w_Settings = false,
        w_Game = false;

static bool
        dbg_Text = false,
        dbg_ViewBasis = false,
        dbg_WorldBasis = false,
        dbg_AllEntityAABB = false,
        dbg_AllChunkAABB = false,
        dbg_Gbuffer = false;


static bool g_GizmoViewManipulation = true;

static int g_WorldGrids = 10;

static Entity* g_InspEntity = nullptr;
static ShaderProgram* g_InspShaderProgram = nullptr;


#include "ImDebugs.cpp"



static void _MenuSystem()
{
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
            w_NewWorld = true;
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

static void ShowMainMenuBar()
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
//            ImGui::Checkbox("DbgHitEntityAABB", &GuiDebugV::dbg_CursorPt);
//            ImGui::Checkbox("DbgNearChunkBoundAABB", &GuiDebugV::dbgChunkBoundAABB);
//            ImGui::Checkbox("DbgCursorNearCellsInfo", &GuiDebugV::dbgCursorRangeInfo);

            ImGui::Checkbox("NoChunkSave", &Settings::dbg_NoChunkSave);
            ImGui::Checkbox("NoChunkLoad", &Settings::dbg_NoChunkLoad);

            ImGui::Separator();

            ImGui::SliderInt("World GridSize", &g_WorldGrids, 0, 500);
            ImGui::Checkbox("ViewManipulation Gizmo", &g_GizmoViewManipulation);

            // ImGui::Checkbox("Hit Entity AABB", &gAllChunkAABB);

            ImGui::Checkbox("ImGui Demo Window", &w_ImGuiDemo);

            ImGui::SeparatorText("Controlling");

            ImGui::Checkbox("Hit Tracking", &Ethertia::getHitCursor().keepTracking);
            ImGui::SliderFloat("BrushSize", &Ethertia::getHitCursor().brushSize, 0, 16);
//    ImGui::Checkbox("BlockMode", &Settings::dbgPolyLine);

            Camera& cam = Ethertia::getCamera();
            ImGui::SliderFloat("Cam Smooth", &cam.m_Smoothness, 0, 5);
            ImGui::SliderFloat("Cam Roll", &cam.eulerAngles.z, -3.14, 3.14);

            ImGui::SeparatorText("Rendering");
            ImGui::Checkbox("PauseWorldRender", &Settings::dbg_PauseWorldRender);
            ImGui::Checkbox("GBuffers", &dbg_Gbuffer);
            ImGui::Checkbox("Border/Norm", &Dbg::dbg_EntityGeo);
            ImGui::Checkbox("HitEntityGeo", &Dbg::dbg_HitPointEntityGeo);
            ImGui::Checkbox("Polygon Line", &Settings::dbg_PolyLine);

            ImGui::Checkbox("NoVegetable", &Dbg::dbg_NoVegetable);


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
            ImGui::SliderFloat("FOV", &Ethertia::getCamera().fov, 0, 180);
            ImGui::SliderFloat("ViewDistance", &Settings::s_ViewDistance, 0, 16);

            ImGui::Checkbox("SSAO", &Settings::g_SSAO);
            ImGui::Checkbox("Shadow Mapping", &Settings::g_ShadowMapping);

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            ImGui::Checkbox("Game", &w_Game);
            ImGui::Checkbox("Entities", &w_Entities);
            ImGui::Checkbox("Entity Inspector", &w_EntityInsp);
            ImGui::Checkbox("ShaderProgram Inspector", &w_ShaderProgramInsp);

            ImGui::Separator();
            ImGui::Checkbox("NodeEditor", &w_NodeEditor);
            ImGui::Checkbox("MessageList", &w_MessageBox);

            ImGui::Checkbox("TitleScreen", &w_TitleScreen);
            ImGui::Checkbox("Singleplayer", &w_Singleplayer);
            ImGui::Checkbox("Settings", &w_Settings);

//            ImGui::Checkbox("Profiler", &GuiDebugV::dbgDrawFrameProfiler);

            ImGui::Separator();

            static bool ShowHUD = true;
            if (ImGui::MenuItem("HUD", "F1", &ShowHUD)) {

            }
            if (ImGui::MenuItem("Save Screenshot", "F2")) {
                Controls::saveScreenshot();
            }
            if (ImGui::MenuItem("Fullscreen", "F11", Ethertia::getWindow().isFullscreen())) {
                Ethertia::getWindow().toggleFullscreen();
            }
            if (ImGui::MenuItem("Controlling Game", ".", Ethertia::isIngame())) {
                Ethertia::isIngame() = !Ethertia::isIngame();
            }

            ImGui::EndMenu();
        }


//        using Dbg = DebugStat;
//        if (Dbg::dbg_ChunkProvideState) {
//            ImGui::Text("ChunkProviding[%s]", Dbg::dbg_ChunkProvideState);
//        }
//        if (Dbg::dbg_ChunksSaving) {
//            ImGui::Text("ChunkSaving... (%i)", Dbg::dbg_ChunksSaving);
//        }
//        if (Dbg::dbg_NumChunksMeshInvalid) {
//            ImGui::Text("ChunkMeshing... (%i)", Dbg::dbg_NumChunksMeshInvalid);
//        }

        ImGui::SameLine(ImGui::GetWindowWidth()-90);
        ImGui::SmallButton("Profile");

        ImGui::EndMenuBar();
    }

}


void ShowNewWorldWindow()
{
    ImGui::Begin("New World", &w_NewWorld);

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
        w_NewWorld = false;
    }

    ImGui::End();
}

static void ShowShaderProgramInsp()
{
    ImGui::Begin("ShaderProgram", &w_ShaderProgramInsp);


    ShaderProgram* shader = g_InspShaderProgram;
    if (ImGui::BeginCombo("###Shaders", shader ? shader->m_SourceLocation.c_str() : nullptr))
    {
        for (auto& it : ShaderProgram::REGISTRY)
        {
            if (ImGui::Selectable(it.first.c_str(), g_InspShaderProgram == it.second)) {
                g_InspShaderProgram = it.second;
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
    ImGui::Begin("Entity", &w_EntityInsp);

    Entity* entity = g_InspEntity;
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
            EntityComponentTransform& ct = entity->getComponent<EntityComponentTransform>();

            glm::mat4 matModel = Mth::matModel(entity->getPosition(),
                                               entity->getRotation(),
                                               {1, 1, 1});

            static float bounds[]     = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
            static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };

            ImGuizmo::Manipulate(glm::value_ptr(Ethertia::getCamera().matView),
                                 glm::value_ptr(Ethertia::getCamera().matProjection),
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
            ImGui::Image((void*)(intptr_t)entity->m_DiffuseMap->texId, {64, 64});
        }
    }




    ImGui::End();
}

static void ShowEntities()
{
    ImGui::Begin("Entities", &w_Entities);

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
            g_InspEntity = nullptr;
        }

        ImGui::EndPopup();
    }

    if (_KeepSelectHitEntity) {
        auto& cur = Ethertia::getHitCursor();
        if (cur.hitEntity) {
            g_InspEntity = cur.hitEntity;
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
            if (_ShowOnlyInFrustum && !Ethertia::getCamera().testFrustum(e->getAABB()))
                continue;
            if (_IgnoreChunkProxyEntities && dynamic_cast<EntityMesh*>(e))
                continue;

            char buf[32];

            sprintf(buf, "#%.3i | %s", i, typeid(*e).name());
            if (ImGui::Selectable(buf, g_InspEntity == e)) {
                g_InspEntity = e;
            }
            ++i;
        }
    }


    ImGui::End();
}


void ShowNodeEditor()
{
    ImGui::Begin("NodeEdit", &w_NodeEditor);

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

//ImVec2 operator+(ImVec2 lhs, ImVec2 rhs) {
//    return {lhs.x + rhs.x, lhs.y+rhs.y}
//}

static void ShowSettingsWindow()
{
    ImGui::Begin("Settings", &w_Settings);

    static enum SettingsPanel {
        Profile,
        CurrentWorld,
        Graphics,
        Audio,
        Controls,
        Language,
        Mods,
        Shaders,
        ResourcePacks,
        Credits,
        Misc
    } g_CurrPanel;

    ImGui::BeginChild("SettingNav", {150, 0}, true);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {8, 8});

    if (ImGui::RadioButton("Profile", g_CurrPanel==Profile)) { g_CurrPanel=Profile; }
    if (ImGui::RadioButton("Current World", g_CurrPanel==CurrentWorld)) { g_CurrPanel=CurrentWorld; }
    ImGui::Separator();
    if (ImGui::RadioButton("Graphics", g_CurrPanel==Graphics)) { g_CurrPanel=Graphics; }
    if (ImGui::RadioButton("Music & Sounds", g_CurrPanel==Audio)) { g_CurrPanel=Audio; }
    if (ImGui::RadioButton("Controls", g_CurrPanel==Controls)) { g_CurrPanel=Controls; }
    if (ImGui::RadioButton("Language", g_CurrPanel==Language)) { g_CurrPanel=Language; }
    ImGui::Separator();
    if (ImGui::RadioButton("Mods", g_CurrPanel==Mods)) { g_CurrPanel=Mods; }
    if (ImGui::RadioButton("Shaders", g_CurrPanel==Shaders)) { g_CurrPanel=Shaders; }
    if (ImGui::RadioButton("Resource Packs", g_CurrPanel==ResourcePacks)) { g_CurrPanel=ResourcePacks; }
    ImGui::Separator();
    if (ImGui::RadioButton("Credits", g_CurrPanel==Credits)) { g_CurrPanel=Credits; }
    if (ImGui::RadioButton("Misc", g_CurrPanel==Misc)) { g_CurrPanel=Misc; }

    ImGui::PopStyleVar();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("SettingPanel", {0,0}, true);
    {
        if (g_CurrPanel==Profile)
        {
            ImGui::Dummy({0, 14});
            ImGui::SetWindowFontScale(1.5f);
            ImGui::Text("Dreamtowards");
            ImGui::SetWindowFontScale(0.9f);
            ImGui::TextDisabled("ref.dreamtowards@gmail.com");
            ImGui::SetWindowFontScale(1.0f);
            ImGui::Dummy({0, 8});
            ImGui::Button("Profile*");

            ImGui::Dummy({0, 64});

            ImGui::SeparatorText("Customize Character");
        }
        else if (g_CurrPanel==CurrentWorld)
        {
            static char WorldName[128];
            ImGui::InputText("World Name", WorldName, 128);
        }
        else if (g_CurrPanel==Graphics)
        {
//            ImGui::SeparatorText("Bloom");
//            ImGui::Checkbox("Bloom", &Settings::g_SSAO);

            ImGui::SeparatorText("General");

            ImGui::SliderFloat("FOV", &Ethertia::getCamera().fov, 0, 180);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Field of View.\nNormal: 70\nQuark Pro: 90");

            ImGui::Checkbox("Vsync", &Settings::g_SSAO);

            if (Settings::g_SSAO) ImGui::BeginDisabled();
            ImGui::SliderInt("FPS Limit", &Settings::s_FpsCap, 0, 2000);
            if (Settings::g_SSAO) ImGui::EndDisabled();

            ImGui::SliderFloat("Chunk Load Distance", &Settings::s_ViewDistance, 0, 12);


            ImGui::SeparatorText("Terrain Material Blending");

            ImGui::DragFloat("Texture Scale", &GeometryRenderer::u_MaterialTextureScale, 0.1f);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Material Texture Sampling Scale (inv)");


            ImGui::DragInt("Texture Resolution", &MaterialTextures::TEX_RESOLUTION, 16, 2048);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("[Reload Required] for bake material texture atlas.");




            ImGui::SeparatorText("SSAO");
            ImGui::Checkbox("SSAO", &Settings::g_SSAO);

            ImGui::SeparatorText("Shadow Mapping");
            ImGui::Checkbox("Shadow Mapping", &Settings::g_ShadowMapping);
            static int g_ShadowResolution = 1024;
            ImGui::SliderInt("Shadow Depth Map Resolution", &g_ShadowResolution, 128, 2048);

            ImGui::SeparatorText("Bloom");
            ImGui::Checkbox("Bloom", &Settings::g_SSAO);

        }
        else if (g_CurrPanel==Audio)
        {
            static int g_MasterVolume = 0;
            ImGui::SliderInt("Master Volume", &g_MasterVolume, 0, 100, "%d%%");
            ImGui::Dummy({0, 8});
            ImGui::SliderInt("Music", &g_MasterVolume, 0, 100, "%d%%");
        }
        else if (g_CurrPanel==Controls)
        {
            ImGui::SeparatorText("Mouse");

            static float s_MouseSensitivity = 1.0f;
            ImGui::SliderFloat("Mouse Sensitivity", &s_MouseSensitivity, 0, 2);


            ImGui::SeparatorText("Keyboard");

        }
        else if (g_CurrPanel==Language)
        {
            std::vector<std::pair<const char*, const char*>> g_Languages = {
                    {"en_us", "English"},
                    {"zh_cn", "Jian ti Zhong Wen"}
            };
            static const char* g_SelectedLanguage = "en_us";

            for (auto it : g_Languages) {
                if (ImGui::Selectable(it.second, g_SelectedLanguage==it.first)) {
                    g_SelectedLanguage = it.first;
                }
            }

            ImGui::Text("Translation may not 100%% accurate.");
        }
        else if (g_CurrPanel==Mods)
        {
            static bool s_ModValidatin = true;
            ImGui::Checkbox("Mod Files Hash Online Validation", &s_ModValidatin);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Only enable mods that passed official validation (need online check official database).\n"
                                  "It will check mod file (hash) whether passed official validation"
                                  "since native mods have big permission, at the same time of powerful, "
                                  "it also maybe damage your computer. so we check");
            // 检查Mod文件(hash摘要)是否通过官方验证 (需要联网检查官方数据库)
            // 由于Native-Mod的权限极大，在强大和高效能的同时，也可能会有恶意Mod损害您的电脑.
            // 开启Mod官方验证将会只启用通过官方验证的Mod
        }
        else if (g_CurrPanel==Shaders)
        {

        }
        else if (g_CurrPanel==ResourcePacks)
        {
            if (ImGui::Button("Open ResourcePacks folder")) {
                Loader::openURL("./resourcepacks");
            }
        }
        else if (g_CurrPanel==Credits)
        {
            ImGui::SetWindowFontScale(1.4f);
            ImGui::Text("%s", Ethertia::Version::name().c_str());
            ImGui::SetWindowFontScale(1.0f);

            ImGui::Text("Dev: Eldrine Le Prismarine");

            ImGui::SeparatorText("Links");
            ImGui::SmallButton("ethertia.com");


        }
    }
    ImGui::EndChild();

    ImGui::End();
}

static void ShowSingleplayerWindow()
{
    ImGui::Begin("Singleplayer", &w_Singleplayer);

    if (ImGui::Button("New World")) {

    }
    if (ImGui::Button("Open World")) {

    }
    if (ImGui::Button("Edit World")) {

    }
    if (ImGui::Button("Delete World")) {

    }
    if (ImGui::Button("Refresh")) {

    }

    ImGui::End();
}

static void ShowTitleScreenWindow()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
    ImGui::Begin("TitleScreen", &w_TitleScreen);
    ImGui::PopStyleVar(2);

    // Background
//    ImGui::Image(Texture::DEBUG->texId_ptr(),
//                 ImGuis::GetWindowContentSize(),
//                 {0,1}, {1,0});

    // LeftBottom Version/Stats
    ImGui::SetCursorPosY(ImGui::GetWindowHeight());
    ImGuis::TextAlign(Strings::fmt("0 mods loaded.\n{}", Ethertia::Version::name()).c_str(),
                      {0.0f, 1.0f});

    // RightBottom Copyright
    ImGui::SetCursorPosY(ImGui::GetWindowHeight());
    ImGui::SetCursorPosX(ImGui::GetWindowWidth());
    ImGuis::TextAlign("Copyright (c) Eldrine Le Prismarine, Do not distribute!",
                      {1,1});

    ImVec2 btnSize = {300, 20};
    float btnX = ImGui::GetWindowWidth() / 2 - btnSize.x /2;
    ImGui::SetCursorPosY(ImGui::GetWindowHeight() * 0.4f);

    ImGui::SetCursorPosX(btnX);
    if (ImGui::Button("Singleplayer", btnSize)) {
        w_Singleplayer = true;
    }
    ImGui::SetCursorPosX(btnX);
    if (ImGui::Button("Multiplayer", btnSize)) {
        Loader::showMessageBox("INFO", "Not available");
    }
    ImGui::SetCursorPosX(btnX);
    if (ImGui::Button("Settings", btnSize)) {
        w_Settings = true;
    }
    ImGui::SetCursorPosX(btnX);
    if (ImGui::Button("Terminate", btnSize)) {
        Ethertia::shutdown();
    }


    ImGui::End();
}


static void ShowDockspaceAndMainMenubar()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});

    // Purple 0.373, 0.157, 0.467, Origen0.741, 0.345, 0.133
    glm::vec4 _col = Ethertia::isIngame() ? glm::vec4{0,0,0,0.6} : glm::vec4{0.373, 0.157, 0.467, 1.0};
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(_col.x, _col.y, _col.z, _col.w));

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
                                    | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                                    | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
    ImGui::Begin("MainDockSpaceWindow", nullptr, window_flags);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();

    ImGui::DockSpace(ImGui::GetID("MainDockSpace"), {0, 0}, ImGuiDockNodeFlags_PassthruCentralNode);

    ShowMainMenuBar();

    ImGui::End();
}

static void RenderWindows()
{
    ShowDockspaceAndMainMenubar();


    ImGuizmo::BeginFrame();
    ImGuizmo::SetOrthographic(false);
    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);


    glPolygonMode(GL_FRONT_AND_BACK, Settings::dbg_PolyLine ? GL_LINE : GL_FILL);

    if (w_ImGuiDemo) {
        ImGui::ShowDemoWindow(&w_ImGuiDemo);
    }

    if (w_NewWorld) {
        ShowNewWorldWindow();
    }
    if (w_Entities) {
        ShowEntities();
    }
    if (w_EntityInsp) {
        ShowEntityInsp();
        if (g_InspEntity) {
            RenderEngine::drawLineBox(g_InspEntity->getAABB(), Colors::YELLOW);
        }
    }
    if (w_ShaderProgramInsp) {
        ShowShaderProgramInsp();
    }


    {

        if (g_WorldGrids > 0)
        {
            glm::mat4 iden(1.0f);
            ImGuizmo::DrawGrid(glm::value_ptr(Ethertia::getCamera().matView), glm::value_ptr(Ethertia::getCamera().matProjection),
                               glm::value_ptr(iden), (float)g_WorldGrids);
        }

        if (g_GizmoViewManipulation)
        {
            static float camLen = 10.0f;
            ImGuizmo::ViewManipulate(glm::value_ptr(Ethertia::getCamera().matView), camLen,
                                     ImVec2(ImGui::GetIO().DisplaySize.x-128-24, 20+24), ImVec2(128, 128),
                                     0x10101010);
        }

        if (w_Game)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
            ImGui::Begin("Game", &w_Game);
            ImGui::PopStyleVar();


            ImGuis::Image(ComposeRenderer::fboCompose->texColor[0]->texId, ImGuis::GetWindowContentSize());

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
                if (Ethertia::getCamera().testFrustum(e->getAABB()))
                    RenderEngine::drawLineBox(e->getAABB(), Colors::RED);
            }
        }
        if (dbg_AllChunkAABB) {
            world->forLoadedChunks([&](Chunk* chunk){
                RenderEngine::drawLineBox(chunk->position, glm::vec3{16.0f}, Colors::RED);
            });
        }
    }


    if (w_NodeEditor) {
        ShowNodeEditor();
    }

    if (w_MessageBox)
    {
        ImGui::Begin("MessageBox", &w_MessageBox);
        ImGui::BeginChild("###MsgTextList", {0, -ImGui::GetFrameHeightWithSpacing()});
        for (auto& str : ImGuis::g_MessageBox) {
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

    if (w_TitleScreen) {
        ShowTitleScreenWindow();
    }
    if (w_Singleplayer) {
        ShowSingleplayerWindow();
    }
    if (w_Settings) {
        ShowSettingsWindow();
    }

}


// texId: 0=white
void ImGuis::Image(GLuint texId, ImVec2 size, glm::vec4 color) {
    assert(false);
//    if (texId == 0)
//        texId = GuiRenderer::TEX_WHITE->texId;
//    ImGui::Image((void*)(intptr_t)texId,
//                 {size.x, size.y},
//                 {0, 1}, {1, 0},
//                 {color.x, color.y, color.z, color.w});
}

ImVec2 ImGuis::GetWindowContentSize() {
    return ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin();
}

void ImGuis::TextAlign(const char* text, ImVec2 align) {
    ImVec2 size = ImGui::CalcTextSize(text);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() - align.x * size.x);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - align.y * size.y);
    ImGui::Text("%s", text);
}

void ImGuis::RenderGUI()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    glDisable(GL_DEPTH_TEST);

    RenderWindows();

    glEnable(GL_DEPTH_TEST);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}