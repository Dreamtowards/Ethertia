//
// Created by Dreamtowards on 2023/3/12.
//


//#include <ethertia/gui/Gui.h>
//
//void Dbg_DrawGbuffers(float x, float y)
//{
//    auto* gbuffer = GeometryRenderer::fboGbuffer;
//
//    float h = 120;
//    float w = h * 1.5f;
//
//    Gui::drawRect(x, y, w, h, {
//            .tex = gbuffer->texColor[0],
//            .channel_mode = Gui::DrawRectArgs::C_RGB
//    });
//    Gui::drawString(x,y, "Pos");
//
//    Gui::drawRect(x+w, y, w, h, {
//            .tex = gbuffer->texColor[0],
//            .channel_mode = Gui::DrawRectArgs::C_AAA
//    });
//    Gui::drawString(x+w,y, "Dep");
//
//    Gui::drawRect(x, y+h, w, h, gbuffer->texColor[1]);
//    Gui::drawString(0,h, "Norm");
//
//    Gui::drawRect(x, y+h*2, w, h, {
//            .tex = gbuffer->texColor[2],
//            .channel_mode = Gui::DrawRectArgs::C_RGB
//    });
//    Gui::drawString(x,y+h*2, "Albedo");
//
//    Gui::drawRect(x+w, y+h*2, w, h, {
//            .tex = gbuffer->texColor[2],
//            .channel_mode = Gui::DrawRectArgs::C_AAA
//    });
//    Gui::drawString(x+w,y+h*2, "Roug");
//
//    Gui::drawRect(x, y+h*3, w, h, {
//            .tex = SSAORenderer::fboSSAO->texColor[0],
//            .channel_mode = Gui::DrawRectArgs::C_RGB
//    });
//    Gui::drawString(x,y+h*3, "SSAO");
//
//
//    Gui::drawRect(x, y+h*4, w, h, {
//            .tex = ShadowRenderer::fboDepthMap->texDepth,
//            .channel_mode = Gui::DrawRectArgs::C_RGB
//    });
//    Gui::drawString(x,y+h*4, "Shadow");
//
//}






static enum SettingsPanel {
    Profile,
    CurrentWorld,
    Graphics,
    Audio,
    Controls,
    Language,
    Mods,
    ResourcePacks,
    Credits,
    Misc
} g_CurrSettingsPanel = SettingsPanel::Graphics;








// w_: Window
static bool
        w_ImGuiDemo = false,
        w_NewWorld = false,
        w_WorldGenNodeEditor = false,
        w_NodeEditor = false,
        w_TitleScreen = false,
        w_Singleplayer = false;

static bool
        dbg_AllEntityAABB = false,
        dbg_AllChunkAABB = false,
        dbg_Gbuffer = false;

static ShaderProgram* g_InspShaderProgram = nullptr;



static Texture* LazyLoadTex(const std::string& p) {
    static std::map<std::string, Texture*> _Cache;
    auto& it = _Cache[p];
    if (!it) {
        it = Loader::loadTexture(p);
    }
    return it;
}






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
    if (ImGui::MenuItem("Edit World..", nullptr, false, worldvalid))
    {
        Settings::w_Settings = true;
        g_CurrSettingsPanel = SettingsPanel::CurrentWorld;
    }
    if (ImGui::MenuItem("Save World", nullptr, false, worldvalid)) {}

    if (ImGui::MenuItem("Close World", nullptr, false, worldvalid)) {
        Ethertia::unloadWorld();
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Settings.."))
    {
        Settings::w_Settings = true;
    }

    if (ImGui::MenuItem("Mods", "0 mods loaded")) {
        Settings::w_Settings = true;
        g_CurrSettingsPanel = SettingsPanel::Mods;
    }
    if (ImGui::MenuItem("Resource Packs")) {
        Settings::w_Settings = true;
        g_CurrSettingsPanel = SettingsPanel::ResourcePacks;
    }
    if (ImGui::MenuItem("Controls")) {
        Settings::w_Settings = true;
        g_CurrSettingsPanel = SettingsPanel::Controls;
    }



    if (ImGui::MenuItem("About")) {
        Settings::w_Settings = true;
        g_CurrSettingsPanel = SettingsPanel::Credits;
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Terminate")) {
        Ethertia::shutdown();
    }
}

