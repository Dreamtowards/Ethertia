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




// w_: Window
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


#include "im_settings.cpp"




//static ShaderProgram* g_InspShaderProgram = nullptr;



static vkx::Image* LazyLoadTex(const std::string& p) {
    static std::map<std::string, vkx::Image*> _Cache;
    auto& it = _Cache[p];
    if (!it) {
        it = Loader::loadTexture(p);
    }
    return it;
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


#include <ethertia/init/ItemTextures.h>

static void ItemImage(const Item* item, float size = 40, ImDrawList* dl = ImGui::GetWindowDrawList())
{
    float n = Item::REGISTRY.size();
    float i = Item::REGISTRY.getOrderId(item);
    ImVec2 uvMin = {i/n, 0};
    ImVec2 uvSize = {1.0f/n, 1};
    ImVec2 min = ImGui::GetCursorScreenPos();
    dl->AddImage(pTexDesc(ItemTextures::ITEM_ATLAS->imageView), min, min+ImVec2{size, size}, uvMin, uvMin+uvSize);
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


#include "im_insp.cpp"






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

//#define _TOOLBAR_BTN(id) ImGui::ImageButton(id, LazyLoadTex(id)->pTexId(), {40, 40}, {0,1}, {1,0})
//
//    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {1, 1});
//    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {1, 0});
////    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
//
//    _TOOLBAR_BTN("gui/geo/sel.png");
//    ImGui::SameLine();
//    _TOOLBAR_BTN("gui/geo/mov.png");
//    ImGui::SameLine();
//    _TOOLBAR_BTN("gui/geo/mov-all.png");
//    ImGui::SameLine();
//    _TOOLBAR_BTN("gui/geo/rot.png");
//    ImGui::SameLine();
//    _TOOLBAR_BTN("gui/geo/scl.png");
//    ImGui::SameLine();
//    _TOOLBAR_BTN("gui/geo/bnd.png");
//    ImGui::SameLine();
//
//    ImGui::Dummy({4, 0});
//    ImGui::SameLine();
//
//    _TOOLBAR_BTN("gui/geo/sel-rect.png");
//    ImGui::SameLine();
//    _TOOLBAR_BTN("gui/geo/sel-cir.png");
//    ImGui::SameLine();
//    _TOOLBAR_BTN("gui/geo/sel-cus.png");
//    ImGui::SameLine();
//
//    ImGui::Dummy({4, 0});
//    ImGui::SameLine();
//
//    _TOOLBAR_BTN("gui/geo/g-cube.png");
//    ImGui::SameLine();
//    _TOOLBAR_BTN("gui/geo/g-sph.png");
//    ImGui::SameLine();
//    _TOOLBAR_BTN("gui/geo/g-col.png");
//    ImGui::SameLine();
//    _TOOLBAR_BTN("gui/geo/g-tor.png");
//    ImGui::SameLine();
//    _TOOLBAR_BTN("gui/geo/g-cone.png");
//    ImGui::SameLine();
//    _TOOLBAR_BTN("gui/geo/g-plane.png");
//    ImGui::SameLine();
//
//    ImGui::Dummy({4, 0});
//    ImGui::SameLine();
//
//    _TOOLBAR_BTN("gui/geo/grab.png");
//    ImGui::SameLine();
//    _TOOLBAR_BTN("gui/geo/brush.png");
//    ImGui::SameLine();
//    _TOOLBAR_BTN("gui/geo/pencil.png");
//    ImGui::SameLine();
//    _TOOLBAR_BTN("gui/geo/fill.png");
//    ImGui::SameLine();
//    _TOOLBAR_BTN("gui/geo/erase.png");
//    ImGui::SameLine();
//    _TOOLBAR_BTN("gui/geo/pik.png");
//    ImGui::SameLine();
//    _TOOLBAR_BTN("gui/geo/fing.png");
//    ImGui::SameLine();
//    _TOOLBAR_BTN("gui/geo/stamp.png");
//    ImGui::SameLine();
//    _TOOLBAR_BTN("gui/geo/cpy.png");
//    ImGui::SameLine();
//
//
//    ImGui::PopStyleVar(2);

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
            "\n"
            "SYS: {}, {} core, {} endian\n"
            "CPU: {}\n"
            "GPU: {}"
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
            DebugStat::dbg_ChunksSaving,

            Loader::sys_target(),
            std::thread::hardware_concurrency(),
            std::endian::native == std::endian::big ? "big" : "little",
            Loader::cpuid(),
            vkx::ctx().PhysDeviceProperties.deviceName
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







