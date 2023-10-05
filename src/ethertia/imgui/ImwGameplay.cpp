//
// Created by Dreamtowards on 9/8/2023.
//

#include "Imgui.h"
#include "ImwGame.h"
#include "Imw.h"
#include <imgui_internal.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>

#include <stdx/str.h>
#include <stdx/collection.h>

#include <ethertia/Ethertia.h>
#include <ethertia/util/Loader.h>
#include <ethertia/util/Colors.h>
#include <ethertia/render/Window.h>
#include <ethertia/init/DebugStat.h>
#include <ethertia/world/Chunk.h>
#include <ethertia/world/chunk/ChunkSystem.h>

#include <ethertia/imgui/ImwInspector.h>
#include <ethertia/render/RenderEngine.h>


static float s_CameraMoveSpeed = 16;


#pragma region Viewport Ops

static void _ShowDebugText()
{
    Camera& cam = Ethertia::GetCamera();
    World* world = Ethertia::GetWorld();
    float dt = Ethertia::GetDelta();

    std::string strPxScene = "--";
    std::string strWorldInfo = "--";

    //HitCursor& cur = Ethertia::getHitCursor();
    if (world)
    {
        WorldInfo& wi = world->GetWorldInfo();

        int ecsNumCompTypes = 0;
        int ecsNumComps = 0;
        for (const auto& it : world->registry().storage())
        {
            ++ecsNumCompTypes;
            ecsNumComps += it.second.size();
        }

        {
            ChunkSystem& chunksys = world->GetChunkSystem();
            //auto _lock = chunksys.LockRead();

            strWorldInfo = std::format(
                "'{}'; DayTime: {}; Inhabited: {:.2f}s; Seed: {}\n"
                "Entity: {}; components: {}, T: {};\n"
                "Chunk: {} loaded. {} loading, {} meshing, -- saving;",
                wi.Name,
                stdx::daytime(wi.DayTime, true, false),
                wi.TimeInhabited,
                wi.Seed,
                world->registry().size(),
                ecsNumComps,
                ecsNumCompTypes,
                chunksys.ChunksCount(),
                chunksys.m_ChunksLoading.size(),
                chunksys.m_ChunksMeshing.size());
        }

        PxScene& pScene = world->PhysScene();
        strPxScene = std::format(
            "{} rstatic, {} rdynamic, {} constraints",
            pScene.getNbActors(PxActorTypeFlag::eRIGID_STATIC), pScene.getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC), pScene.getNbConstraints());

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
    }

    std::string strRAM;
    {
        uint64_t _PrivateBytes, _WorkingSet, _PhysUsed, _PhysTotal;
        Loader::ram(&_PrivateBytes, &_WorkingSet, &_PhysUsed, &_PhysTotal);

        strRAM = std::format(
            "{} | +dll -pagefile: {}; {} / {}",
            stdx::size_str(_PrivateBytes), stdx::size_str(_WorkingSet),
            stdx::size_str(_PhysUsed), stdx::size_str(_PhysTotal));
    }


    ETPX_CTX;
    std::string strPhysX = std::format(
        "{} shape, {} mtl, {} TriMesh, {} CovMesh;\n"
        "PxScene: {};",
        PhysX.getNbShapes(), PhysX.getNbMaterials(), PhysX.getNbTriangleMeshes(), PhysX.getNbConvexMeshes(), strPxScene);

    static glm::vec3 CamPosLast;
    glm::vec3 CamPosCurr = glm::inverse(cam.matView)[3];  // HeavyCost!!! try make cache if necessary
    glm::vec3 CamPosMoved = CamPosCurr - CamPosLast;
    CamPosLast = CamPosCurr;
    float CamPosMoveSpeedMPS = glm::length(CamPosMoved);
    
    const auto& thread_pool = Ethertia::GetThreadPool();

    // kph = mps*3.6
    std::string str = std::format(
        "cam: pos: {}, spd: {}mps {}kph; edt_spd_fac: {}\n" 
        "fps: sec_avg: {}. instant: {}, delta: {}\n"
        "threadpool: {} working / {} threads, pending tasks: {}\n"
        //"NumEntityRendered: {}/{}, LoadedChunks: {}\n"
        //"plr ground: {}, collide pts: {}\n"
        "\n"
        "World: {}\n"
        "\n"
        "PhysX: {}\n"
        "\n"
        "HitResult: (0, 0, 0); cell: --; chunk: --;\n"
        "\n"
        "OS:  {}, {} concurrency, {}-endian\n"
        "CPU: {}\n"
        "GPU: {}\n"
        "RAM: {}"
        ,
        glm::to_string(CamPosCurr).substr(4),
        CamPosMoveSpeedMPS, CamPosMoveSpeedMPS * 3.6f,
        s_CameraMoveSpeed,
        //player->m_OnGround, player->m_NumContactPoints,
    
        Dbg::dbg_FPS, std::floor(1.0f / dt), dt,

        thread_pool.num_working_threads(), thread_pool.num_threads(), thread_pool.num_tasks(),
    
        //Dbg::dbg_NumEntityRendered, world ? world->getEntities().size() : 0, world ? world->getLoadedChunks().size() : 0,
    
        strWorldInfo,
        //chunkInfo,
        //cellInfo,
        //
        //Ethertia::getScheduler().numTasks(), Ethertia::getAsyncScheduler().numTasks(),
        //DebugStat::dbg_ChunkProvideState ? DebugStat::dbg_ChunkProvideState : "/",
        //DebugStat::dbg_NumChunksMeshInvalid,
        //DebugStat::dbg_ChunksSaving,
        strPhysX,
    
        Loader::os_arch(), std::thread::hardware_concurrency(), std::endian::native == std::endian::big ? "B" : "L",
        Loader::cpuid(),
        (const char*)vkx::ctx().PhysDeviceProperties.deviceName,
        strRAM
    );

    ImGui::SetCursorPos({ 0, 48 });
    ImGui::Text(str.c_str());
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