#include <ethertia/init/KeyBinding.h>

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
        if (ImGui::BeginMenu("World"))
        {

            ImGui::Checkbox("AllEntityAABB", &dbg_AllEntityAABB);
            ImGui::Checkbox("AllChunkAABB", &dbg_AllChunkAABB);
//            ImGui::Checkbox("DbgHitEntityAABB", &GuiDebugV::dbg_CursorPt);
//            ImGui::Checkbox("DbgNearChunkBoundAABB", &GuiDebugV::dbgChunkBoundAABB);
//            ImGui::Checkbox("DbgCursorNearCellsInfo", &GuiDebugV::dbgCursorRangeInfo);

            ImGui::Checkbox("MeshingChunks AABB", &Dbg::dbg_MeshingChunksAABB);
            ImGui::Checkbox("Chunk Mesh Counter", &Dbg::dbg_ChunkMeshedCounter);

            ImGui::Checkbox("NoChunkSave", &Dbg::dbg_NoChunkSave);
            ImGui::Checkbox("NoChunkLoad", &Dbg::dbg_NoChunkLoad);

            ImGui::Checkbox("PauseThread ChunkMeshing", &Dbg::dbg_PauseThread_ChunkMeshing);
            ImGui::Checkbox("PauseThread ChunkLoad/Gen/Save", &Dbg::dbg_PauseThread_ChunkLoadGenSave);

            if (ImGui::Button("Remesh All Chunks")) {
                for (auto it : Ethertia::getWorld()->getLoadedChunks()) {
                    it.second->requestRemodel();
                }
            }

            if (ImGui::BeginMenu("Gamemode"))
            {
                EntityPlayer& p = *Ethertia::getPlayer();
                int gm = Ethertia::getPlayer()->getGamemode();

                if (ImGui::MenuItem("Survival",  nullptr, gm==Gamemode::SURVIVAL))  p.switchGamemode(Gamemode::SURVIVAL);
                if (ImGui::MenuItem("Creative",  nullptr, gm==Gamemode::CREATIVE))  p.switchGamemode(Gamemode::CREATIVE);
                if (ImGui::MenuItem("Spectator", nullptr, gm==Gamemode::SPECTATOR)) p.switchGamemode(Gamemode::SPECTATOR);

                ImGui::EndMenu();
            }
            ImGui::SliderFloat("Breaking Terrain Interval in CreativeMode", &Settings::gInterval_BreakingTerrain_CreativeMode, 0, 0.5f);

            ImGui::Separator();

            ImGui::Checkbox("Text Info", &Dbg::dbg_TextInfo);
            ImGui::Checkbox("View Gizmo", &Dbg::dbg_ViewGizmo);
            ImGui::Checkbox("View Basis", &Dbg::dbg_ViewBasis);
            ImGui::Checkbox("World Basis", &Dbg::dbg_WorldBasis);
            ImGui::SliderInt("World GridSize", &Dbg::dbg_WorldHintGrids, 0, 500);

            ImGui::Separator();

            ImGui::Checkbox("Hit Tracking", &Ethertia::getHitCursor().keepTracking);
            ImGui::SliderFloat("BrushSize", &Ethertia::getHitCursor().brushSize, 0, 16);

            Camera& cam = Ethertia::getCamera();
            ImGui::SliderFloat("Camera Smoothness", &cam.m_Smoothness, 0, 5);
            ImGui::SliderFloat("Camera Roll", &cam.eulerAngles.z, -3.14, 3.14);

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Rendering"))
        {
            ImGui::SliderFloat("FOV", &Ethertia::getCamera().fov, 0, 180);
            ImGui::SliderFloat("ViewDistance", &Settings::s_ViewDistance, 0, 16);
            ImGui::Checkbox("Vsync", &Settings::s_Vsync);

            ImGui::Checkbox("SSAO", &Settings::g_SSAO);
            ImGui::Checkbox("Shadow Mapping", &Settings::g_ShadowMapping);
            ImGui::SliderFloat("Shadow Depth Map Re-Render Interval", &Settings::gInterval_ShadowDepthMap, 0, 10);

            ImGui::Separator();

            ImGui::Checkbox("PauseWorldRender", &Dbg::dbg_PauseWorldRender);
            ImGui::Checkbox("GBuffers", &dbg_Gbuffer);
            ImGui::Checkbox("Border/Norm", &Dbg::dbg_EntityGeo);
            ImGui::Checkbox("HitEntityGeo", &Dbg::dbg_HitPointEntityGeo);

            ImGui::Checkbox("NoVegetable", &Dbg::dbg_NoVegetable);

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Audio"))
        {

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
        if (ImGui::BeginMenu("View"))
        {
            ImGui::Checkbox("Viewport", &Settings::w_Viewport);
            ImGui::Checkbox("Full Viewport", &Settings::w_Viewport_Full);
            ImGui::Checkbox("Entity List", &Settings::w_EntityList);
            ImGui::Checkbox("Entity Inspect", &Settings::w_EntityInsp);
            ImGui::Checkbox("Shader Inspect", &Settings::w_ShaderInsp);
            ImGui::Checkbox("Console", &Settings::w_Console);
            ImGui::Checkbox("Profiler", &Settings::w_Profiler);

            ImGui::Separator();
            ImGui::Checkbox("Settings", &Settings::w_Settings);
            ImGui::Checkbox("Toolbar", &Settings::w_Toolbar);

            ImGui::Checkbox("NodeEditor", &w_NodeEditor);
            ImGui::Checkbox("WorldGenNodeEditor", &w_WorldGenNodeEditor);
            ImGui::Checkbox("TitleScreen", &w_TitleScreen);
            ImGui::Checkbox("Singleplayer", &w_Singleplayer);
            ImGui::Checkbox("ImGui Demo Window", &w_ImGuiDemo);


            ImGui::Separator();

            static bool ShowHUD = true;
            if (ImGui::MenuItem("HUD", "F1", &ShowHUD)) {

            }
            if (ImGui::MenuItem("Save Screenshot", KeyBindings::KEY_SCREENSHOT.keyName())) {
                Controls::saveScreenshot();
            }
            if (ImGui::MenuItem("Fullscreen", KeyBindings::KEY_FULLSCREEN.keyName(), Ethertia::getWindow().isFullscreen())) {
                Ethertia::getWindow().toggleFullscreen();
            }
            if (ImGui::MenuItem("Controlling Game", KeyBindings::KEY_ESC.keyName(), Ethertia::isIngame())) {
                Ethertia::isIngame() = !Ethertia::isIngame();
            }

            ImGui::EndMenu();
        }

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
    ImGui::Begin("ShaderProgram", &Settings::w_Settings);


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
        shader->reload_sources_by_filenames();
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
    ImGui::Begin("Entity", &Settings::w_EntityInsp);

    Entity* entity = Imgui::g_InspEntity;
    if (!entity) {
        ImGui::TextDisabled("No entity selected.");
        ImGui::End();
        return;
    }
//    ImGui::BeginChild();

    ImGui::TextDisabled("%i components", (int)entity->numComponents());

    if (ImGui::CollapsingHeader("Transform")) {

        ImGui::DragFloat3("Position", &entity->position()[0]);
        ImGui::DragFloat3("Rotation", &entity->position()[0]);
        ImGui::DragFloat3("Scale", &entity->position()[0]);

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
//            EntityComponentTransform& ct = entity->getComponent<EntityComponentTransform>();

            glm::mat4 matModel = Mth::matModel(entity->position(),
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

                entity->position() = pos;
//                *(glm::mat3*)entity->rot() = rot;
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

#include <imgui_internal.h>


#include <ethertia/init/ItemTextures.h>

static void ItemImage(const Item* item, float size = 40, ImDrawList* dl = ImGui::GetWindowDrawList())
{
    float n = Item::REGISTRY.size();
    float i = Item::REGISTRY.getOrderId(item);
    ImVec2 uvMin = {i/n, 1};
    ImVec2 uvSize = {1.0f/n, -1};
    ImVec2 min = ImGui::GetCursorScreenPos();
    dl->AddImage(ItemTextures::ITEM_ATLAS->pTexId(), min, min+ImVec2{size, size}, uvMin, uvMin+uvSize);
    ImGui::Dummy({size, size});
}

static ItemStack s_HoldingItemStack;

void RenderItemStack(ItemStack& stack, bool manipulation = false, float size = 40)
{
    ImVec2 pos = ImGui::GetCursorScreenPos();  // before ItemImage()

    if (!stack.empty())
    {
        ItemImage(stack.item(), size);
    }
    bool k = ImGui::IsKeyPressed(ImGuiKey_MouseLeft);

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
    {
        if (!stack.empty())
        {
            ImGui::SetTooltip("%s", stack.item()->m_Name.c_str());
        }

        if (manipulation && ImGui::IsKeyPressed(ImGuiKey_MouseLeft, false))
        {

            if (s_HoldingItemStack.empty() && !stack.empty())
            {
                stack.moveTo(s_HoldingItemStack);
            }
            else if (!s_HoldingItemStack.empty() && stack.empty())
            {
                s_HoldingItemStack.moveTo(stack);
            }
        }
    }

    // Amount
    if (!stack.empty())
    {
        std::string str = std::to_string(stack.m_Amount);
        ImVec2 t_size = ImGui::CalcTextSize(str.c_str());

        ImGui::RenderText(pos + ImVec2{size-t_size.x, size-14}, str.c_str());
    }
}

#include <ethertia/entity/passive/EntityChicken.h>
#include <ethertia/entity/EntityFurnace.h>


static void CreateEntityToWorld(Entity* e)
{
    e->position() = Ethertia::getCamera().position;;
    Ethertia::getWorld()->addEntity(e);
}

static void ShowEntities()
{
    ImGui::Begin("Entities", &Settings::w_EntityList);

    World* world = Ethertia::getWorld();
    if (!world) {
        ImGui::TextDisabled("World not loaded.");
        ImGui::End();
        return;
    }
    auto& entities = world->m_Entities;

    if (ImGui::Button(" + ")) {
        ImGui::OpenPopup("new_entity");
    }
    if (ImGui::BeginPopup("new_entity")) {
        bool disabledDueNoWorld = Ethertia::getWorld() == nullptr;
        if (disabledDueNoWorld) {
            ImGui::TextDisabled("Disabled due No World Loaded");
            ImGui::BeginDisabled();
        }

        if (ImGui::BeginMenu("Vehicle")) {
            if (ImGui::MenuItem("Helicopter"))
            {
                CreateEntityToWorld(new EntityHelicopter());
            }
//            if (ImGui::MenuItem("DrivingSeat"))
//            {
//                CreateEntityToWorld(new EntityDrivingSeat());
//            }
//            if (ImGui::MenuItem("Propeller"))
//            {
//                CreateEntityToWorld(new EntityPropeller());
//            }
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Light")) {

        }
        if (ImGui::MenuItem("Mesh"))
        {
            CreateEntityToWorld(new EntityMesh());
        }
        if (ImGui::MenuItem("Chicken"))
        {
            CreateEntityToWorld(new EntityChicken());
        }
        if (ImGui::MenuItem("Furnace"))
        {
            CreateEntityToWorld(new EntityFurnace());
        }
        ImGui::SeparatorText("Items");

        for (auto& it : Item::REGISTRY)
        {
            if (ImGui::MenuItem(it.first.c_str()))
            {
                ItemStack stack(it.second, 10);
                Ethertia::getPlayer()->m_Inventory.putItemStack(stack);
            }
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ItemImage(it.second);
                ImGui::EndTooltip();
            }
        }

        if (disabledDueNoWorld) {
            ImGui::EndDisabled();
        }
        ImGui::EndPopup();
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


        ImGui::TextDisabled("%i rendered / %i loaded.", Dbg::dbg_NumEntityRendered, (int)entities.size());

        if (ImGui::Button("Unselect")) {
            Imgui::g_InspEntity = nullptr;
        }

        ImGui::EndPopup();
    }

    if (_KeepSelectHitEntity) {
        auto& cur = Ethertia::getHitCursor();
        if (cur.hitEntity) {
            Imgui::g_InspEntity = cur.hitEntity;
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
            if (ImGui::Selectable(buf, Imgui::g_InspEntity == e)) {
                Imgui::g_InspEntity = e;
            }
            ++i;
        }
    }


    ImGui::End();
}

#include <FastNoise/FastNoise.h>
#include <FastNoise/Metadata.h>
#include <ethertia/world/gen/NoiseGen.h>
#include "ImWorldGenNodeEditor.h"


void generatePreviewNoise(FastNoise::NodeData* nodeData,
                      int nodeId,
                      std::unordered_map<FastNoise::NodeData*, int>& fastNoiseNode2id,
                      std::unordered_map<int, std::array<float, 256 * 256>>& id2previewNoiseData,
                      std::vector<std::pair<int, int>>& links,
                      float mNodeFrequency,
                      int mNodeSeed)
{
    auto generator = FastNoise::NewFromEncodedNodeTree(FastNoise::Metadata::SerialiseNodeData(nodeData, true).c_str(), NoiseGen::g_SIMDLevel);
    if (generator)
    {
        auto genRGB = FastNoise::New<FastNoise::ConvertRGBA8>(NoiseGen::g_SIMDLevel);
        genRGB->SetSource(generator);

        // .contains() func do not exists on my computer.. (Verbo @Dreamtowards, 230505)
        if (id2previewNoiseData.find(nodeId) == id2previewNoiseData.end())
        {
            std::array<float, 256 * 256> newArray{};
            newArray.fill(0.0f);
            id2previewNoiseData[nodeId] = newArray;
        }

        genRGB->GenUniformGrid2D(id2previewNoiseData[nodeId].data(), 256 / -2, 256 / -2, 256, 256, mNodeFrequency, mNodeSeed);

        // Update linked nodes
        int startOutputAttributeId = (nodeId << 8) | (( 1 << 8 ) - 1);
        for (auto linkage : links)
        {
            if (linkage.first == startOutputAttributeId)
            {
                int targetNodeId = (int)((unsigned int)linkage.second >> 8);
                FastNoise::NodeData* targetNodeData;
                for (auto& node : fastNoiseNode2id)
                {
                    if (node.second == targetNodeId)
                    {
                        targetNodeData = node.first;
                        generatePreviewNoise(targetNodeData, nodeId, fastNoiseNode2id, id2previewNoiseData, links, mNodeFrequency, mNodeSeed);
                        break;
                    }
                }
                break;
            }
        }
    }
}

using namespace ImWorldGenNodeEditor;

static WorldGenNodeEditor worldGenNodeEditor;
void ShowWorldGenNodeEditor()
{
    worldGenNodeEditor.ShowWorldGenNodeEditor(w_WorldGenNodeEditor);
}

void ShowNodeEditor()
{
    ImGui::Begin("NodeEdit", &w_NodeEditor);

//    // id of Attrib/Pin
//    static std::vector<std::pair<int, int>> g_Links;
//
//    ImNodes::BeginNodeEditor();
//
//    for (int i = 0; i < 4; ++i) {
//        ImNodes::BeginNode(i);
//
//        ImNodes::BeginNodeTitleBar();
//        ImGui::Text("Title");
//        ImNodes::EndNodeTitleBar();
//
//        ImNodes::BeginInputAttribute(i*5+1);
//
//        ImGui::Text("Input3");
//
//        ImNodes::EndInputAttribute();
//
//        ImNodes::BeginOutputAttribute(i*5+2, ImNodesPinShape_Triangle);
//        static bool bl;
//        ImGui::Checkbox("SthCheck", &bl);
//        ImNodes::EndOutputAttribute();
//
//        ImNodes::BeginStaticAttribute(i*5+3);
//        ImGui::Text("Static");
//        ImNodes::EndStaticAttribute();
//
//        ImNodes::EndNode();
//    }
//
//    {
//        int i = 0;
//        for (auto& lk : g_Links) {
//            ImNodes::Link(++i, lk.first, lk.second);
//        }
//    }
//    ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomLeft);  // call before ImNodes::EndNodeEditor();
//    ImNodes::EndNodeEditor();
//
//    {
//        int attr_beg, attr_end;
//        if (ImNodes::IsLinkCreated(&attr_beg, &attr_end)) {  // call after ImNodes::EndNodeEditor();
//            g_Links.emplace_back(attr_beg, attr_end);
//        }
//    }
//

    ImGui::End();
}


class ImKeymap
{
public:
    inline static float _keysize_std = 54;
    inline static ImVec2 _keysize = {_keysize_std, _keysize_std};
    inline static float _keygap = 4;

    inline static std::function<ImU32(ImGuiKey, bool hover)> g_FuncKeyColor = [](auto, auto){return -1;};

    static void RenderKey(ImVec2& p, const char* name, ImGuiKey keycode, float keySizeX = 1.0f, float sizeAddGaps = 0)
    {
        float gapAdd = sizeAddGaps * _keygap;

        ImVec2 offset = {2, 2};
        if (name[1] == '\0')
            offset = {0.5f*(_keysize.x-8), 0.5f*(_keysize.y-12)};

        ImVec2 size = ImVec2(keySizeX * _keysize.x + gapAdd, _keysize.y);
        bool hover = ImGui::IsMouseHoveringRect(p, p+size);

        ImU32 col = ImGui::GetColorU32(hover ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
        if (ImGui::IsKeyDown(keycode)) {
            col = ImGui::GetColorU32(ImGuiCol_HeaderActive);
        }
        ImU32 evalColor = g_FuncKeyColor(keycode, hover);
        if (evalColor != -1) {
            col = evalColor;
        }

        ImGui::RenderFrame(p, p+size,col, true, ImGui::GetStyle().FrameRounding);
        ImGui::RenderText(p+offset, name);

        p.x += keySizeX * _keysize.x + gapAdd + _keygap;
    }

    static void ShowKeymap()
    {
        ImVec2 begin = ImGui::GetCursorScreenPos();
        ImVec2 p = begin;

        _keysize.y = _keysize_std * 0.5f;
        RenderKey(p, "ESC", ImGuiKey_Escape); p.x += _keysize.x + _keygap;
        RenderKey(p, "F1", ImGuiKey_F1);
        RenderKey(p, "F2", ImGuiKey_F1);
        RenderKey(p, "F3", ImGuiKey_F3);
        RenderKey(p, "F4", ImGuiKey_F4); p.x += 0.5f * (_keysize.x + _keygap);
        RenderKey(p, "F5", ImGuiKey_F5);
        RenderKey(p, "F6", ImGuiKey_F6);
        RenderKey(p, "F7", ImGuiKey_F7);
        RenderKey(p, "F8", ImGuiKey_F8); p.x += 0.5f * (_keysize.x + _keygap);
        RenderKey(p, "F9", ImGuiKey_F9);
        RenderKey(p, "F10", ImGuiKey_F10);
        RenderKey(p, "F11", ImGuiKey_F11);
        RenderKey(p, "F12", ImGuiKey_F12);
        _keysize.y = _keysize_std;

#define STEP_ROW p.x = begin.x; p.y += _keysize.y + _keygap;
        STEP_ROW;
        RenderKey(p, "~\n`", ImGuiKey_GraveAccent);
        RenderKey(p, "!\n1", ImGuiKey_1);
        RenderKey(p, "@\n2", ImGuiKey_2);
        RenderKey(p, "#\n3", ImGuiKey_3);
        RenderKey(p, "$\n4", ImGuiKey_4);
        RenderKey(p, "%\n5", ImGuiKey_5);
        RenderKey(p, "^\n6", ImGuiKey_6);
        RenderKey(p, "&\n7", ImGuiKey_7);
        RenderKey(p, "*\n8", ImGuiKey_8);
        RenderKey(p, "(\n9", ImGuiKey_9);
        RenderKey(p, ")\n0", ImGuiKey_0);
        RenderKey(p, "_\n-", ImGuiKey_Minus);
        RenderKey(p, "+\n=", ImGuiKey_Equal);
        RenderKey(p, "backspace", ImGuiKey_Backspace, 2.0f, 1);

        STEP_ROW
        RenderKey(p, "tab", ImGuiKey_Tab, 1.5f);
        RenderKey(p, "Q", ImGuiKey_Q);
        RenderKey(p, "W", ImGuiKey_W);
        RenderKey(p, "E", ImGuiKey_E);
        RenderKey(p, "R", ImGuiKey_R);
        RenderKey(p, "T", ImGuiKey_T);
        RenderKey(p, "Y", ImGuiKey_Y);
        RenderKey(p, "U", ImGuiKey_U);
        RenderKey(p, "I", ImGuiKey_I);
        RenderKey(p, "O", ImGuiKey_O);
        RenderKey(p, "P", ImGuiKey_P);
        RenderKey(p, "{\n[", ImGuiKey_LeftBracket);
        RenderKey(p, "}\n]", ImGuiKey_RightBracket);
        RenderKey(p, "|\n\\", ImGuiKey_Backslash, 1.5f, 1);

        STEP_ROW;
        RenderKey(p, "caps", ImGuiKey_CapsLock, 1.75f, 1);
        RenderKey(p, "A", ImGuiKey_A);
        RenderKey(p, "S", ImGuiKey_S);
        RenderKey(p, "D", ImGuiKey_D);
        RenderKey(p, "F", ImGuiKey_F);
        RenderKey(p, "G", ImGuiKey_G);
        RenderKey(p, "H", ImGuiKey_H);
        RenderKey(p, "J", ImGuiKey_J);
        RenderKey(p, "K", ImGuiKey_K);
        RenderKey(p, "L", ImGuiKey_L);
        RenderKey(p, ":\n;", ImGuiKey_Semicolon);
        RenderKey(p, "\"\n'", ImGuiKey_Apostrophe);
        RenderKey(p, "return", ImGuiKey_Enter, 2.25f, 1);

        STEP_ROW;
        RenderKey(p, "shift", ImGuiKey_LeftShift, 2.0f, 1);
        RenderKey(p, "Z", ImGuiKey_Z);
        RenderKey(p, "X", ImGuiKey_X);
        RenderKey(p, "C", ImGuiKey_C);
        RenderKey(p, "V", ImGuiKey_V);
        RenderKey(p, "B", ImGuiKey_B);
        RenderKey(p, "N", ImGuiKey_N);
        RenderKey(p, "M", ImGuiKey_M);
        RenderKey(p, "<\n,", ImGuiKey_Comma);
        RenderKey(p, ">\n.", ImGuiKey_Period);
        RenderKey(p, "?\n/", ImGuiKey_Slash);
        RenderKey(p, "shift", ImGuiKey_RightShift, 3, 2);

        STEP_ROW;
        RenderKey(p, "fn", ImGuiKey_F);
        RenderKey(p, "ctrl", ImGuiKey_LeftCtrl);
        RenderKey(p, "alt", ImGuiKey_LeftAlt);
        RenderKey(p, "super", ImGuiKey_LeftSuper);
        RenderKey(p, "", ImGuiKey_Space, 6, 5);
        RenderKey(p, "super", ImGuiKey_RightSuper);
        RenderKey(p, "alt", ImGuiKey_RightAlt);

        float tmpY = p.y;
        _keysize.y = _keysize_std*0.5f - _keygap*0.5f;
        p.y += _keysize.y + _keygap;
        RenderKey(p, "<", ImGuiKey_LeftArrow);
        float tmpX = p.x;
        RenderKey(p, "v", ImGuiKey_DownArrow);
        RenderKey(p, ">", ImGuiKey_RightArrow);
        p = {tmpX, tmpY};
        RenderKey(p, "^", ImGuiKey_UpArrow);
        p.y += _keysize.y + _keygap;
        p.y += 4;

        STEP_ROW;
        float mbSize = _keysize.x;
        RenderKey(p, "MLB", ImGuiKey_MouseLeft);
        _keysize.x = 40;
        RenderKey(p, "MMB", ImGuiKey_MouseMiddle);
        _keysize.x = mbSize;
        RenderKey(p, "MRB", ImGuiKey_MouseRight);


        ImGui::ItemSize(p-begin + ImVec2(0, 60));
//        ImGui::SetCursorScreenPos({begin.x, p.y + 60});
    }


};


#include <ethertia/init/KeyBinding.h>
#include <ethertia/world/ChunkLoader.h>

static void ShowSettingsWindow()
{
    ImGui::Begin("Settings", &Settings::w_Settings);

    SettingsPanel& currp = g_CurrSettingsPanel;

    ImGui::BeginChild("SettingNav", {150, 0}, true);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {8, 8});

    if (ImGui::RadioButton("Profile", currp==Profile)) { currp=Profile; }
    if (ImGui::RadioButton("Current World", currp==CurrentWorld)) { currp=CurrentWorld; }
    ImGui::Separator();
    if (ImGui::RadioButton("Graphics", currp==Graphics)) { currp=Graphics; }
    if (ImGui::RadioButton("Music & Sounds", currp==Audio)) { currp=Audio; }
    if (ImGui::RadioButton("Controls", currp==Controls)) { currp=Controls; }
    if (ImGui::RadioButton("Language", currp==Language)) { currp=Language; }
    ImGui::Separator();
    if (ImGui::RadioButton("Mods", currp==Mods)) { currp=Mods; }
    if (ImGui::RadioButton("Resource Packs", currp==ResourcePacks)) { currp=ResourcePacks; }
    ImGui::Separator();
    if (ImGui::RadioButton("Credits", currp==Credits)) { currp=Credits; }
    if (ImGui::RadioButton("Misc", currp==Misc)) { currp=Misc; }

    ImGui::PopStyleVar();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("SettingPanel", {0,0}, true);
    {
        if (currp==Profile)
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
        else if (currp==CurrentWorld)
        {
            World* world = Ethertia::getWorld();
            if (!world)
            {
                ImGui::TextDisabled("No world loaded.");
            }
            else
            {
                ImGui::BeginDisabled();
                const WorldInfo& winf = world->m_WorldInfo;

                static char WorldName[128];
                ImGui::InputText("World Name", WorldName, 128);

                static char WorldSeed[128];
                ImGui::InputText("World Seed", WorldSeed, 128);

                ImGui::EndDisabled();

                ImGui::SliderFloat("Day Time", &world->m_WorldInfo.DayTime, 0, 1);
                ImGui::SliderFloat("Day Time Length", &world->m_WorldInfo.DayLength, 1, 3600);


                ImGui::ColorEdit3("Sun Color", &Dbg::dbg_WorldSunColor.x);
                ImGui::ColorEdit3("Dbg Color", &Dbg::dbg_ShaderDbgColor.x);
                ImGui::DragFloat("Sun Brightness Mul", &Dbg::dbg_WorldSunColorBrightnessMul, 0.1);


                if (ImGui::Button("Open World Save Directory"))
                {
                    Loader::openURL(world->m_ChunkLoader->m_ChunkDir);
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("%s", world->m_ChunkLoader->m_ChunkDir.c_str());
                }
            }

        }
        else if (currp==Graphics)
        {
//            ImGui::SeparatorText("Bloom");
//            ImGui::Checkbox("Bloom", &Settings::g_SSAO);



            ImGui::SeparatorText("General");

            ImGui::SliderFloat("FOV", &Ethertia::getCamera().fov, 0, 180);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Field of View.\nNormal: 70\nQuark Pro: 90");

            ImGui::Checkbox("Vsync", &Settings::s_Vsync);

            if (Settings::s_Vsync) ImGui::BeginDisabled();
            ImGui::SliderInt("FPS Limit", &Settings::s_FpsCap, 0, 2000);
            if (Settings::s_Vsync) ImGui::EndDisabled();


            static float s_SlidingGuiScale = 0;
            float _gui_scale = Imgui::GlobalScale;
            if (ImGui::SliderFloat("GUI Scale", &_gui_scale, 0.5f, 4.0f))
            {
                s_SlidingGuiScale = _gui_scale;
            }
            else if (s_SlidingGuiScale != 0)
            {
                Imgui::GlobalScale = s_SlidingGuiScale;
                s_SlidingGuiScale = 0;
            }


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
        else if (currp==Audio)
        {
            static int g_MasterVolume = 0;
            ImGui::SliderInt("Master Volume", &g_MasterVolume, 0, 100, "%d%%");
            ImGui::Dummy({0, 8});
            ImGui::SliderInt("Music", &g_MasterVolume, 0, 100, "%d%%");
        }
        else if (currp==Controls)
        {
//            ImGui::SeparatorText("Mouse");
//
//            static float s_MouseSensitivity = 1.0f;
//            ImGui::SliderFloat("Mouse Sensitivity", &s_MouseSensitivity, 0, 2);
//
//
//            ImGui::SeparatorText("Keyboard");

            static KeyBinding* s_HoveredKey = nullptr;


            ImGui::BeginChild("KeyBindingList", {270, 0});
            for (auto& it : KeyBinding::REGISTRY)
            {
                auto& name = it.first;
                KeyBinding* keyBinding = it.second;

                ImVec2 line_min = ImGui::GetCursorScreenPos();
                ImVec2 line_max = line_min + ImVec2{270, 18};
                if (ImGui::IsMouseHoveringRect(line_min, line_max) || s_HoveredKey == keyBinding) {
                    ImGui::RenderFrame(line_min, line_max, ImGui::GetColorU32(ImGuiCol_TitleBg));
                    s_HoveredKey = keyBinding;
                }

                ImGui::Text("%s", name.c_str());

                ImGui::SameLine(160);


                if (ImGui::Button(ImGui::GetKeyName(keyBinding->key()), {100, 0})) {}
                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip("press key to set");

                    ImGuiKey k = GetPressedKey();
                    if (k) {
                        keyBinding->m_Key = k;
                    }
                }

                if (keyBinding->key() != keyBinding->m_DefaultKey)
                {
                    ImGui::SameLine(132);

                    ImGui::PushID(name.c_str());
                    if (ImGui::Button("X"))
                    {
                        keyBinding->m_Key = keyBinding->m_DefaultKey;
                    }
                    ImGui::PopID();
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("Reset (to: %s)", ImGui::GetKeyName(keyBinding->m_DefaultKey));
                    }
                }
            }
            ImGui::EndChild();

            ImGui::SameLine();


            bool updatedHoveredKey = false;
            ImKeymap::g_FuncKeyColor = [&updatedHoveredKey](ImGuiKey k, bool hover) -> ImU32 {
                for (auto& it : KeyBinding::REGISTRY) {
                    if (it.second->key() == k) {
                        if (hover) {
                            s_HoveredKey = it.second;
                            updatedHoveredKey = true;

                            ImGui::BeginTooltip();
                            ImGui::Text("%s", it.first.c_str());
                            ImGui::EndTooltip();
                        }
                        if (it.second == s_HoveredKey) {
                            return ImGui::GetColorU32(ImGuiCol_TitleBg);
                        }
                        return ImGui::GetColorU32(ImGuiCol_Header);
                    }
                }
                return -1;
            };
            ImKeymap::ShowKeymap();

            if (!updatedHoveredKey)
                s_HoveredKey = nullptr;
        }
        else if (currp==Language)
        {
            std::vector<std::pair<const char*, const char*>> g_Languages = {
                    {"en_us", "English"},
                    {"zh_tw", "Chinese (Complified)"},
                    {"zh_cn", "Chinese (Simplified)"}
            };
            static const char* g_SelectedLanguage = "en_us";

            ImGui::BeginChild("LangList", {0, 200});
            for (auto it : g_Languages) {
                if (ImGui::Selectable(it.second, g_SelectedLanguage==it.first)) {
                    g_SelectedLanguage = it.first;
                }
            }
            ImGui::EndChild();

            ImGui::TextDisabled("Translation may not 100%% accurate.");
        }
        else if (currp==Mods)
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
        else if (currp==ResourcePacks)
        {
            if (ImGui::Button("Open ResourcePacks folder")) {
                Loader::openURL("./resourcepacks");
            }

            ImGui::BeginChild("AssetsList", {0, 200});
            for (auto& path : Settings::ASSETS) {
                ImGui::Selectable(path.c_str(), false);
            }
            ImGui::EndChild();

        }
        else if (currp==Credits)
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
//                 Imgui::GetWindowContentSize(),
//                 {0,1}, {1,0});

    // LeftBottom Version/Stats
    ImGui::SetCursorPosY(ImGui::GetWindowHeight());
    Imgui::TextAlign(Strings::fmt("0 mods loaded.\n{}", Ethertia::Version::name()).c_str(),
                     {0.0f, 1.0f});

    // RightBottom Copyright
    ImGui::SetCursorPosY(ImGui::GetWindowHeight());
    ImGui::SetCursorPosX(ImGui::GetWindowWidth());
    Imgui::TextAlign("Copyright (c) Eldrine Le Prismarine, Do not distribute!",
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
        Settings::w_Settings = true;
    }
    ImGui::SetCursorPosX(btnX);
    if (ImGui::Button("Terminate", btnSize)) {
        Ethertia::shutdown();
    }


    ImGui::End();
}

static float RenderProfilerSection(const Profiler::Section& sec, float x, float y, float full_width, float full_width_time, const std::function<float(const Profiler::Section&)>& timefunc)
{
    const float LINE_HEIGHT = 16;

    double s_time = timefunc(sec);
    double s_width = (s_time / full_width_time) * full_width;
    glm::vec4 color = Colors::ofRGB(std::hash<std::string>()(sec.name) * 256);
    ImU32 col = ImGui::GetColorU32({color.x, color.y, color.z, color.w});

    ImVec2 min = {x,y};
    ImVec2 max = min+ImVec2(s_width, LINE_HEIGHT);
    ImGui::RenderFrame(min, max, col);
    ImGui::RenderText(min, sec.name.c_str());

    if (s_width > 180) {
        std::string str = Strings::fmt("{}ms@{}", s_time * 1000.0, sec.numExec);
        ImVec2 text_size = ImGui::CalcTextSize(str.c_str());
        ImGui::RenderText({max.x-text_size.x, min.y}, str.c_str());
    }

    float dx = 0;
    for (const Profiler::Section& sub_sec : sec.sections)
    {
        dx += RenderProfilerSection(sub_sec, x+dx, y-LINE_HEIGHT, s_width, s_time, timefunc);
    }

    if (ImGui::IsWindowFocused() && ImGui::IsMouseHoveringRect(min, max))
    {
        ImGui::SetTooltip("%s\n"
                          "\n"
                          "avg %fms\n"
                          "las %fms\n"
                          "sum %fms\n"
                          "exc %u\n"
                          "%%p  %f",
                          sec.name.c_str(),
                          sec._avgTime * 1000.0f,
                          sec._lasTime * 1000.0f,
                          sec.sumTime  * 1000.0f,
                          (uint32_t)sec.numExec,
                          (float)(sec.parent ? sec.sumTime / sec.parent->sumTime : Mth::NaN));
    }

    return s_width;
}

static void ShowProfilers()
{
    ImGui::Begin("Profiler");

    static int s_SelectedProfilerIdx = 0;
    static std::pair<const char*, Profiler*> PROFILERS[] = {
            {"MainLoop", &Ethertia::getProfiler()},
            {"Chunk Mesh", &Dbg::dbgProf_ChunkMeshGen},
            {"Chunk Gen/Load/Save", &Dbg::dbgProf_ChunkGen}
    };


    ImGui::SetNextItemWidth(200);
    if (ImGui::BeginCombo("###Profiler", PROFILERS[s_SelectedProfilerIdx].first))
    {
        for (int i = 0; i < std::size(PROFILERS); ++i)
        {
            if (ImGui::Selectable(PROFILERS[i].first, s_SelectedProfilerIdx == i)) {
                s_SelectedProfilerIdx = i;
            }
        }
        ImGui::EndCombo();
    }


    ImGui::SameLine();

    static int s_SelectedTimeFunc = 0;
    static std::pair<const char*, std::function<float(const Profiler::Section& sec)>> s_TimeFuncs[] = {
            {"AvgTime", [](const Profiler::Section& sec) { return sec._avgTime; }},
            {"SumTime", [](const Profiler::Section& sec) { return sec.sumTime; }},
            {"LastTime",[](const Profiler::Section& sec) { return sec._lasTime; }}
    };
    ImGui::SetNextItemWidth(120);
    if (ImGui::BeginCombo("###ProfilerTimeFunc", s_TimeFuncs[s_SelectedTimeFunc].first))
    {
        for (int i = 0; i < std::size(s_TimeFuncs); ++i)
        {
            if (ImGui::Selectable(s_TimeFuncs[i].first, s_SelectedTimeFunc == i)) {
                s_SelectedTimeFunc = i;
            }
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();

    Profiler& prof = *PROFILERS[s_SelectedProfilerIdx].second;

    if (ImGui::Button("Reset"))
    {
        prof.laterClearRootSection();
    }

    const Profiler::Section& sec = prof.GetRootSection();
    auto& timefunc = s_TimeFuncs[s_SelectedTimeFunc].second;
    ImVec2 begin = ImGui::GetWindowPos() + ImVec2{ImGui::GetWindowContentRegionMin().x, ImGui::GetWindowContentRegionMax().y - 16};
    float width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;
    RenderProfilerSection(sec, begin.x, begin.y, width, timefunc(sec), timefunc);

    ImGui::End();
}

// draw AABB Box for InvalidMeshChunks (To be mesh), and draw Mesh Counter for each chunk indicates how many times the chunk had meshed.
static void DbgShowChunkMeshingAndCounter()
{
    Ethertia::getWorld()->forLoadedChunks([](Chunk* c)
    {
        if (Dbg::dbg_MeshingChunksAABB && c->m_MeshingState != Chunk::MESH_VALID)
        {
            Imgui::RenderAABB(c->chunkpos(), c->chunkpos()+glm::vec3(16),
                              c->m_MeshingState == Chunk::MESH_INVALID ?  // MeshInvalid or Meshing
                                ImGui::GetColorU32({0.2, 0, 0, .5}) :
                                ImGui::GetColorU32({1, 0, 0, 1}));
        }

        if (Dbg::dbg_ChunkMeshedCounter)
        {
            glm::vec3 pTextW = c->chunkpos() + glm::vec3(8.0f);
            glm::vec2 pTextS;
            if (Imgui::CalcViewportWorldpos(pTextW, pTextS))
            {
                ImGui::RenderText({pTextS.x, pTextS.y}, Strings::fmt("Meshed x{}", c->dbg_MeshCounter).c_str());
            }
        }
    });


}


void Imgui::RenderWorldLine(glm::vec3 p0, glm::vec3 p1, ImU32 col, float thickness)
{
    glm::vec2 p0s, p1s;
    if (Imgui::CalcViewportWorldpos(p0, p0s) && Imgui::CalcViewportWorldpos(p1, p1s))
    {
        ImGui::GetWindowDrawList()->AddLine({p0s.x, p0s.y}, {p1s.x, p1s.y}, col, thickness);
    }
}

void Imgui::RenderAABB(glm::vec3 min, glm::vec3 max, ImU32 col, float tk)
{
    using glm::vec3; using glm::vec2;
    // p0-3: on min.z
    vec3 p0 = min;
    vec3 p1 = {max.x, min.y, min.z};
    vec3 p2 = {min.x, max.y, min.z};
    vec3 p3 = {max.x, max.y, min.z};
    // on max.z
    vec3 p4 = {min.x, min.y, max.z};
    vec3 p5 = {max.x, min.y, max.z};
    vec3 p6 = {min.x, max.y, max.z};
    vec3 p7 = max;

    RenderWorldLine(p0, p1, col, tk); RenderWorldLine(p2, p3, col, tk); RenderWorldLine(p4, p5, col, tk); RenderWorldLine(p6, p7, col, tk);  // X
    RenderWorldLine(p0, p2, col, tk); RenderWorldLine(p1, p3, col, tk); RenderWorldLine(p4, p6, col, tk); RenderWorldLine(p5, p7, col, tk);  // Y
    RenderWorldLine(p0, p4, col, tk); RenderWorldLine(p1, p5, col, tk); RenderWorldLine(p2, p6, col, tk); RenderWorldLine(p3, p7, col, tk);  // Z
}

void Imgui::RenderAABB(const AABB& aabb, glm::vec4 c)
{
    Imgui::RenderAABB(aabb.min, aabb.max, ImGui::GetColorU32({c.x, c.y, c.z, c.w}));
}

static void ShowDockspaceAndMainMenubar()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});

    ImVec4 Blu = {0.188, 0.478, 0.776, 1.0};
    ImVec4 Pur = {0.373, 0.157, 0.467, 1.0};
    ImVec4 Org = {0.741, 0.345, 0.133, 1.0};
    ImVec4 _Dar = {0.176f, 0.176f, 0.176f, 0.700f};
    ImVec4 _col =
            Dbg::dbg_PauseWorldRender ? Org :
            Ethertia::isIngame() ? _Dar : Pur;
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, _col);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
                                    | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                                    | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
    ImGui::Begin("MainDockSpaceWindow", nullptr, window_flags);
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();

    ImGui::DockSpace(ImGui::GetID("MainDockSpace"), {0, 0},
                     ImGuiDockNodeFlags_PassthruCentralNode);  //ImGuiDockNodeFlags_AutoHideTabBar

    ShowMainMenuBar();


    // Draw Holding ItemStack. tmp.
    if (!s_HoldingItemStack.empty())
    {
        ImGui::SetCursorScreenPos(ImGui::GetMousePos() - ImVec2(20, 20));
        ItemImage(s_HoldingItemStack.item(), 40, ImGui::GetForegroundDrawList());
    }

    ImGui::End();
}

static void ShowToolbar()
{
    ImGui::Begin("MASH", nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

#define _TOOLBAR_BTN(id) ImGui::ImageButton(id, LazyLoadTex(id)->pTexId(), {40, 40}, {0,1}, {1,0})

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {1, 1});
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {1, 0});
//    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);

    _TOOLBAR_BTN("gui/geo/sel.png");
    ImGui::SameLine();
    _TOOLBAR_BTN("gui/geo/mov.png");
    ImGui::SameLine();
    _TOOLBAR_BTN("gui/geo/mov-all.png");
    ImGui::SameLine();
    _TOOLBAR_BTN("gui/geo/rot.png");
    ImGui::SameLine();
    _TOOLBAR_BTN("gui/geo/scl.png");
    ImGui::SameLine();
    _TOOLBAR_BTN("gui/geo/bnd.png");
    ImGui::SameLine();

    ImGui::Dummy({4, 0});
    ImGui::SameLine();

    _TOOLBAR_BTN("gui/geo/sel-rect.png");
    ImGui::SameLine();
    _TOOLBAR_BTN("gui/geo/sel-cir.png");
    ImGui::SameLine();
    _TOOLBAR_BTN("gui/geo/sel-cus.png");
    ImGui::SameLine();

    ImGui::Dummy({4, 0});
    ImGui::SameLine();

    _TOOLBAR_BTN("gui/geo/g-cube.png");
    ImGui::SameLine();
    _TOOLBAR_BTN("gui/geo/g-sph.png");
    ImGui::SameLine();
    _TOOLBAR_BTN("gui/geo/g-col.png");
    ImGui::SameLine();
    _TOOLBAR_BTN("gui/geo/g-tor.png");
    ImGui::SameLine();
    _TOOLBAR_BTN("gui/geo/g-cone.png");
    ImGui::SameLine();
    _TOOLBAR_BTN("gui/geo/g-plane.png");
    ImGui::SameLine();

    ImGui::Dummy({4, 0});
    ImGui::SameLine();

    _TOOLBAR_BTN("gui/geo/grab.png");
    ImGui::SameLine();
    _TOOLBAR_BTN("gui/geo/brush.png");
    ImGui::SameLine();
    _TOOLBAR_BTN("gui/geo/pencil.png");
    ImGui::SameLine();
    _TOOLBAR_BTN("gui/geo/fill.png");
    ImGui::SameLine();
    _TOOLBAR_BTN("gui/geo/erase.png");
    ImGui::SameLine();
    _TOOLBAR_BTN("gui/geo/pik.png");
    ImGui::SameLine();
    _TOOLBAR_BTN("gui/geo/fing.png");
    ImGui::SameLine();
    _TOOLBAR_BTN("gui/geo/stamp.png");
    ImGui::SameLine();
    _TOOLBAR_BTN("gui/geo/cpy.png");
    ImGui::SameLine();


    ImGui::PopStyleVar(2);

    ImGui::End();
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
                                    (int)c->exp_meta,
                                    cur.cell_breaking_time);
        }
    }


    std::string str = Strings::fmt(
            "CamPos: {}, len: {}, spd {}mps {}kph; ground: {}, CollPts {}.\n"
            "avg-fps: {}. dt: {}, {}fps\n"
            "NumEntityRendered: {}/{}, LoadedChunks: {}\n"
            "\n"
            "World: {}\n"
            "HitChunk: {}\n"
            "HitCell: {}\n"
            "\n"
            "task {}, async {}\n"
            "ChunkProvide: {}\n"
            "ChunkMeshing: {}\n"
            "ChunkSaving:  {}\n"
            ,
            Ethertia::getCamera().position, Ethertia::getCamera().len,
            meterPerSec, meterPerSec * 3.6f,
            player->m_OnGround, player->m_NumContactPoints,

            Dbg::dbg_FPS, dt, Mth::floor(1.0f/dt),

            Dbg::dbg_NumEntityRendered, world ? world->getEntities().size() : 0, world ? world->getLoadedChunks().size() : 0,

            worldInfo,
            chunkInfo,
            cellInfo,

            Ethertia::getScheduler().numTasks(), Ethertia::getAsyncScheduler().numTasks(),
            DebugStat::dbg_ChunkProvideState ? DebugStat::dbg_ChunkProvideState : "/",
            DebugStat::dbg_NumChunksMeshInvalid,
            DebugStat::dbg_ChunksSaving

    );

    auto& vp = Ethertia::getViewport();
    ImGui::RenderText({vp.x, vp.y+16}, str.c_str());
