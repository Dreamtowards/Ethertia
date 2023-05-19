//
// Created by Dreamtowards on 2023/5/14.
//



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

#include <imgui_internal.h>
#include "ImKeymap.cpp"

#include <ethertia/init/KeyBinding.h>
#include <ethertia/world/ChunkLoader.h>

#include <ethertia/render/renderer/deferred/RendererGbuffers.h>

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
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Field of View.\nNormal: 70\nQuark Pro: 90");

//            ImGui::Checkbox("Vsync", &Settings::s_Vsync);
//            if (Settings::s_Vsync) ImGui::BeginDisabled();
            int fpsLimit = Settings::s_Vsync ? 0 : Settings::s_FpsCap;
            ImGui::SliderInt("FPS Limit or Vsync", &fpsLimit, 0, 1000,
                             fpsLimit ? Strings::fmt("{} FPS", fpsLimit).c_str() : "Vsync");
            if (fpsLimit != 0) {
                Settings::s_FpsCap = fpsLimit;
                Settings::s_Vsync = false;
            } else {
                Settings::s_Vsync = true;
            }
//            if (Settings::s_Vsync) ImGui::EndDisabled();


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

#define SeparateRenderSection ImGui::Dummy({0, 18})

            SeparateRenderSection;


            if (ImGui::CollapsingHeader("Terrain Material Rendering"))
            {
                ImGui::DragFloat("Texture Scale", &RendererGbuffer::g_uboFrag.MtlTexScale, 0.1);

                ImGui::DragFloat("Texture Triplanar Blend Pow", &RendererGbuffer::g_uboFrag.MtlTriplanarBlendPow, 0.1);
                if (ImGui::IsItemHovered()) ImGui::SetTooltip("Higher = 'Sharper' Normal Vectors");

                ImGui::DragFloat("Texture Heightmap Blend Pow", &RendererGbuffer::g_uboFrag.MtlHeightmapBlendPow, 0.1);
                if (ImGui::IsItemHovered()) ImGui::SetTooltip("Lower = More Intertexture at Material Blend");

                ImGui::DragInt("Texture Resolution", &MaterialTextures::TEX_RESOLUTION, 16, 2048);
                if (ImGui::IsItemHovered()) ImGui::SetTooltip("[Reload Required] for bake material texture atlas."
                                                              "\n64x  = Great Performance"
                                                              "\n128x = Optimal Performance"
                                                              "\n256x = Low-End Mobile Devices"
                                                              "\n512x = Normal"
                                                              "\n1024x= High-End PC");
            }



            if (ImGui::CollapsingHeader("Volumetric Cloud"))
            {
            }

            if (ImGui::CollapsingHeader("Atmosphere"))
            {
            }

            if (ImGui::CollapsingHeader("Exponential Fog"))
            {
            }


            if (ImGui::CollapsingHeader("SSAO"))
            {
                ImGui::Checkbox("SSAO", &Settings::g_SSAO);
            }

            if (ImGui::CollapsingHeader("Shadow Mapping"))
            {
                ImGui::Checkbox("Shadow Mapping", &Settings::g_ShadowMapping);
                static int g_ShadowResolution = 1024;
                ImGui::SliderInt("Shadow Depth Map Resolution", &g_ShadowResolution, 128, 2048);
            }

            if (ImGui::CollapsingHeader("Bloom"))
            {
            }


//            ImGui::SeparatorText("SSAO");
//            ImGui::Checkbox("SSAO", &Settings::g_SSAO);
//            SeparateRenderSection;

//            ImGui::SeparatorText("Shadow Mapping");
//            ImGui::Checkbox("Shadow Mapping", &Settings::g_ShadowMapping);
//            static int g_ShadowResolution = 1024;
//            ImGui::SliderInt("Shadow Depth Map Resolution", &g_ShadowResolution, 128, 2048);
//            SeparateRenderSection;
//
//            ImGui::SeparatorText("Bloom");
//            ImGui::Checkbox("Bloom", &Settings::g_SSAO);
//            SeparateRenderSection;

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