static void _ShowGizmo(Entity& entity)
{
    auto& compTrans = entity.GetComponent<TransformComponent>();
    glm::mat4 mat = compTrans.Transform;

    auto& guizmoOp = ImwGame::GuizmoOperation;
    auto& guizmoMode = ImwGame::GuizmoMode;

    if (ImGui::IsKeyPressed(ImGuiKey_W)) guizmoOp = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(ImGuiKey_E)) guizmoOp = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(ImGuiKey_R)) guizmoOp = ImGuizmo::SCALE;

    //if (ImGui::RadioButton("Translate", _GizmoOp == ImGuizmo::TRANSLATE))    _GizmoOp = ImGuizmo::TRANSLATE;  ImGui::SameLine();
    //if (ImGui::RadioButton("Rotate",    _GizmoOp == ImGuizmo::ROTATE))       _GizmoOp = ImGuizmo::ROTATE;     ImGui::SameLine();
    //if (ImGui::RadioButton("Scale",     _GizmoOp == ImGuizmo::SCALE))        _GizmoOp = ImGuizmo::SCALE;

    //if (gizmoOp != ImGuizmo::SCALE)
    //{
    //    if (ImGui::RadioButton("World", _GizmoMode == ImGuizmo::WORLD)) _GizmoMode = ImGuizmo::WORLD; ImGui::SameLine();
    //    if (ImGui::RadioButton("Local", _GizmoMode == ImGuizmo::LOCAL)) _GizmoMode = ImGuizmo::LOCAL;
    //}
    //static bool _OpSnap = false;
    //static glm::vec3 _SnapValue{ 0.5 };
    //bool snap = _OpSnap || ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftSuper);
    //if (ImGui::Checkbox("Snap", &snap)) {
    //    _OpSnap = snap;
    //}
    //if (snap) {
    //    ImGui::DragFloat3("Snap value", &_SnapValue[0], 0.5f);
    //}
    //
    //static bool _OpBound = false;
    //static glm::vec3 _BoundSnapValue{ 0.5f };
    //ImGui::Checkbox("Bound", &_OpBound);
    //if (_OpBound) {
    //    ImGui::DragFloat3("Bound Snap value", &_BoundSnapValue[0], 0.5f);
    //}
    //static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
    Camera& cam = Ethertia::GetCamera();

    bool manipulated = ImGuizmo::Manipulate(
        glm::value_ptr(cam.matView),
        glm::value_ptr(cam.matProjection),
        guizmoOp, guizmoMode,
        glm::value_ptr(mat),
        nullptr,  // delta matrix?
        nullptr,  // snap
        /*_OpBound ? bounds : */nullptr,    // bound
        /*_OpBound ? snap : */nullptr);     // boundSnap

    if (manipulated)  // || ImGuizmo::IsUsing()
    {
        compTrans.Transform = mat;
    }

    //ImGuizmo::DrawCubes(
    //    glm::value_ptr(Ethertia::getCamera().matView),
    //    glm::value_ptr(Ethertia::getCamera().matProjection), 
    //    glm::value_ptr(mat), 1);
}