//    ImGui::SetNextWindowPos({vp.x+0, vp.y+16});
//    ImGui::SetNextWindowBgAlpha(0.0f);
//    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
//    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0,0});
//    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking |
//                                    ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
//    if (ImGui::Begin("DebugTextOverlay", &dbg_Text, window_flags)) {
//        ImGui::Text("%s", str.c_str());
//
//        if (dbg_Gbuffer) {
//            ImVec2 siz = {vp.width / 10, vp.height / 10};
//            Imgui::Image(GeometryRenderer::fboGbuffer->texColor[0]->texId, siz);  // Pos.rgb Dep.a
//            Imgui::Image(GeometryRenderer::fboGbuffer->texColor[1]->texId, siz);  // Norm.rgb
//            Imgui::Image(GeometryRenderer::fboGbuffer->texColor[2]->texId, siz);  // Albedo.rgb
//
//            Imgui::Image(SSAORenderer::fboSSAO->texColor[0]->texId, siz);  // AO.r
//            Imgui::Image(ShadowRenderer::fboDepthMap->texDepth->texId, siz);  // Depth.texDepth.r
//        }
//    }
//    ImGui::End();
//    ImGui::PopStyleVar(2);
}


static void DrawViewportDebugs()
{

    if (Dbg::dbg_ViewGizmo)
    {
        static float camLen = 10.0f;
        auto& vp = Ethertia::getViewport();
        ImGuizmo::ViewManipulate(glm::value_ptr(Ethertia::getCamera().matView), camLen,
                                 ImVec2(vp.right()-128-24, vp.y+24), ImVec2(128, 128),
                //0x10101010
                                 0);
    }
    if (Dbg::dbg_WorldHintGrids > 0)
    {
        glm::mat4 iden(1.0f);
        ImGuizmo::DrawGrid(glm::value_ptr(Ethertia::getCamera().matView), glm::value_ptr(Ethertia::getCamera().matProjection),
                           glm::value_ptr(iden), (float)Dbg::dbg_WorldHintGrids);
    }

    if (Dbg::dbg_ChunkMeshedCounter || Dbg::dbg_MeshingChunksAABB)
    {
        DbgShowChunkMeshingAndCounter();
    }
    if (Dbg::dbg_TextInfo)
    {
        ShowDebugTextOverlay();
    }
    if (Dbg::dbg_WorldBasis)
    {
        float n = 5;
        Imgui::RenderWorldLine({0,0,0}, {n,0,0}, ImGui::GetColorU32({1,0,0,1}));
        Imgui::RenderWorldLine({0,0,0}, {0,n,0}, ImGui::GetColorU32({0,1,0,1}));
        Imgui::RenderWorldLine({0,0,0}, {0,0,n}, ImGui::GetColorU32({0,0,1,1}));
    }
    if (Dbg::dbg_ViewBasis)
    {
        glm::mat4* pView = &Ethertia::getCamera().matView;
        glm::mat4 camView = *pView;
        *pView = glm::mat4(1.0f);  // disable view matrix. of RenderWorldLine
        float n = 0.016f;
        glm::vec3 o = {0, 0, -0.1};
        glm::mat3 rot(camView);
        Imgui::RenderWorldLine(o, o+rot*glm::vec3(n, 0, 0), ImGui::GetColorU32({1,0,0,1}));
        Imgui::RenderWorldLine(o, o+rot*glm::vec3(0, n, 0), ImGui::GetColorU32({0,1,0,1}));
        Imgui::RenderWorldLine(o, o+rot*glm::vec3(0, 0, n), ImGui::GetColorU32({0,0,1,1}));
        *pView = camView;  // restore.
    }

    World* world = Ethertia::getWorld();
    if (world) {
        if (dbg_AllEntityAABB) {
            for (Entity* e : world->m_Entities) {
                if (Ethertia::getCamera().testFrustum(e->getAABB()))
                    Imgui::RenderAABB(e->getAABB(), Colors::RED);
            }
        }
        if (dbg_AllChunkAABB) {
            world->forLoadedChunks([&](Chunk* chunk){
                Imgui::RenderAABB({chunk->position, chunk->position + glm::vec3{16.0f}}, Colors::RED);
            });
        }
    }

}

