//
// Created by Dreamtowards on 2023/3/21.
//


static void ShowMainMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("System"))
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
//                    w_NewWorld = true;
                }
                if (ImGui::MenuItem("Open World..")) {
//                    const char* filename = Loader::openFolderDialog("Open World..", "./saves/");  //std::filesystem::current_path().append("/saves/").string().c_str());
//                    if (filename) {
//                        Log::info("Open world: ", filename);
//                        Ethertia::loadWorld(filename);
//                    }
                }

                //ImGui::SeparatorText("Saves");
                ImGui::Separator();
                ImGui::TextDisabled("Saves:");

//                if (Loader::fileExists("saves/"))
//                {
//                    for (const auto& savedir : std::filesystem::directory_iterator("saves/"))
//                    {
////            std::string size_str = Strings::size_str(Loader::calcDirectorySize(savedir.path()));
//
//                        float epoch = std::chrono::duration_cast<std::chrono::seconds>(savedir.last_write_time().time_since_epoch()).count();
//                        if (epoch < 0)  epoch = 0;  // Error on Windows.
//                        std::string time_str = Strings::time_fmt(epoch);
//
//                        auto filename = savedir.path().filename();
//                        if (ImGui::MenuItem((const char*)filename.c_str(), time_str.c_str()))
//                        {
//                            Ethertia::loadWorld(savedir.path().string());
//                        }
//                    }
//                }

                ImGui::EndMenu();
            }

            bool worldvalid = false;//Ethertia::getWorld();
            if (ImGui::MenuItem("Edit World..", nullptr, false, worldvalid)) {}
            if (ImGui::MenuItem("Save World", nullptr, false, worldvalid)) {}

            if (ImGui::MenuItem("Close World", nullptr, false, worldvalid)) {
//                Ethertia::unloadWorld();
            }

            ImGui::Separator();

            if (ImGui::BeginMenu("Settings..")) {

                if (ImGui::MenuItem("Profile")) { Imgui::w_Settings = true; }
                if (ImGui::MenuItem("Graphics")) {}
                if (ImGui::MenuItem("Sounds & Music")) {}
                if (ImGui::MenuItem("Controls")) {}
                if (ImGui::MenuItem("Language")) {}

                ImGui::EndMenu();
            }

            ImGui::MenuItem("Mods", "0 mods loaded");
            ImGui::MenuItem("Resource Packs");

            if (ImGui::MenuItem("About"/*, Ethertia::Version::version_name().c_str())*/)) {}

            ImGui::Separator();

            if (ImGui::MenuItem("Terminate")) {
//                Ethertia::shutdown();
            }
            ImGui::EndMenu();
        }

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

//            ImGui::Checkbox("DbgText", &dbg_Text);
//            ImGui::Checkbox("DbgWorldBasis", &dbg_WorldBasis);
//            ImGui::Checkbox("DbgViewBasis", &dbg_ViewBasis);
//
//            ImGui::Checkbox("DbgAllEntityAABB", &dbg_AllEntityAABB);
//            ImGui::Checkbox("DbgAllChunkAABB", &dbg_AllChunkAABB);
////            ImGui::Checkbox("DbgHitEntityAABB", &GuiDebugV::dbg_CursorPt);
////            ImGui::Checkbox("DbgNearChunkBoundAABB", &GuiDebugV::dbgChunkBoundAABB);
////            ImGui::Checkbox("DbgCursorNearCellsInfo", &GuiDebugV::dbgCursorRangeInfo);
//
//            ImGui::Checkbox("NoChunkSave", &Settings::dbg_NoChunkSave);
//            ImGui::Checkbox("NoChunkLoad", &Settings::dbg_NoChunkLoad);
//
//            ImGui::Checkbox("PauseThread ChunkMeshing", &Dbg::dbg_PauseThread_ChunkMeshing);
//            ImGui::Checkbox("PauseThread ChunkLoad/Gen/Save", &Dbg::dbg_PauseThread_ChunkLoadGenSave);

            ImGui::Separator();

//            ImGui::SliderInt("World GridSize", &g_WorldGrids, 0, 500);
//            ImGui::Checkbox("ViewManipulation Gizmo", &g_GizmoViewManipulation);

            // ImGui::Checkbox("Hit Entity AABB", &gAllChunkAABB);

//            ImGui::Checkbox("ImGui Demo Window", &w_ImGuiDemo);

            ImGui::SeparatorText("Controlling");

//            ImGui::Checkbox("Hit Tracking", &Ethertia::getHitCursor().keepTracking);
//            ImGui::SliderFloat("BrushSize", &Ethertia::getHitCursor().brushSize, 0, 16);
//    ImGui::Checkbox("BlockMode", &Settings::dbgPolyLine);

//            Camera& cam = Ethertia::getCamera();
//            ImGui::SliderFloat("Cam Smooth", &cam.m_Smoothness, 0, 5);
//            ImGui::SliderFloat("Cam Roll", &cam.eulerAngles.z, -3.14, 3.14);
//
//            ImGui::SeparatorText("Rendering");
//            ImGui::Checkbox("PauseWorldRender", &Settings::dbg_PauseWorldRender);
//            ImGui::Checkbox("GBuffers", &dbg_Gbuffer);
//            ImGui::Checkbox("Border/Norm", &Dbg::dbg_EntityGeo);
//            ImGui::Checkbox("HitEntityGeo", &Dbg::dbg_HitPointEntityGeo);
//            ImGui::Checkbox("Polygon Line", &Settings::dbg_PolyLine);
//
//            ImGui::Checkbox("NoVegetable", &Dbg::dbg_NoVegetable);
//
//
//            static float knobVal = 10;
//            ImGuiKnobs::Knob("Test2", &knobVal, -100, 100, 0.1f, "%.1fdB", ImGuiKnobVariant_Tick);

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
//            ImGui::SliderFloat("FOV", &Ethertia::getCamera().fov, 0, 180);
//            ImGui::SliderFloat("ViewDistance", &Settings::s_ViewDistance, 0, 16);
//
//            ImGui::Checkbox("SSAO", &Settings::g_SSAO);
//            ImGui::Checkbox("Shadow Mapping", &Settings::g_ShadowMapping);

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            ImGui::Checkbox("Viewport", &Imgui::w_Viewport);
            ImGui::Checkbox("Entity List", &Imgui::w_EntityList);
            ImGui::Checkbox("Entity Insp", &Imgui::w_EntityInsp);
            ImGui::Checkbox("Console", &Imgui::w_Console);

