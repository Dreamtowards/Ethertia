﻿//
// Created by Dreamtowards on 9/8/2023.
//

#include "Imw.h"
#include <imguizmo/ImGuizmo.h>
#include <imgui_internal.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>

#include <stdx/str.h>

#include <ethertia/Ethertia.h>
//#include <ethertia/entity/player/EntityPlayer.h>
#include <ethertia/util/Loader.h>
#include <ethertia/render/Window.h>
#include <ethertia/init/DebugStat.h>
#include <ethertia/world/Chunk.h>


#pragma region Game Viewport Debug

static void ShowDebugTextOverlay()
{
    World* world = Ethertia::GetWorld();
    float dt = Ethertia::getDelta();
    //EntityPlayer* player = Ethertia::getPlayer();
    //btRigidBody* playerRb = player->m_Rigidbody;
    //float meterPerSec = Mth::floor_dn(playerRb->getLinearVelocity().length(), 3);
    //
    //std::string cellInfo = "nil";
    //std::string chunkInfo = "nil";
    //std::string worldInfo = "nil";
    //HitCursor& cur = Ethertia::getHitCursor();
    //
    //
    //if (world)
    //{
    //    worldInfo = std::format("{}. inhabited {}s, daytime {}. seed {}",
    //        world->m_WorldInfo.Name,
    //        world->m_WorldInfo.InhabitedTime,
    //        stdx::daytime(world->getDayTime()),
    //        world->getSeed());
    //    Chunk* hitChunk = world->getLoadedChunk(cur.position);
    //    if (hitChunk) {
    //        chunkInfo = std::format("GenPop: {}, Inhabited: {}s",
    //            hitChunk->m_Populated,
    //            hitChunk->m_InhabitedTime);
    //    }
    //    if (cur.cell) {
    //        Cell* c = cur.cell;
    //        cellInfo = std::format("mtl: {}, dens: {}, meta: {} | DiggingTime: {}",
    //            c->mtl ? c->mtl->getRegistryId() : "nil",
    //            c->density,
    //            (int)c->exp_meta,
    //            cur.cell_breaking_time);
    //    }
    //}
    //
    //
    //std::string str = std::format(
    //    "CamPos: {}, len: {}, spd {}mps {}kph; ground: {}, CollPts {}.\n"
    //    "avg-fps: {}. dt: {}, {}fps\n"
    //    "NumEntityRendered: {}/{}, LoadedChunks: {}\n"
    //    "\n"
    //    "World: {}\n"
    //    "HitChunk: {}\n"
    //    "HitCell: {}\n"
    //    "\n"
    //    "task {}, async {}\n"
    //    "ChunkProvide: {}\n"
    //    "ChunkMeshing: {}\n"
    //    "ChunkSaving:  {}\n"
    //    "\n"
    //    "OS: {}, {} concurrency, {} endian\n"
    //    "CPU: {}\n"
    //    "GPU: {}"
    //    ,
    //    glm::to_string(Ethertia::getCamera().position), Ethertia::getCamera().len,
    //    meterPerSec, meterPerSec * 3.6f,
    //    player->m_OnGround, player->m_NumContactPoints,
    //
    //    Dbg::dbg_FPS, dt, Mth::floor(1.0f / dt),
    //
    //    Dbg::dbg_NumEntityRendered, world ? world->getEntities().size() : 0, world ? world->getLoadedChunks().size() : 0,
    //
    //    worldInfo,
    //    chunkInfo,
    //    cellInfo,
    //
    //    Ethertia::getScheduler().numTasks(), Ethertia::getAsyncScheduler().numTasks(),
    //    DebugStat::dbg_ChunkProvideState ? DebugStat::dbg_ChunkProvideState : "/",
    //    DebugStat::dbg_NumChunksMeshInvalid,
    //    DebugStat::dbg_ChunksSaving,
    //
    //    Loader::os_arch(),
    //    std::thread::hardware_concurrency(),
    //    std::endian::native == std::endian::big ? "big" : "little",
    //    Loader::cpuid(),
    //    (const char*)vkx::ctx().PhysDeviceProperties.deviceName
    //);

    //auto& vp = Ethertia::getViewport();
    //ImGui::RenderText({ vp.x, vp.y + 16 }, str.c_str());
    // 
    // 
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
            ImVec2(vp.right() - 128 - 24, vp.y + 24), ImVec2(128, 128),
            //0x10101010
            0);
    }
    if (Dbg::dbg_WorldHintGrids > 0)
    {
        glm::mat4 iden(1.0f);
        ImGuizmo::DrawGrid(glm::value_ptr(Ethertia::getCamera().matView), glm::value_ptr(Ethertia::getCamera().matProjection),
            glm::value_ptr(iden), (float)Dbg::dbg_WorldHintGrids);
    }

    //if (Dbg::dbg_ChunkMeshedCounter || Dbg::dbg_MeshingChunksAABB)
    //{
    //    DbgShowChunkMeshingAndCounter();
    //}
    if (Dbg::dbg_TextInfo)
    {
        ShowDebugTextOverlay();
    }
    if (Dbg::dbg_WorldBasis)
    {
        float n = 5;
        Imgui::RenderWorldLine({ 0,0,0 }, { n,0,0 }, ImGui::GetColorU32({ 1,0,0,1 }));
        Imgui::RenderWorldLine({ 0,0,0 }, { 0,n,0 }, ImGui::GetColorU32({ 0,1,0,1 }));
        Imgui::RenderWorldLine({ 0,0,0 }, { 0,0,n }, ImGui::GetColorU32({ 0,0,1,1 }));
    }
    if (Dbg::dbg_ViewBasis)
    {
        glm::mat4* pView = &Ethertia::getCamera().matView;
        glm::mat4 camView = *pView;  // backup.
        *pView = glm::mat4(1.0f);  // disable view matrix. of RenderWorldLine
        float n = 0.016f;
        glm::vec3 o = { 0, 0, -0.1 };
        glm::mat3 rot(camView);
        Imgui::RenderWorldLine(o, o + rot * glm::vec3(n, 0, 0), ImGui::GetColorU32({ 1,0,0,1 }));
        Imgui::RenderWorldLine(o, o + rot * glm::vec3(0, n, 0), ImGui::GetColorU32({ 0,1,0,1 }));
        Imgui::RenderWorldLine(o, o + rot * glm::vec3(0, 0, n), ImGui::GetColorU32({ 0,0,1,1 }));
        *pView = camView;  // restore.
    }

    //World* world = Ethertia::getWorld();
    //if (world) {
    //    if (dbg_AllEntityAABB) {
    //        for (Entity* e : world->m_Entities) {
    //            if (Ethertia::getCamera().testFrustum(e->getAABB()))
    //                Imgui::RenderAABB(e->getAABB(), Colors::RED);
    //        }
    //    }
    //    if (dbg_AllChunkAABB) {
    //        world->forLoadedChunks([&](Chunk* chunk) {
    //            Imgui::RenderAABB({ chunk->position, chunk->position + glm::vec3{16.0f} }, Colors::RED);
    //            });
    //    }
    //}

}