static void _ShowViewportWidgets()
{
    using Gizmos = ImwGame::Gizmos;

    Entity SelectedEntity = ImwInspector::SelectedEntity;

    Camera& cam = Ethertia::GetCamera();
    World* world = Ethertia::GetWorld();

    ImGui::SetCursorPos(ImGui::GetWindowContentRegionMin());
    ImGui::ArrowButton("Menu", ImGuiDir_Down);

    if (ImGui::BeginPopupContextItem(0, ImGuiPopupFlags_MouseButtonLeft))
    {
        static float Snap = 0;

        if (!SelectedEntity) { ImGui::BeginDisabled(); }

        auto& guizmoOp   = ImwGame::GuizmoOperation;
        auto& guizmoMode = ImwGame::GuizmoMode;

        float MenuMaxX = ImGui::GetContentRegionAvail().x;
        int _SnapInputWidth = 100;
        ImGui::PushItemWidth(_SnapInputWidth);
        if (ImGui::RadioButton("Translate", guizmoOp == ImGuizmo::TRANSLATE)) guizmoOp = ImGuizmo::TRANSLATE;   ImGui::SameLine(MenuMaxX - _SnapInputWidth);  ImGui::DragFloat("##SnapPos", &Snap);
        if (ImGui::RadioButton("Rotate",    guizmoOp == ImGuizmo::ROTATE))    guizmoOp = ImGuizmo::ROTATE;      ImGui::SameLine(MenuMaxX - _SnapInputWidth);  ImGui::DragFloat("##SnapRot", &Snap);
        if (ImGui::RadioButton("Scale",     guizmoOp == ImGuizmo::SCALE))     guizmoOp = ImGuizmo::SCALE;       ImGui::SameLine(MenuMaxX - _SnapInputWidth);  ImGui::DragFloat("##SnapScl", &Snap);
        ImGui::PopItemWidth();

        ImGui::Spacing();
        if (ImGui::RadioButton("World", guizmoMode == ImGuizmo::WORLD)) guizmoMode = ImGuizmo::WORLD;
        ImGui::SameLine();  
        if (ImGui::RadioButton("Local", guizmoMode == ImGuizmo::LOCAL)) guizmoMode = ImGuizmo::LOCAL;

        if (!SelectedEntity) { ImGui::EndDisabled(); }

        ImGui::SeparatorText("Camera");

        ImGui::DragFloat("Move Speed", &s_CameraMoveSpeed);

        ImGui::SeparatorText("Gizmos");

        ImGui::Checkbox("View Basis", &Gizmos::ViewBasis);
        ImGui::Checkbox("View Gizmo", &Gizmos::ViewGizmo);
        ImGui::Checkbox("World Basis", &Gizmos::WorldBasis);
        ImGui::DragInt("World Grids", &Gizmos::WorldGrids);

        ImGui::Checkbox("Text Info", &Gizmos::TextInfo);

        ImGui::Separator();

        ImGui::Checkbox("Chunk Load Range", &Gizmos::ChunkLoadRangeAABB);
        ImGui::Checkbox("Chunks Loading Bound", &Gizmos::ChunksLoadingAABB);
        ImGui::Checkbox("Chunks Meshing Bound", &Gizmos::ChunksMeshingAABB);
        ImGui::Checkbox("Chunks Loaded Bound", &Gizmos::ChunksLoadedAABB);

        if (ImGui::Button("Reloa Pipeline"))
        {
            RenderEngine::_ReloadPipeline();
        }

        ImGui::EndPopup();
    }

    if (world && SelectedEntity)
    {
        _ShowGizmo(SelectedEntity);
    }

    if (Gizmos::ViewGizmo)
    {
        static float camLen = 10.0f;
        auto vp = Ethertia::GetViewport();
        ImGuizmo::ViewManipulate(glm::value_ptr(cam.matView), camLen,
            ImVec2(vp.right() - 128 - 24, vp.y + 24), ImVec2(128, 128),
            //0x10101010
            0);
    }
    if (Gizmos::WorldGrids > 0)
    {
        glm::mat4 iden(1.0f);
        ImGuizmo::DrawGrid(
            glm::value_ptr(cam.matView),
            glm::value_ptr(cam.matProjection),
            glm::value_ptr(iden), 
            (float)Gizmos::WorldGrids);
    }

    //if (Dbg::dbg_ChunkMeshedCounter || Dbg::dbg_MeshingChunksAABB)
    //{
    //    DbgShowChunkMeshingAndCounter();
    //}
    if (ImwGame::Gizmos::TextInfo)
    {
        _ShowDebugText();
    }
    if (ImwGame::Gizmos::WorldBasis)
    {
        float n = 5;
        Imgui::RenderWorldLine({ 0,0,0 }, { n,0,0 }, ImGui::GetColorU32({ 1,0,0,1 }));
        Imgui::RenderWorldLine({ 0,0,0 }, { 0,n,0 }, ImGui::GetColorU32({ 0,1,0,1 }));
        Imgui::RenderWorldLine({ 0,0,0 }, { 0,0,n }, ImGui::GetColorU32({ 0,0,1,1 }));
    }
    if (Gizmos::ViewBasis)
    {
        glm::mat4* pView = &cam.matView;
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

    if (world)
    {
        ChunkSystem& chunksys = world->GetChunkSystem();

        if (Gizmos::ChunkLoadRangeAABB)
        {
            glm::vec3 p = chunksys.m_ChunkLoadCenter;
            glm::vec3 ex = glm::vec3{ chunksys.m_TmpLoadDistance.x, chunksys.m_TmpLoadDistance.y, chunksys.m_TmpLoadDistance.x } * 16.0f;
            Imgui::RenderAABB(AABB{p-ex, p+ex+16.0f}, Colors::GRAY_DARK);
        }

        if (Gizmos::ChunksLoadingAABB)
        {
            for (auto& it : chunksys.m_ChunksLoading)
            {
                Imgui::RenderAABB(AABB{ it.first, it.first + 16 }, Colors::GREEN);
            }
        }
        if (Gizmos::ChunksMeshingAABB)
        {
            for (auto& it : chunksys.m_ChunksMeshing)
            {
                Imgui::RenderAABB(AABB{ it.first, it.first + 16 }, Colors::YELLOW);
            }
        }
        if (Gizmos::ChunksLoadedAABB)
        {
            chunksys.ForChunks([](auto cp, auto chunk) {

                Imgui::RenderAABB(AABB{ cp, cp + 16 }, Colors::GRAY);
                return true;
            });
        }
    }

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
    bool dragRMB = ImGui::IsMouseDown(ImGuiMouseButton_Right);
    bool dragMMB = ImGui::IsMouseDown(ImGuiMouseButton_Middle);
    bool dragLMB = ImGui::IsMouseDown(ImGuiMouseButton_Left);

    if (dragRMB || dragMMB)
    {
        if (dragRMB && (dragMMB || dragLMB))
        {
            // Pin XZ
            move.z += -MouseDelta.y * mspd;
            move.x += -MouseDelta.x * mspd;
        }
        else if (dragRMB)
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

        if (io.KeysDown[ImGuiKey_W]) move.z -= 1;
        if (io.KeysDown[ImGuiKey_S]) move.z += 1;
        if (io.KeysDown[ImGuiKey_A]) move.x -= 1;
        if (io.KeysDown[ImGuiKey_D]) move.x += 1;
        if (io.KeysDown[ImGuiKey_Q]) move.y -= 1;
        if (io.KeysDown[ImGuiKey_E]) move.y += 1;
        if (keyCtrlDown) moveaa.y -= 1;
        if (io.KeysDown[ImGuiKey_Space]) moveaa.y += 1;


        s_CameraMoveSpeed += MouseWheel;
        s_CameraMoveSpeed = std::max(0.0f, s_CameraMoveSpeed);

        float spd = Ethertia::GetDelta() * s_CameraMoveSpeed;

        move *= spd;
        moveaa *= spd;

    } 
    else if (MouseWheel && ImGui::IsWindowHovered())
    {
        // Zoom Z
        move.z += -MouseWheel * s_CameraMoveSpeed;
    }


    if (dYaw || dPitch || glm::length2(move) || glm::length2(moveaa))
    {
        Camera& cam = Ethertia::GetCamera();
        cam.matView = MatView_MoveRotate(cam.matView, move, dYaw, dPitch, len, moveaa);
    }
}


#pragma endregion




void ImwGame::ShowGame(bool* _open)
{
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;

    static ImGuiID s_ViewportLastDockId = 0;
    static bool s_RequestSetBackToLastDock = false;  // when just cancel full viewport

    // WorkArea: menubars
    if (ImwGame::IsFullwindow)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize;  // ImGuiWindowFlags_NoDocking
        s_RequestSetBackToLastDock = true;
    } else if (s_RequestSetBackToLastDock) {
        s_RequestSetBackToLastDock = false;
        ImGui::SetNextWindowDockID(s_ViewportLastDockId);
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::Begin("World", _open, windowFlags);
    ImGui::PopStyleVar(2);

    // ImGuizmo: Make Draw to Current Window. otherwise the draw will behind the window.
    ImGuizmo::SetDrawlist();

    if (ImGui::GetWindowDockID()) {
        s_ViewportLastDockId = ImGui::GetWindowDockID();
    }

    ImVec2 viewSize = Imgui::GetWindowContentSize();
    ImVec2 viewPos = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
    ImwGame::Viewport = { viewPos.x, viewPos.y, viewSize.x, viewSize.y };

    // Viewport Image
    if (ImwGame::WorldImageView)
    {
        ImGui::Image(Imgui::mapImage(ImwGame::WorldImageView), viewSize);
    }
    //ImGui::SetCursorPos({0,0});
    //ImGui::InvisibleButton("PreventsGameWindowDragMove", viewSize);



    if (ImGui::IsWindowHovered() || ImGui::IsWindowFocused())
    {
        _MoveCamera();
    }


    _ShowViewportWidgets();




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

    //auto& vp = Ethertia::getViewport();
    const ImVec2 vpCenter = {viewPos.x + viewSize.x/2, viewPos.y + viewSize.y/2};

    // Crosshair
    if (!ImwGame::Gizmos::ViewGizmo)
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


#include <ethertia/world/World.h>

#include <time.h>

static uint64_t _ParseSeed(const char* str)
{
    if (*str == 0)
        return std::time(0);  // unix timestamp in seconds

    char* end;
    uint64_t seed = std::strtoll(str, &end, 10);

    if (end == str)  // parse failed.
    {
        return std::hash<std::string_view>()(str);
    }

    return seed;
}


void ImwGame::ShowWorldNew(bool* _open)
{
    ImGui::Begin("New World", _open);

    static char _WorldName[128];
    ImGui::InputText("World Name", _WorldName, std::size(_WorldName));
    std::string save_path = std::format("./saves/{}", _WorldName);
    ImGui::TextDisabled("Will save as: %s", save_path.c_str());
    
    static char _WorldSeed[128];
    ImGui::InputText("Seed", _WorldSeed, std::size(_WorldSeed));
    uint64_t seed = _ParseSeed(_WorldSeed);
    ImGui::TextDisabled("Actual u64 seed: %llu", seed);
    
    
    if (ImGui::Button("Create"))
    {
        WorldInfo worldinfo{
                .Seed = seed,
                .Name = _WorldName
        };
        Log::info("Creating world '{}' seed {}.", worldinfo.Name, worldinfo.Seed);
        Ethertia::LoadWorld(save_path, &worldinfo);
        *_open = false;
    }

    ImGui::End();
}


void ImwGame::ShowWorldSettings(bool* _open)
{
    ImGui::Begin("World Settings", _open);

    World* world = Ethertia::GetWorld();
    if (!world) {
        ImGui::TextDisabled("World not loaded");
        ImGui::End();
        return;
    }

    ImGui::TextDisabled("World Dir: %s", "some where");
    ImGui::SameLine();
    if (ImGui::Button("Open"))
    {

    }
    
    ImGui::SeparatorText("World Info");
    WorldInfo& wi = world->GetWorldInfo();

    Imgui::InputText("World Name", wi.Name);

    ImGui::InputInt("World Seed", (int*)&wi.Seed);

    ImGui::DragFloat("Inhabited Time", &wi.TimeInhabited);

    ImGui::SliderFloat("DayTime", &wi.DayTime, 0, 1, std::format("{:.5f} {}", wi.DayTime, stdx::daytime(wi.DayTime)).c_str());

    ImGui::DragFloat("DayTimeLength", &wi.DayTimeLength, 1, 1, 86400, std::format("{}s {}", (int)wi.DayTimeLength, stdx::duration(wi.DayTimeLength, false)).c_str());


    ImGui::SeparatorText("Chunk System");
    ChunkSystem& chunksys = world->GetChunkSystem();

    ImGui::DragInt2("Load Distance", &chunksys.m_TmpLoadDistance.x, 0, 10000);

    if (ImGui::Button("ReMesh All Chunks"))
    {
        chunksys.ForChunks([](auto cp, auto chunk) {
        
            chunk->m_NeedRebuildMesh = true;
            return true;
        });
    }

    ImGui::SliderInt("ChunkLoading MaxConcurrent", &chunksys.cfg_ChunkLoadingMaxConcurrent, 0, 20);
    ImGui::SliderInt("ChunkMeshing MaxConcurrent", &chunksys.cfg_ChunkMeshingMaxConcurrent, 0, 20);

    ImGui::End();
}






void ImwGame::ShowWorldList(bool* _open)
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

void ImwGame::ShowTitleScreen(bool* _open)
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
    Imgui::TextAlign(std::format("0 mods loaded.\n{}", Ethertia::GetVersion(true)).c_str(),
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
        Imgui::Show(ImwGame::ShowWorldList);
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
        Ethertia::Shutdown();
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