//            ImGui::Separator();
//            ImGui::Checkbox("Shader Inspect", &Imgui::w_ShaderInsp);
//            ImGui::Checkbox("NodeEditor", &w_NodeEditor);
//            ImGui::Checkbox("TitleScreen", &w_TitleScreen);
//            ImGui::Checkbox("Singleplayer", &w_Singleplayer);
//            ImGui::Checkbox("Settings", &Settings::w_Settings);
//            ImGui::Checkbox("Profiler", &GuiDebugV::dbgDrawFrameProfiler);

            ImGui::Separator();

            static bool ShowHUD = true;
            if (ImGui::MenuItem("HUD", "F1", &ShowHUD)) {

            }
            if (ImGui::MenuItem("Save Screenshot", "F2")) {
//                Controls::saveScreenshot();
            }
//            if (ImGui::MenuItem("Fullscreen", "F11", Ethertia::getWindow().isFullscreen())) {
//                Ethertia::getWindow().toggleFullscreen();
//            }
//            if (ImGui::MenuItem("Controlling Game", ".", Ethertia::isIngame())) {
//                Ethertia::isIngame() = !Ethertia::isIngame();
//            }

            ImGui::EndMenu();
        }


        {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {1, 1});
            ImGui::SameLine(ImGui::GetWindowWidth() - 180 - 100);

            if (ImGui::BeginCombo("cTmp", "Ethertia | Debug")) {
                ImGui::Selectable("Ethertia", true);
                ImGui::Selectable("DedicatedServer", false);
                ImGui::EndCombo();
            }

            float sz = 14;

//            ImGui::ImageButton("gPP", LazyLoadTex("gui/icon/play-prev.png")->pTexId(), {sz,sz});
//
//            if (ImGui::ImageButton("gPlayPause",
//                                   Ethertia::isIngame() ? LazyLoadTex("gui/icon/pause28.png")->pTexId() :
//                                   LazyLoadTex("gui/icon/play28.png")->pTexId(), {sz, sz})) {
//                Ethertia::isIngame() = !Ethertia::isIngame();
//            }
//
//            ImGui::ImageButton("gSettings", LazyLoadTex("gui/icon/cogs28.png")->pTexId(), {sz,sz});
//            ImGui::ImageButton("gSettings2", LazyLoadTex("gui/icon/books.png")->pTexId(), {sz,sz});

            ImGui::PopStyleVar();
        }


        ImGui::EndMenuBar();
    }

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
    ImVec4 _col = Pur;
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

    ImGui::End();
}













static void ShowSettingsWindow()
{
    ImGui::Begin("Settings", &Imgui::w_Settings);

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

//            ImGui::SliderFloat("FOV", &Ethertia::getCamera().fov, 0, 180);
//            if (ImGui::IsItemHovered())
//                ImGui::SetTooltip("Field of View.\nNormal: 70\nQuark Pro: 90");

//            ImGui::Checkbox("Vsync", &Settings::s_Vsync);
//
//            if (Settings::g_SSAO) ImGui::BeginDisabled();
//            ImGui::SliderInt("FPS Limit", &Settings::s_FpsCap, 0, 2000);
//            if (Settings::g_SSAO) ImGui::EndDisabled();
//
//            ImGui::SliderFloat("Chunk Load Distance", &Settings::s_ViewDistance, 0, 12);
//
//
//            ImGui::SeparatorText("Terrain Material Blending");
//
//            ImGui::DragFloat("Texture Scale", &GeometryRenderer::u_MaterialTextureScale, 0.1f);
//            if (ImGui::IsItemHovered())
//                ImGui::SetTooltip("Material Texture Sampling Scale (inv)");


//            ImGui::DragInt("Texture Resolution", &MaterialTextures::TEX_RESOLUTION, 16, 2048);
//            if (ImGui::IsItemHovered())
//                ImGui::SetTooltip("[Reload Required] for bake material texture atlas.");
//
//
//
//
//            ImGui::SeparatorText("SSAO");
//            ImGui::Checkbox("SSAO", &Settings::g_SSAO);
//
//            ImGui::SeparatorText("Shadow Mapping");
//            ImGui::Checkbox("Shadow Mapping", &Settings::g_ShadowMapping);
//            static int g_ShadowResolution = 1024;
//            ImGui::SliderInt("Shadow Depth Map Resolution", &g_ShadowResolution, 128, 2048);
//
//            ImGui::SeparatorText("Bloom");
//            ImGui::Checkbox("Bloom", &Settings::g_SSAO);

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
//                Loader::openURL("./resourcepacks");
            }
        }
        else if (g_CurrPanel==Credits)
        {
//            ImGui::SetWindowFontScale(1.4f);
//            ImGui::Text("%s", Ethertia::Version::name().c_str());
//            ImGui::SetWindowFontScale(1.0f);

            ImGui::Text("Dev: Eldrine Le Prismarine");

            ImGui::SeparatorText("Links");
            ImGui::SmallButton("ethertia.com");


        }
    }
    ImGui::EndChild();

    ImGui::End();
}

