#pragma endregion


glm::mat4 MatView_MoveRotate(glm::mat4 view, glm::vec3 moveDelta, float yawDelta, float pitchDelta, float len = 0.1f, glm::vec3 moveAbsDelta = {0, 0, 0})
{
    glm::vec3 right     = glm::vec3(view[0][0], view[1][0], view[2][0]);
    glm::vec3 up        = glm::vec3(view[0][1], view[1][1], view[2][1]);
    glm::vec3 forward   = glm::vec3(view[0][2], view[1][2], view[2][2]);

    //view = glm::translate(view, moveDelta.x * right);
    //view = glm::translate(view, moveDelta.y * up); 
    //view = glm::translate(view, moveDelta.z * forward);

    glm::mat4 invView = glm::inverse(view);

    glm::vec3 eye = glm::vec3(invView[3]);
    glm::vec3 pivot = eye - forward * len;

    glm::mat4 yawMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(yawDelta), {0, 1, 0});
    glm::mat4 pitchMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(pitchDelta), right);
    glm::vec3 newForward = glm::normalize(glm::mat3(yawMatrix) * glm::mat3(pitchMatrix) * forward);

    glm::vec3 newEye = pivot + newForward * len;

    moveDelta = glm::mat3(invView) * moveDelta;
    pivot  += moveDelta + moveAbsDelta;
    newEye += moveDelta + moveAbsDelta;

    return glm::lookAt(newEye, pivot, { 0, 1, 0 });
}

