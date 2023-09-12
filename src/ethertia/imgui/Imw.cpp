//
// Created by Dreamtowards on 9/8/2023.
//

#include "Imw.h"

#include <ethertia/imgui/Imgui.h>
#include <imgui-knobs/imgui-knobs.h>
#include <imgui_internal.h>

#include <filesystem>

#include <ethertia/Ethertia.h>
#include <ethertia/init/Settings.h>
#include <ethertia/init/DebugStat.h>
#include <ethertia/init/Controls.h>
#include <ethertia/init/KeyBinding.h>
#include <ethertia/render/Window.h>
#include <ethertia/entity/player/EntityPlayer.h>
#include <ethertia/world/Chunk.h>


static bool
        w_ImGuiDemo = false,
        w_NewWorld = false,
        w_NodeEditor = false,
        w_TitleScreen = false,
        w_Singleplayer = false;

static bool
        dbg_AllEntityAABB = false,
        dbg_AllChunkAABB = false,
        dbg_Gbuffer = false;




#pragma region Main Dockspace & Menubar


static void _ShowMainMenu_System()
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
            const char* filename = Loader::OpenFolderDialog("Open World..", "./saves/");  //std::filesystem::current_path().append("/saves/").string().c_str());
            if (filename) {
                Log::info("Open world: ", filename);
                Ethertia::LoadWorld(filename);
            }
        }

        //ImGui::SeparatorText("Saves");
        ImGui::Separator();
        ImGui::TextDisabled("Saves:");

        if (Loader::FileExists("saves/"))
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
                    Ethertia::LoadWorld(savedir.path().string());
                }
                ImGui::SameLine();
                ImGui::SmallButton("*");
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Modify World");
                }
            }
        }

        ImGui::EndMenu();
    }

    bool worldvalid = Ethertia::GetWorld() != nullptr;
    if (ImGui::MenuItem("Edit World..", nullptr, false, worldvalid))
    {
        Imw::Settings::CurrentPanel = Imw::Settings::Panel::CurrentWorld;
    }
    if (ImGui::MenuItem("Save World", nullptr, false, worldvalid)) {}

    if (ImGui::MenuItem("Close World", nullptr, false, worldvalid)) {
        Ethertia::UnloadWorld();
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Settings.."))
    {
        Imgui::Show(Imw::Settings::ShowSettings);
    }

    if (ImGui::MenuItem("Mods", "0 mods loaded")) {
        Imw::Settings::CurrentPanel = Imw::Settings::Panel::Mods;
    }
    if (ImGui::MenuItem("Resource Packs")) {
        Imw::Settings::CurrentPanel = Imw::Settings::Panel::ResourcePacks;
    }
    if (ImGui::MenuItem("Controls")) {
        Imw::Settings::CurrentPanel = Imw::Settings::Panel::Controls;
    }



    if (ImGui::MenuItem("About")) {
        Imw::Settings::CurrentPanel = Imw::Settings::Panel::Credits;
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Terminate")) {
        Ethertia::shutdown();
    }
}