#include <ethertia/item/recipe/Recipe.h>

static void ShowPlayerInventory()
{
    ImGui::Begin("Inventory");

    static bool tmpInited = false;
    if (!tmpInited) {
        tmpInited = true;
        for (auto& it : Item::REGISTRY) {
            ItemStack stack(it.second, 10);
            Ethertia::getPlayer()->m_Inventory.putItemStack(stack);
        }
    }

    const float slot_size = 40;
    {
        const float slot_gap = 4;
        const int row_items = 6;
        ImGui::BeginChild("InventoryStacks", {(slot_size+slot_gap)*row_items+slot_gap, 0});

        ImVec2 stacks_min = ImGui::GetCursorScreenPos();
        ImVec2 min = stacks_min;
        Inventory& inv = Ethertia::getPlayer()->m_Inventory;
        for (int i = 0; i < inv.size(); ++i)
        {
            ItemStack& stack = inv.at(i);

            ImGui::SetCursorScreenPos(min);
            ImGui::Button("###InvStackClick", {slot_size, slot_size});

            ImGui::SetCursorScreenPos(min);
            RenderItemStack(stack, true, slot_size);

            if ((i+1) % row_items == 0) {
                min.x = stacks_min.x;
                min.y += slot_size + slot_gap;
            } else {
                min.x += slot_size + slot_gap;
            }
        }

        ImGui::EndChild();
    }
    ImGui::SameLine();
    {
        ImGui::BeginChild("InventoryRecipes", {190, 0});

        for (auto& it : Recipe::REGISTRY)
        {
            Recipe* recipe = it.second;

            ImVec2 mark = ImGui::GetCursorScreenPos();
            ImGui::Button("###RecipeCraft", {180, slot_size});
            ImGui::SetCursorScreenPos(mark);

            RenderItemStack(recipe->m_Result, false, slot_size);

            for (ItemStack& src_stack : recipe->m_Source)
            {
                ImGui::SameLine();
                RenderItemStack(src_stack, false, slot_size * 0.6f);
            }


//            ImGui::Dummy({180, slot_size});
        }

        ImGui::EndChild();
    }

    ImGui::End();
}