static void _MoveCamera()
{
    float dYaw = 0;
    float dPitch = 0;
    glm::vec3 move{};
    glm::vec3 moveaa{};  // axis align, for mc-like shift space - down up
    float len = 0.1f;

    auto& io = ImGui::GetIO();
    ImVec2 MouseDelta = io.MouseDelta;
    float  MouseWheel = io.MouseWheel;

    float mspd = 0.2f;

    bool keyAltDown = Window::isAltKeyDown();
    bool keyCtrlDown = Window::isCtrlKeyDown();
    bool dragRMB = ImGui::IsMouseDragging(ImGuiMouseButton_Right);
    bool dragMMB = ImGui::IsMouseDragging(ImGuiMouseButton_Middle);

    if (dragRMB || dragMMB)
    {
        if (dragRMB)
        {
            // FPS Rotate
            dYaw += -MouseDelta.x * mspd;
            dPitch += -MouseDelta.y * mspd;
        }
        else if (dragMMB)
        {
            if (keyAltDown)
            {
                // Pivot Rotate
                dYaw += -MouseDelta.x * mspd;
                dPitch += -MouseDelta.y * mspd;
                len = 16;
            }
            else
            {
                // Pin XY
                move.x += -MouseDelta.x * mspd;
                move.y += MouseDelta.y * mspd;
            }
        }

        float dt = Ethertia::getDelta() * 4.0f;
        if (io.KeyCtrl) dt *= 8.0f;

        if (io.KeysDown[ImGuiKey_W]) move.z -= dt;
        if (io.KeysDown[ImGuiKey_S]) move.z += dt;
        if (io.KeysDown[ImGuiKey_A]) move.x -= dt;
        if (io.KeysDown[ImGuiKey_D]) move.x += dt;
        if (io.KeysDown[ImGuiKey_Q]) move.y -= dt;
        if (io.KeysDown[ImGuiKey_E]) move.y += dt;
        if (Window::isShiftKeyDown()) moveaa.y -= dt;
        if (io.KeysDown[ImGuiKey_Space]) moveaa.y += dt;
    }


    if (MouseWheel)
    {
        // Zoom Z
        move.z += -MouseWheel;
    }


    if (dYaw || dPitch || glm::length2(move) || glm::length2(moveaa))
    {
        Camera& cam = Ethertia::getCamera();
        cam.matView = MatView_MoveRotate(cam.matView, move, dYaw, dPitch, len, moveaa);
    }
}