static void _ShowMainMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("System"))
        {
            _ShowMainMenu_System();
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
                for (auto it : Ethertia::GetWorld()->getLoadedChunks()) {
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
            ImGui::Checkbox("PauseWorldRender", &RenderEngine::s_PauseWorldRender);

            ImGui::Separator();


            ImGui::SliderFloat("FOV", &Ethertia::getCamera().fov, 0, 180);
            ImGui::SliderFloat("ViewDistance", &Settings::s_ViewDistance, 0, 16);
            ImGui::Checkbox("Vsync", &Settings::s_Vsync);

            ImGui::Checkbox("SSAO", &Settings::g_SSAO);
            ImGui::Checkbox("Shadow Mapping", &Settings::g_ShadowMapping);
            ImGui::SliderFloat("Shadow Depth Map Re-Render Interval", &Settings::gInterval_ShadowDepthMap, 0, 10);

            ImGui::Separator();

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
            Imgui::MenuItemToggleShow("Game", Imw::Gameplay::ShowGame, "F5");
            ImGui::MenuItem("Game Fullwindow", "`", &Imw::Gameplay::GameFullwindow);
            Imgui::MenuItemToggleShow("TitleScreen", Imw::Gameplay::ShowTitleScreen);
            Imgui::MenuItemToggleShow("WorldList", Imw::Gameplay::ShowWorldList);
            Imgui::MenuItemToggleShow("WorldNew", Imw::Gameplay::ShowWorldNew);
//            Imgui::MenuItemToggleShow("WorldModify", Imw::Gameplay::ShowWorldModify);

            ImGui::Separator();
            Imgui::MenuItemToggleShow("Settings", Imw::Settings::ShowSettings, "F8");
            Imgui::MenuItemToggleShow("Hierarchy", Imw::Editor::ShowHierarchy, "F9");
            Imgui::MenuItemToggleShow("Inspector", Imw::Editor::ShowInspector, "F12");
            Imgui::MenuItemToggleShow("WorldGen", Imw::Editor::ShowWorldGen);

            Imgui::MenuItemToggleShow("Toolbar", Imw::Editor::ShowToolbar);
            Imgui::MenuItemToggleShow("Console", Imw::Editor::ShowConsole);
            Imgui::MenuItemToggleShow("Profiler", Imw::Editor::ShowProfiler);

            ImGui::Separator();
            Imgui::MenuItemToggleShow("Debug Info", Imw::Settings::ShowSettings, "F3");
            Imgui::MenuItemToggleShow("ImGui::DemoWindow", ImGui::ShowDemoWindow);

            ImGui::Separator();

            static bool ShowHUD = true;
            if (ImGui::MenuItem("HUD", "F1", &ShowHUD)) {

            }
            if (ImGui::MenuItem("Save Screenshot", KeyBindings::KEY_SCREENSHOT.keyName())) {
                Controls::saveScreenshot();
            }
            if (ImGui::MenuItem("Fullscreen", KeyBindings::KEY_FULLSCREEN.keyName(), Window::isFullscreen())) {
                Window::ToggleFullscreen();
            }
            if (ImGui::MenuItem("Controlling Game", KeyBindings::KEY_ESC.keyName(), Ethertia::isIngame())) {
                Ethertia::isIngame() = !Ethertia::isIngame();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

}

void Imw::ShowDockspaceAndMainMenubar()
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
    ImVec4 Dark = {0.176f, 0.176f, 0.176f, 0.700f};
    ImVec4 _col =
            Dbg::dbg_PauseWorldRender ? Org :
            Ethertia::isIngame() ? Dark : Pur;
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

    _ShowMainMenuBar();


    // Draw Holding ItemStack. tmp.
    if (!g_InventoryHoldingItemStack.empty())
    {
        ImGui::SetCursorScreenPos(ImGui::GetMousePos() - ImVec2(20, 20));
        Imw::ItemImage(g_InventoryHoldingItemStack.item(), 40, ImGui::GetForegroundDrawList());
    }

    ImGui::End();
}

#pragma endregion





//void Imw::InitWindows()
//{
//    if (Settings::w_Toolbar)
//        ShowToolbar();

//    ShowPlayerInventory();

    //     if (w_ImGuiDemo)

//    if (w_NewWorld)
//        ShowNewWorldWindow();
//
//    if (Settings::w_Profiler)
//        ShowProfilers();
//
//    if (Settings::w_EntityList)
//        ShowEntities();
//    if (Settings::w_EntityInsp) {
//        ShowEntityInsp();
//        if (Imgui::g_InspEntity) {
//            Imgui::RenderAABB(Imgui::g_InspEntity->getAABB(), Colors::YELLOW);
//        }
//    }
//    if (Settings::w_ShaderInsp) {
//        ShowShaderProgramInsp();
//    }
//
//    if (Settings::w_Viewport)
//    {
//        ShowGameViewport();
//    } else {
//        Imgui::wViewportXYWH = {Mth::Inf, 0, 0, 0};
//    }
//
//    if (Settings::w_Console)
//        ShowConsole();
//
//    if (Settings::w_Settings) {
//        ShowSettingsWindow();
//    }
//
//
//
//    if (w_NodeEditor)
//        ShowNodeEditor();
//
//
//    if (w_TitleScreen) {
//        ShowTitleScreenWindow();
//    }
//    if (w_Singleplayer) {
//        ShowSingleplayerWindow();
//    }
//}



#include <ethertia/init/ItemTextures.h>

void Imw::ItemImage(const Item* item, float size, ImDrawList* dl)
{
    float n = Item::REGISTRY.size();
    float i = Item::REGISTRY.getOrderId(item);
    ImVec2 uvMin = {i/n, 0};
    ImVec2 uvSize = {1.0f/n, 1};
    ImVec2 min = ImGui::GetCursorScreenPos();
    dl->AddImage(Imgui::mapImage(ItemTextures::ITEM_ATLAS->imageView), min, min+ImVec2{size, size}, uvMin, uvMin+uvSize);
    ImGui::Dummy({size, size});
}


void Imw::ShowItemStack(ItemStack& stack, bool manipulation, float size)
{
    ImVec2 pos = ImGui::GetCursorScreenPos();  // before ItemImage()

    if (!stack.empty())
    {
        Imw::ItemImage(stack.item(), size);
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

            if (g_InventoryHoldingItemStack.empty() && !stack.empty())
            {
                stack.moveTo(g_InventoryHoldingItemStack);
            }
            else if (!g_InventoryHoldingItemStack.empty() && stack.empty())
            {
                g_InventoryHoldingItemStack.moveTo(stack);
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



static vkx::Image* _LoadCachedTex(const std::string& p) {
    static std::map<std::string, vkx::Image*> _Cache;
    auto& it = _Cache[p];
    if (!it) {
        it = Loader::LoadImage(p);
    }
    return it;
}


void Imw::Editor::ShowToolbar(bool* _open)
{
    ImGui::Begin("MASH", _open, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

    static bool s_Horiz = true;
    static float
        s_Size = 40,
        s_Border = 0,
        s_FramePadding = 1,
        s_ItemSpacing = 1;

#define _ET_TOOLBAR_BTN(id) ImGui::ImageButton(id, Imgui::mapImage(_LoadCachedTex(id)->imageView), {s_Size, s_Size}); if (s_Horiz) { ImGui::SameLine();}

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {s_FramePadding, s_FramePadding});
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {s_ItemSpacing, s_ItemSpacing});
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, s_Border);

    _ET_TOOLBAR_BTN("gui/geo/sel.png");
    _ET_TOOLBAR_BTN("gui/geo/mov.png");
    _ET_TOOLBAR_BTN("gui/geo/mov-all.png");
    _ET_TOOLBAR_BTN("gui/geo/rot.png");
    _ET_TOOLBAR_BTN("gui/geo/scl.png");
    _ET_TOOLBAR_BTN("gui/geo/bnd.png");

    ImGui::Dummy({4, 4}); if (s_Horiz) { ImGui::SameLine(); }

    _ET_TOOLBAR_BTN("gui/geo/sel-rect.png");
    _ET_TOOLBAR_BTN("gui/geo/sel-cir.png");
    _ET_TOOLBAR_BTN("gui/geo/sel-cus.png");

    ImGui::Dummy({4, 4}); if (s_Horiz) { ImGui::SameLine(); }

    _ET_TOOLBAR_BTN("gui/geo/g-cube.png");
    _ET_TOOLBAR_BTN("gui/geo/g-sph.png");
    _ET_TOOLBAR_BTN("gui/geo/g-col.png");
    _ET_TOOLBAR_BTN("gui/geo/g-tor.png");
    _ET_TOOLBAR_BTN("gui/geo/g-cone.png");
    _ET_TOOLBAR_BTN("gui/geo/g-plane.png");

    ImGui::Dummy({4, 4}); if (s_Horiz) { ImGui::SameLine(); }

    _ET_TOOLBAR_BTN("gui/geo/grab.png");
    _ET_TOOLBAR_BTN("gui/geo/brush.png");
    _ET_TOOLBAR_BTN("gui/geo/pencil.png");
    _ET_TOOLBAR_BTN("gui/geo/fill.png");
    _ET_TOOLBAR_BTN("gui/geo/erase.png");
    _ET_TOOLBAR_BTN("gui/geo/pik.png");
    _ET_TOOLBAR_BTN("gui/geo/fing.png");
    _ET_TOOLBAR_BTN("gui/geo/stamp.png");
    _ET_TOOLBAR_BTN("gui/geo/cpy.png");

    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        ImGui::OpenPopup("ImwEditorToolbarEdit");
    }
    if (ImGui::BeginPopup("ImwEditorToolbarEdit"))
    {
        ImGui::Checkbox("Horiz", &s_Horiz);
        ImGui::DragFloat("Size", &s_Size);
        ImGui::DragFloat("Border", &s_Border);
        ImGui::DragFloat("FramePadding", &s_FramePadding);
        ImGui::DragFloat("ItemSpacing", &s_ItemSpacing);
        ImGui::EndPopup();
    }

    ImGui::PopStyleVar(3);

    ImGui::End();
}


void Imw::Editor::ShowConsole(bool* _open)
{
    ImGui::Begin("Console", _open);
    ImGui::BeginChild("###MsgTextList", {0, -ImGui::GetFrameHeightWithSpacing()});
    for (auto& str : Imw::Editor::ConsoleMessages) {
        ImGui::Text("%s", str.c_str());
    }
    ImGui::EndChild();
    static char InputBuf[128];

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    if (ImGui::InputText("###MsgBoxInput", InputBuf, std::size(InputBuf),
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