static void ShowGameViewport()
{
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;

    static ImGuiID _ViewportLastDockId = 0;
    static bool _RequestSetBackToLastDock = false;  // when just cancel full viewport

    // WorkArea: menubars
    if (Settings::w_Viewport_Full)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;  // ImGuiWindowFlags_NoDocking
        _RequestSetBackToLastDock = true;
    } else if (_RequestSetBackToLastDock) {
        _RequestSetBackToLastDock = false;
        ImGui::SetNextWindowDockID(_ViewportLastDockId);
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::Begin("Viewport", &Settings::w_Viewport, windowFlags);
    ImGui::PopStyleVar(2);

    if (ImGui::GetWindowDockID()) {
        _ViewportLastDockId = ImGui::GetWindowDockID();
    }

    ImVec2 size = Imgui::GetWindowContentSize();
    ImVec2 pos = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
    Imgui::wViewportXYWH = {pos.x, pos.y, size.x, size.y};

    Imgui::Image(ComposeRenderer::fboCompose->texColor[0]->texId, size);
    ImGui::SetCursorPos({0,0});
    ImGui::InvisibleButton("PreventsGameWindowMove", size);


    ImGuizmo::BeginFrame();
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();
    auto& vp = Ethertia::getViewport();
    ImGuizmo::SetRect(vp.x, vp.y, vp.width, vp.height);


    DrawViewportDebugs();


    // Hotbar
    EntityPlayer& player = *Ethertia::getPlayer();
    int gm = player.getGamemode();
    if (gm == Gamemode::SURVIVAL || gm == Gamemode::CREATIVE)
    {
        int hotbarSlots = std::min((int)player.m_Inventory.size(), 8);

        if (Ethertia::isIngame())
        {
            player.m_HotbarSlot += Mth::signal(-Ethertia::getWindow().getDScroll());
            player.m_HotbarSlot = Mth::clamp(player.m_HotbarSlot, 0, hotbarSlots);
        }

        float hotbarSlotSize = 45;
        float hotbarSlotGap = 4;
        float hotbarWidth = (hotbarSlotSize + hotbarSlotGap) * hotbarSlots - hotbarSlotGap;
        const ImVec2 hotbar_min = {vp.x + (vp.width-hotbarWidth)/2,
                      vp.y + vp.height - hotbarSlotSize - hotbarSlotGap};
        ImVec2 size = {hotbarSlotSize, hotbarSlotSize};
        static ImU32 col_bg = ImGui::GetColorU32({0, 0, 0, 0.3});
        static ImU32 col_bg_sel = ImGui::GetColorU32({1, 1, 1, 0.5});

        // Player Inventory Hotbar
        ImVec2 min = hotbar_min;
        for (int i = 0; i < hotbarSlots; ++i)
        {
            ImGui::RenderFrame(min, min+size, i == player.m_HotbarSlot ? col_bg_sel : col_bg);
            ItemStack& stack = player.m_Inventory.at(i);

            if (!stack.empty())
            {
                ImGui::SetCursorScreenPos(min);
                RenderItemStack(stack, false, hotbarSlotSize);
            }

            min.x += hotbarSlotSize + hotbarSlotGap;
        }

        // Player Health
        if (gm == Gamemode::SURVIVAL)
        {
            float healthWidth = hotbarWidth * 0.42f;
            float healthHeight = 4;
            static ImU32 col_health_bg = ImGui::GetColorU32({0.3, 0.3, 0.3, 0.6});
            static ImU32 col_health = ImGui::GetColorU32({0.8, 0.3, 0.3, 1});
            float perc = player.m_Health / 10.0f;

            min = hotbar_min + ImVec2(0, -healthHeight - 8);
            ImGui::RenderFrame(min, min+ImVec2(healthWidth, healthHeight), col_health_bg);
            ImGui::RenderFrame(min, min+ImVec2(healthWidth * perc, healthHeight), col_health);
        }
    }

    const ImVec2 vpCenter = {vp.x + vp.width/2, vp.y + vp.height/2};

    // Crosshair
    if (!Dbg::dbg_ViewBasis)
    {
        // want make a 3d Crosshair like MCVR. but it requires a special Pipeline (shaders).
//        HitCursor& cur = Ethertia::getHitCursor();
//        glm::mat4 matModel = glm::lookAt(cur.position, cur.position + cur.normal, glm::vec3(0, 1, 0));

        ImVec2 min = vpCenter;
        ImVec2 size = {2, 2};
        ImGui::RenderFrame(min-size/2, min+size, ImGui::GetColorU32({1,1,1,1}));
    }

    // Cell Breaking Time Indicator
    HitCursor& cur = Ethertia::getHitCursor();
    if (gm == Gamemode::SURVIVAL && cur.cell_breaking_time)
    {
        float width = 40;
        float h = 4;
        ImVec2 min = vpCenter + ImVec2(-width/2, -12);
        float perc = cur.cell_breaking_time / Dbg::dbg_CurrCellBreakingFullTime;

        static ImU32 col_bg = ImGui::GetColorU32({0.3, 0.3, 0.3, 0.8});
        static ImU32 col_fg = ImGui::GetColorU32({0.8, 0.8, 0.8, 1});
        ImGui::RenderFrame(min, min+ImVec2(width, h), col_bg);
        ImGui::RenderFrame(min, min+ImVec2(width * perc, h), col_fg);
    }

    ImGui::End();
}