void Imw::Gameplay::ShowGame(bool* _open)
{
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;

    static ImGuiID s_ViewportLastDockId = 0;
    static bool s_RequestSetBackToLastDock = false;  // when just cancel full viewport

    // WorkArea: menubars
    if (Imw::Gameplay::GameFullwindow)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;  // ImGuiWindowFlags_NoDocking
        s_RequestSetBackToLastDock = true;
    } else if (s_RequestSetBackToLastDock) {
        s_RequestSetBackToLastDock = false;
        ImGui::SetNextWindowDockID(s_ViewportLastDockId);
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::Begin("Viewport", _open, windowFlags);
    ImGui::PopStyleVar(2);

    if (ImGui::GetWindowDockID()) {
        s_ViewportLastDockId = ImGui::GetWindowDockID();
    }

    ImVec2 size = Imgui::GetWindowContentSize();
    ImVec2 pos = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
    Imgui::wViewportXYWH = {pos.x, pos.y, size.x, size.y};

    // Viewport Texture
    if (Imw::Gameplay::GameImageView)
    {
        ImGui::Image(Imgui::mapImage(Imw::Gameplay::GameImageView), size);
    }
    ImGui::SetCursorPos({0,0});
    ImGui::InvisibleButton("PreventsGameWindowMove", size);

    ImGuizmo::BeginFrame();
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();
    auto& vp = Ethertia::getViewport();
    ImGuizmo::SetRect(vp.x, vp.y, vp.width, vp.height);



    DrawViewportDebugs();

    if (ImGui::IsWindowHovered() || ImGui::IsWindowFocused())
    {
        _MoveCamera();
    }


    // Hotbar
    //EntityPlayer& player = *Ethertia::getPlayer();
    //int gm = player.getGamemode();
    //if (gm == Gamemode::SURVIVAL || gm == Gamemode::CREATIVE)
    //{
    //    int hotbarSlots = std::min((int)player.m_Inventory.size(), 8);
    //
    //    if (Ethertia::isIngame())
    //    {
    //        player.m_HotbarSlot += Mth::signal(-Window::MouseWheelSum());
    //        player.m_HotbarSlot = Mth::clamp(player.m_HotbarSlot, 0, hotbarSlots);
    //    }
    //
    //    float hotbarSlotSize = 45;
    //    float hotbarSlotGap = 4;
    //    float hotbarWidth = (hotbarSlotSize + hotbarSlotGap) * hotbarSlots - hotbarSlotGap;
    //    const ImVec2 hotbar_min = {vp.x + (vp.width-hotbarWidth)/2,
    //                               vp.y + vp.height - hotbarSlotSize - hotbarSlotGap};
    //    ImVec2 size = {hotbarSlotSize, hotbarSlotSize};
    //    static ImU32 col_bg = ImGui::GetColorU32({0, 0, 0, 0.3});
    //    static ImU32 col_bg_sel = ImGui::GetColorU32({1, 1, 1, 0.5});
    //
    //    // Player Inventory Hotbar
    //    ImVec2 min = hotbar_min;
    //    for (int i = 0; i < hotbarSlots; ++i)
    //    {
    //        ImGui::RenderFrame(min, min+size, i == player.m_HotbarSlot ? col_bg_sel : col_bg);
    //        ItemStack& stack = player.m_Inventory.at(i);
    //
    //        if (!stack.empty())
    //        {
    //            ImGui::SetCursorScreenPos(min);
    //            Imw::ShowItemStack(stack, false, hotbarSlotSize);
    //        }
    //
    //        min.x += hotbarSlotSize + hotbarSlotGap;
    //    }
    //
    //    // Player Health
    //    if (gm == Gamemode::SURVIVAL)
    //    {
    //        float healthWidth = hotbarWidth * 0.42f;
    //        float healthHeight = 4;
    //        static ImU32 col_health_bg = ImGui::GetColorU32({0.3, 0.3, 0.3, 0.6});
    //        static ImU32 col_health = ImGui::GetColorU32({0.8, 0.3, 0.3, 1});
    //        float perc = player.m_Health / 10.0f;
    //
    //        min = hotbar_min + ImVec2(0, -healthHeight - 8);
    //        ImGui::RenderFrame(min, min+ImVec2(healthWidth, healthHeight), col_health_bg);
    //        ImGui::RenderFrame(min, min+ImVec2(healthWidth * perc, healthHeight), col_health);
    //    }
    //}

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
    //HitCursor& cur = Ethertia::getHitCursor();
    //if (gm == Gamemode::SURVIVAL && cur.cell_breaking_time)
    //{
    //    float width = 40;
    //    float h = 4;
    //    ImVec2 min = vpCenter + ImVec2(-width/2, -12);
    //    float perc = cur.cell_breaking_time / Dbg::dbg_CurrCellBreakingFullTime;
    //
    //    static ImU32 col_bg = ImGui::GetColorU32({0.3, 0.3, 0.3, 0.8});
    //    static ImU32 col_fg = ImGui::GetColorU32({0.8, 0.8, 0.8, 1});
    //    ImGui::RenderFrame(min, min+ImVec2(width, h), col_bg);
    //    ImGui::RenderFrame(min, min+ImVec2(width * perc, h), col_fg);
    //}

    ImGui::End();
}



#include <ethertia/item/recipe/Recipe.h>

static void ShowPlayerInventory()
{
    ImGui::Begin("Inventory");

//    static bool tmpInited = false;
//    if (!tmpInited) {
//        tmpInited = true;
//        for (auto& it : Item::REGISTRY) {
//            ItemStack stack(it.second, 10);
//            Ethertia::getPlayer()->m_Inventory.putItemStack(stack);
//        }
//    }
//    
//    const float slot_size = 40;
//    {
//        const float slot_gap = 4;
//        const int row_items = 6;
//        ImGui::BeginChild("InventoryStacks", {(slot_size+slot_gap)*row_items+slot_gap, 0});
//    
//        ImVec2 stacks_min = ImGui::GetCursorScreenPos();
//        ImVec2 min = stacks_min;
//        Inventory& inv = Ethertia::getPlayer()->m_Inventory;
//        for (int i = 0; i < inv.size(); ++i)
//        {
//            ItemStack& stack = inv.at(i);
//    
//            ImGui::SetCursorScreenPos(min);
//            ImGui::Button("###InvStackClick", {slot_size, slot_size});
//    
//            ImGui::SetCursorScreenPos(min);
//            Imw::ShowItemStack(stack, true, slot_size);
//    
//            if ((i+1) % row_items == 0) {
//                min.x = stacks_min.x;
//                min.y += slot_size + slot_gap;
//            } else {
//                min.x += slot_size + slot_gap;
//            }
//        }
//    
//        ImGui::EndChild();
//    }
//    ImGui::SameLine();
//    {
//        ImGui::BeginChild("InventoryRecipes", {190, 0});
//    
//        for (auto& it : Recipe::REGISTRY)
//        {
//            Recipe* recipe = it.second;
//    
//            ImVec2 mark = ImGui::GetCursorScreenPos();
//            ImGui::Button("###RecipeCraft", {180, slot_size});
//            ImGui::SetCursorScreenPos(mark);
//    
//            Imw::ShowItemStack(recipe->m_Result, false, slot_size);
//    
//            for (ItemStack& src_stack : recipe->m_Source)
//            {
//                ImGui::SameLine();
//                Imw::ShowItemStack(src_stack, false, slot_size * 0.6f);
//            }
//    
//    
////            ImGui::Dummy({180, slot_size});
//        }
//    
//        ImGui::EndChild();
//    }

    ImGui::End();
}





