//
// Created by Dreamtowards on 2023/3/12.
//



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
            "NumEntityRendered: {}/{}, LoadedChunks: {}\n"
            "dt: {}, {}fps\n"
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

            Settings::dbgEntitiesRendered, world ? world->getEntities().size() : 0, world ? world->getLoadedChunks().size() : 0,

            dt, Mth::floor(1.0f/dt),
            worldInfo,
            chunkInfo,
            cellInfo,

            Ethertia::getScheduler().numTasks(), Ethertia::getAsyncScheduler().numTasks(),
            DebugStat::dbg_ChunkProvideState ? DebugStat::dbg_ChunkProvideState : "/",
            DebugStat::dbg_NumChunksMeshInvalid,
            DebugStat::dbg_ChunksSaving

    );

    ImGui::SetNextWindowPos({0, 32});
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0,0});
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking |
                                    ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (ImGui::Begin("DebugTextOverlay", &dbg_Text, window_flags)) {
        ImGui::Text("%s", str.c_str());
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
}

#include <ethertia/gui/Gui.h>

void Dbg_DrawGbuffers(float x, float y)
{
    auto* gbuffer = GeometryRenderer::fboGbuffer;

    float h = 120;
    float w = h * 1.5f;

    Gui::drawRect(x, y, w, h, {
            .tex = gbuffer->texColor[0],
            .channel_mode = Gui::DrawRectArgs::C_RGB
    });
    Gui::drawString(x,y, "Pos");

    Gui::drawRect(x+w, y, w, h, {
            .tex = gbuffer->texColor[0],
            .channel_mode = Gui::DrawRectArgs::C_AAA
    });
    Gui::drawString(x+w,y, "Dep");

    Gui::drawRect(x, y+h, w, h, gbuffer->texColor[1]);
    Gui::drawString(0,h, "Norm");

    Gui::drawRect(x, y+h*2, w, h, {
            .tex = gbuffer->texColor[2],
            .channel_mode = Gui::DrawRectArgs::C_RGB
    });
    Gui::drawString(x,y+h*2, "Albedo");

    Gui::drawRect(x+w, y+h*2, w, h, {
            .tex = gbuffer->texColor[2],
            .channel_mode = Gui::DrawRectArgs::C_AAA
    });
    Gui::drawString(x+w,y+h*2, "Roug");

    Gui::drawRect(x, y+h*3, w, h, {
            .tex = SSAORenderer::fboSSAO->texColor[0],
            .channel_mode = Gui::DrawRectArgs::C_RGB
    });
    Gui::drawString(x,y+h*3, "SSAO");


    Gui::drawRect(x, y+h*4, w, h, {
            .tex = ShadowRenderer::fboDepthMap->texDepth,
            .channel_mode = Gui::DrawRectArgs::C_RGB
    });
    Gui::drawString(x,y+h*4, "Shadow");

}