static void ShowConsole()
{
    ImGui::Begin("Console", &Settings::w_Console);
    ImGui::BeginChild("###MsgTextList", {0, -ImGui::GetFrameHeightWithSpacing()});
    for (auto& str : Imgui::g_MessageBox) {
        ImGui::Text("%s", str.c_str());
    }
    ImGui::EndChild();
    static char InputBuf[128];

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    if (ImGui::InputText("###MsgBoxInput", InputBuf, 128,
                         ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CtrlEnterForNewLine))
    {
        if (InputBuf[0]) {
            Ethertia::dispatchCommand(InputBuf);
            InputBuf[0] = 0;  // clear.
        }
    }
    ImGui::PopItemWidth();

    // keeping auto focus on the input box
    if (ImGui::IsItemHovered() || (ImGui::IsWindowFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))) {
        //|| (Settings::w_Console_FocusInput)) {  // only enable when on focus.
//        if (ImGui::GetWindowDockID() && Settings::w_Console_FocusInput)
//            ImGui::SetWindowDock(ImGui::GetCurrentWindow(), 0, ImGuiCond_Always);
//        Settings::w_Console_FocusInput = false;

        ImGui::SetWindowFocus();
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
    }

    ImGui::End();
}








static void RenderWindows()
{
    ShowDockspaceAndMainMenubar();

    if (Settings::w_Toolbar)
        ShowToolbar();

    ShowPlayerInventory();

    if (w_ImGuiDemo)
        ImGui::ShowDemoWindow(&w_ImGuiDemo);

    if (w_NewWorld)
        ShowNewWorldWindow();

    if (Settings::w_Profiler)
        ShowProfilers();

    if (Settings::w_EntityList)
        ShowEntities();
    if (Settings::w_EntityInsp) {
        ShowEntityInsp();
        if (Imgui::g_InspEntity) {
            Imgui::RenderAABB(Imgui::g_InspEntity->getAABB(), Colors::YELLOW);
        }
    }
    if (Settings::w_ShaderInsp) {
        ShowShaderProgramInsp();
    }

    if (Settings::w_Viewport)
    {
        ShowGameViewport();
    } else {
        Imgui::wViewportXYWH = {Mth::Inf, 0, 0, 0};
    }

    if (Settings::w_Console)
        ShowConsole();

    if (Settings::w_Settings) {
        ShowSettingsWindow();
    }



    if (w_NodeEditor)
        ShowNodeEditor();
    if (w_WorldGenNodeEditor)
        ShowWorldGenNodeEditor();
    if (w_TitleScreen) {
        ShowTitleScreenWindow();
    }
    if (w_Singleplayer) {
        ShowSingleplayerWindow();
    }


}