void Imw::Gameplay::ShowWorldNew(bool* _open)
{
    ImGui::Begin("New World", _open);

    //static char _WorldName[128];
    //ImGui::InputText("World Name", _WorldName, std::size(_WorldName));
    //std::string save_path = std::format("./saves/{}", _WorldName);
    //ImGui::TextDisabled("Will save as: %s", save_path.c_str());
    //
    //static char _WorldSeed[128];
    //ImGui::InputText("Seed", _WorldSeed, std::size(_WorldSeed));
    //uint64_t seed = WorldInfo::ofSeed(_WorldSeed);
    //ImGui::TextDisabled("Actual u64 seed: %llu", seed);
    //
    //
    //if (ImGui::Button("Create"))
    //{
    //    WorldInfo worldinfo{
    //            .Seed = seed,
    //            .Name = _WorldName
    //    };
    //    Log::info("Creating world '{}' seed {}.", worldinfo.Name, worldinfo.Seed);
    //    Ethertia::LoadWorld(save_path, &worldinfo);
    //    *_open = false;
    //}

    ImGui::End();
}









void Imw::Gameplay::ShowWorldList(bool* _open)
{
    ImGui::Begin("Singleplayer", _open);

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

void Imw::Gameplay::ShowTitleScreen(bool* _open)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
    ImGui::Begin("TitleScreen", _open);
    ImGui::PopStyleVar(2);

    // Background
//    ImGui::Image(Texture::DEBUG->texId_ptr(),
//                 Imgui::GetWindowContentSize(),
//                 {0,1}, {1,0});

    // LeftBottom Version/Stats
    ImGui::SetCursorPosY(ImGui::GetWindowHeight());
    Imgui::TextAlign(std::format("0 mods loaded.\n{}", Ethertia::Version::name()).c_str(),
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
        Imgui::Show(Imw::Gameplay::ShowWorldList);
    }
    ImGui::SetCursorPosX(btnX);
    if (ImGui::Button("Multiplayer", btnSize)) {
        Loader::ShowMessageBox("INFO", "Not available");
    }
    ImGui::SetCursorPosX(btnX);
    if (ImGui::Button("Settings", btnSize)) {
        Imgui::Show(Imw::Settings::ShowSettings);
    }
    ImGui::SetCursorPosX(btnX);
    if (ImGui::Button("Terminate", btnSize)) {
        Ethertia::shutdown();
    }


    ImGui::End();
}


// draw AABB Box for InvalidMeshChunks (To be mesh), and draw Mesh Counter for each chunk indicates how many times the chunk had meshed.
//static void DbgShowChunkMeshingAndCounter()
//{
//    Ethertia::getWorld()->forLoadedChunks([](Chunk* c)
//                                          {
//                                              if (Dbg::dbg_MeshingChunksAABB && c->m_MeshingState != Chunk::MESH_VALID)
//                                              {
//                                                  Imgui::RenderAABB(c->chunkpos(), c->chunkpos()+glm::vec3(16),
//                                                                    c->m_MeshingState == Chunk::MESH_INVALID ?  // MeshInvalid or Meshing
//                                                                    ImGui::GetColorU32({0.2, 0, 0, .5}) :
//                                                                    ImGui::GetColorU32({1, 0, 0, 1}));
//                                              }
//
//                                              if (Dbg::dbg_ChunkMeshedCounter)
//                                              {
//                                                  glm::vec3 pTextW = c->chunkpos() + glm::vec3(8.0f);
//                                                  glm::vec2 pTextS;
//                                                  if (Imgui::CalcViewportWorldpos(pTextW, pTextS))
//                                                  {
//                                                      ImGui::RenderText({pTextS.x, pTextS.y}, Strings::fmt("Meshed x{}", c->dbg_MeshCounter).c_str());
//                                                  }
//                                              }
//                                          });
//
//
//}
