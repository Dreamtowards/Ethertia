//
// Created by Dreamtowards on 9/8/2023.
//



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

    // Viewport Texture
    if (Ethertia::getWorld())
    {
        ImGui::Image(pTexDesc(RenderEngine::g_ComposeView), size);
    }
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
            player.m_HotbarSlot += Mth::signal(-Window::MouseWheelSum());
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


#include <imgui_internal.h>







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
