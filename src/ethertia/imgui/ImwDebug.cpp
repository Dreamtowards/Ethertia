//
// Created by Dreamtowards on 9/8/2023.
//

#include "Imw.h"

#include <ethertia/Ethertia.h>
#include <ethertia/entity/Entity.h>

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



