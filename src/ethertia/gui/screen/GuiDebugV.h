//
// Created by Dreamtowards on 2022/12/30.
//

#ifndef ETHERTIA_GUIDEBUGV_H
#define ETHERTIA_GUIDEBUGV_H


#include <ethertia/gui/GuiCommon.h>

#include <ethertia/render/Camera.h>
#include <ethertia/render/renderer/EntityRenderer.h>
#include <ethertia/world/World.h>
#include <ethertia/entity/player/EntityPlayer.h>
#include <ethertia/util/MemoryTrack.h>
#include <ethertia/render/chunk/proc/ChunkMeshProc.h>
#include <ethertia/render/chunk/proc/ChunkGenProc.h>

class GuiDebugV : public GuiCollection
{
public:
    DECL_Inst_(GuiDebugV);

    inline static bool dbgText = true;
    inline static bool dbgPolyLine = false;

    inline static bool dbgBasis = true;
    inline static bool dbgWorldBasis = true;
    inline static bool dbgAllEntityAABB = false;
    inline static bool dbgChunkAABB = false;
    inline static bool dbgChunkBoundAABB = false;

    inline static bool dbgCursorRangeInfo = false;

    inline static bool dbgGBuffers = false;

    inline static float dbgLastDrawTime = 0;

    inline static bool dbgDrawFrameProfiler = true;


    inline static bool g_BlockMode = false;

    inline static bool dbg_CursorPt = false;

    Gui* optsGui = nullptr;

    GuiDebugV()
    {
        setWidth(Inf);
        setHeight(Inf);

        GuiStack* menubar = new GuiStack(GuiStack::D_HORIZONTAL);
        menubar->setWidth(Inf);
        menubar->setHeight(16);
        menubar->addDrawBackground(Colors::WHITE20);

        RenderEngine* rde = Ethertia::getRenderEngine();
        Camera* cam = Ethertia::getCamera();



        {
            GuiStack* opts = new GuiStack(GuiStack::D_VERTICAL, 4);
            opts->addDrawBackground(Colors::BLACK40);
            opts->setWidth(140);

            opts->addGui(newLabel("Rendering"));
            opts->addGui(new GuiSlider("FOV", 15, 165, &rde->fov, 5.0f));

            opts->addGui(new GuiSlider("View Distance", 0, 16, &rde->viewDistance, 1.0f));

            opts->addGui(new GuiSlider("Brush Size", 0, 16, &Ethertia::getBrushCursor().brushSize, 0.2f));


            opts->addGui(newLabel("Debug Geo."));

            {
                GuiButton* btnReloadShaders = new GuiButton("Reload Shaders");
                opts->addGui(btnReloadShaders);
                btnReloadShaders->addOnClickListener([](OnReleased* e) {
                    delete Ethertia::getRenderEngine()->entityRenderer;
                    Ethertia::getRenderEngine()->entityRenderer = new EntityRenderer();
                });
            }
            {
                GuiButton* btnRemeshAllChunks = new GuiButton("Remesh Chunks");
                opts->addGui(btnRemeshAllChunks);
                btnRemeshAllChunks->addOnClickListener([](OnReleased* e) {
                    for (auto it : Ethertia::getWorld()->getLoadedChunks()) {
                        it.second->requestRemodel();
                    }
                });
            }
            {
                GuiButton* btnClearProfilerData = new GuiButton("Reset Prof");
                opts->addGui(btnClearProfilerData);
                btnClearProfilerData->addOnClickListener([](OnReleased* e) {
                    Ethertia::getProfiler().laterClearRootSection();

                    ChunkMeshProc::gp_MeshGen.laterClearRootSection();
                    ChunkGenProc::gp_ChunkGen.laterClearRootSection();
                });
            }

            opts->addGui(new GuiCheckBox("Debug TextInf", &dbgText));
            opts->addGui(new GuiCheckBox("Basis", &dbgBasis));
            opts->addGui(new GuiCheckBox("World Basis", &dbgWorldBasis));
            opts->addGui(new GuiCheckBox("Rendered Entity AABB", &RenderEngine::dbg_RenderedEntityAABB));
            opts->addGui(new GuiCheckBox("All Entity AABB", &dbgAllEntityAABB));
            opts->addGui(new GuiCheckBox("Loaded Chunk AABB", &dbgChunkAABB));
            opts->addGui(new GuiCheckBox("Chunk Bound AABB", &dbgChunkBoundAABB));
            opts->addGui(new GuiCheckBox("BlockMode", &g_BlockMode));
            opts->addGui(new GuiCheckBox("CursorPoint", &dbg_CursorPt));
            opts->addGui(new GuiCheckBox("HitPtGeo", &RenderEngine::dbg_HitPointEntityGeo));

            opts->addGui(new GuiCheckBox("GBuffers", &dbgGBuffers));
            opts->addGui(new GuiCheckBox("Norm & Border", &RenderEngine::dbg_EntityGeo));
            opts->addGui(new GuiCheckBox("glPoly Line", &dbgPolyLine));

            opts->addGui(new GuiCheckBox("R/No Vegetable", &RenderEngine::dbg_NoVegetable));
            opts->addGui(new GuiCheckBox("D/Frame Profiler", &dbgDrawFrameProfiler));
            opts->addGui(new GuiCheckBox("D/Chunk Unload", &ChunkMeshProc::dbg_ChunkUnload));

            opts->addGui(new GuiSlider("Cam Smth", 0, 5, &cam->smoothness, 0.5f));
            opts->addGui(new GuiSlider("Cam Roll", -Mth::PI, Mth::PI, &cam->eulerAngles.z));

            opts->addGui(new GuiSlider("Fog Density", 0, 0.2f, &EntityRenderer::fogDensity, 0.001f));
            opts->addGui(new GuiSlider("Fog Gradient", 0, 5, &EntityRenderer::fogGradient, 0.01f));


            opts->addGui(newLabel("World"));

            opts->addGui(new GuiSlider("D/var0", -2, 2, &EntityRenderer::debugVar0, 0.01f));
            opts->addGui(new GuiSlider("D/var1", 0, 4, &EntityRenderer::debugVar1, 0.01f));
            opts->addGui(new GuiSlider("D/var2", 0, 256, &EntityRenderer::debugVar2, 0.1f));

            opts->addGui(new GuiCheckBox("Brush Tracking", &Ethertia::getBrushCursor().keepTracking));
            opts->addGui(new GuiCheckBox("Brush pInfo", &dbgCursorRangeInfo));


            GuiScrollBox* scrollbox = new GuiScrollBox();
            scrollbox->setContent(opts);
            scrollbox->setWidth(150);
            scrollbox->setHeight(400);

            addGui(optsGui=new GuiAlign(Inf, Inf,
                                        scrollbox,
                                        Inf, 16+8, 8, 8));
        }


        {
            GuiPopupMenu* mProfiler = newMenu(menubar, "Profiler");
        }

        {
            GuiPopupMenu* mRendering = newMenu(menubar, "Rendering");

        }

        {
            GuiPopupMenu* mEntity = newMenu(menubar, "Entity");
            mEntity->addMenu(new GuiButton("New"));
            mEntity->addMenu(new GuiButton("Modify"));
        }

        {
            GuiPopupMenu* mWorld = newMenu(menubar, "World");

            mWorld->addMenu(new GuiButton("Load"));
            mWorld->addMenu(new GuiButton("Unload"));
        }

        {
            GuiButton* btnOpts;
            GuiPopupMenu* mOpts = newMenu(menubar, "Opts", &btnOpts);

            btnOpts->addOnClickListener([&](OnReleased* e) {
                Log::info("opt");
                optsGui->setVisible(!optsGui->isVisible());
            });
        }

        addGui(menubar);
    }

    void implDraw() override {

        RenderEngine* rde = Ethertia::getRenderEngine();
        EntityPlayer* player = Ethertia::getPlayer();
        World* world = Ethertia::getWorld();

        if (dbgBasis) {
            rde->renderDebugBasis();
        }

        if (dbgWorldBasis) {
            rde->renderDebugWorldBasis();
        }

        glPolygonMode(GL_FRONT_AND_BACK, dbgPolyLine ? GL_LINE : GL_FILL);

        if (dbgText && world) {
            float dt = Ethertia::getDelta();
            static std::string dbg_s;
            if (span_crossed(dbgLastDrawTime, Ethertia::getPreciseTime(), 0.1f)) {
                std::string chunkInfo = "nil";
                Chunk* pointingChunk = nullptr;
                std::string cellInfo = "nil";
                BrushCursor& cur = Ethertia::getBrushCursor();
                if (world && cur.hit) {
                    pointingChunk = world->getLoadedChunk(cur.position);
                    Cell& c = world->getCell(cur.position - cur.normal*0.1f);
                    cellInfo = Strings::fmt("mtl: {}, dens: {}, meta: {}", c.mtl ? c.mtl->getRegistryId() : "nil", c.density, c.exp_meta);
                }
                if (pointingChunk) {
                    chunkInfo = Strings::fmt("GenPop: {}, Inhabited: {}s",
                                             pointingChunk->m_Populated,
                                             pointingChunk->m_InhabitedTime);
                }


                btRigidBody* playerRb = Ethertia::getPlayer()->m_Rigidbody;
                float meterPerSec = Mth::floor_dn(playerRb->getLinearVelocity().length(), 3);
                dbg_s = Strings::fmt(
                        "cam p: {}, len: {}, spd {}mps {}kph. ground: {}, CPs {}.\n"
                        "Entity: {}/{}, LoadedChunks: {}\n"
//                        "ChunkGen {}\n"
//                        "ChunkMesh{}\n"
//                        "ChunkEmit{}\n"
//                        "ChunkSave{}\n"
//                        "ChunkLoad{}\n"
                        "task {}, async {}\n"
                        "dt: {}, {}fps\n"
                        "mem: {}, alloc {}, freed: {}\n"
                        "ChunkHit: {}\n"
                        "CellHit: {}\n"
                        "World {}, {}s #{}. seed {}",
                        glm::to_string(Ethertia::getCamera()->position).substr(3), Ethertia::getCamera()->len, meterPerSec, meterPerSec * 3.6f, player->m_OnGround, player->m_NumContactPoints,
                        world ? rde->dbg_NumEntityRendered : 0, world ? world->getEntities().size() : 0, world ? world->getLoadedChunks().size() : 0,
//                        ChunkProcStat::GEN.str(),
//                        ChunkProcStat::MESH.str(),
//                        ChunkProcStat::EMIT.str(),
//                        ChunkProcStat::SAVE.str(),
//                        ChunkProcStat::LOAD.str(),
                        Ethertia::getScheduler()->numTasks(), Ethertia::getAsyncScheduler()->numTasks(),
                        dt, Mth::floor(1.0f/dt),
                        Strings::size_str(MemoryTrack::g_MemoryPresent()), Strings::size_str(MemoryTrack::g_MemoryAllocated), Strings::size_str(MemoryTrack::g_MemoryFreed),
                        chunkInfo,
                        cellInfo,
                        world->m_Name, world->m_InhabitedTime, Strings::daytime(world->m_DayTime), world->m_Seed
                );
            }
            Gui::drawString(0, 32, dbg_s, Colors::WHITE, 16, {0,0}, false);
        }

        if (dbgAllEntityAABB) {
            for (Entity* e : Ethertia::getWorld()->getEntities()) {
                AABB aabb = e->getAABB();
                rde->renderLineBox(aabb.min, aabb.max - aabb.min, Colors::RED);
            }
        }


        if (dbgChunkAABB) { using glm::vec3;
            world->forLoadedChunks([&](Chunk* chunk){
                vec3 pos = chunk->position;
                rde->renderLineBox(pos, vec3(16), Colors::RED);
            });
        }

        if (dbgChunkBoundAABB) { using glm::vec3;
            int n = 1;
            vec3 cpos = Mth::floor(Ethertia::getCamera()->position, 16);
            for (int dx = -n; dx <= n; ++dx) {
                for (int dy = -n; dy <= n; ++dy) {
                    for (int dz = -n; dz <= n; ++dz) {
                        vec3 d(dx, dy, dz);
                        vec3 p = cpos + d * 16.0f;
                        rde->renderLineBox(p, vec3(16), Colors::RED);
                    }
                }
            }
        }

        if (dbg_CursorPt) {
            BrushCursor& cur = Ethertia::getBrushCursor();

            Ethertia::getRenderEngine()->drawLine(cur.position, cur.normal, Colors::GREEN);

            Entity* selEntity = Ethertia::getBrushCursor().hitEntity;
            if (selEntity) {
                AABB aabb = selEntity->getAABB();
                rde->renderLineBox(aabb.min, aabb.max - aabb.min, Colors::YELLOW);
            }
        }

        if (dbgCursorRangeInfo) {
            drawCursorRangeInfo();
        }

        if (dbgGBuffers) {
            auto* gbuffer = rde->gbuffer;

            float h = Gui::maxHeight() / 6;
            float w = h * 1.5f;

            Gui::drawRect(0, 0, w, h, {
                .tex = gbuffer->texColor[0],
                .channel_mode = Gui::DrawRectArgs::C_RGB
            });
            Gui::drawRect(0, h, w, h, {
                    .tex = gbuffer->texColor[0],
                    .channel_mode = Gui::DrawRectArgs::C_AAA
            });

            Gui::drawRect(0, h*2, w, h, gbuffer->texColor[1]);

            Gui::drawRect(0, h*3, w, h, {
                .tex = gbuffer->texColor[2],
                .channel_mode = Gui::DrawRectArgs::C_RGB
            });
            Gui::drawRect(0, h*4, w, h, {
                .tex = gbuffer->texColor[2],
                .channel_mode = Gui::DrawRectArgs::C_AAA
            });
        }


//    Gui::drawWorldpoint(player->intpposition, [](glm::vec2 p) {
//        Gui::drawRect(p.x, p.y, 4, 4, Colors::RED);
//    });

        if (dbgDrawFrameProfiler) {
            float w = Gui::maxWidth() * 0.65f,
                  h = 16 * 7;
            float x = 0,
                  y = Gui::maxHeight() - h;

            Gui::drawRect(x, y, w, h, Colors::BLACK20);

            const Profiler::Section& sec = Ethertia::getProfiler().GetRootSection();

            // Standard FPS Time.
            Gui::drawRect(x, y, (1.0 / RenderEngine::fpsCap) / sec._avgTime * w, h, Colors::alpha(Colors::GREEN, 0.1f));

            drawProfilerSection(sec, x, y+h-16, w, sec.sumTime);

            {
                h = 16*3;
                y -= h + 16;

                const Profiler::Section& sec = ChunkMeshProc::gp_MeshGen.GetRootSection();

                drawProfilerSection(sec, x, y+h-16, w, sec.sumTime);
            }
            {
                h = 16*3;
                y -= h + 16;

                const Profiler::Section& sec = ChunkGenProc::gp_ChunkGen.GetRootSection();

                drawProfilerSection(sec, x, y+h-16, w, sec.sumTime);
            }
        }

        dbgLastDrawTime = Ethertia::getPreciseTime();
    }


    static bool span_crossed(float a, float b, float span) {
        return Mth::floor(a / span) != Mth::floor(b / span);
    }

    GuiText* newLabel(const std::string& s) {
        GuiText* l = new GuiText(s);
        l->addDrawBackground(Colors::BLACK40);
        l->setWidth(Inf);
        return l;
    }

    GuiPopupMenu* newMenu(GuiCollection* p, const std::string& text, GuiButton** _btn = nullptr) {
        GuiButton* btnMenu = new GuiButton(text);
        btnMenu->setHeight(Inf);
        p->addGui(btnMenu);

        if (_btn) *_btn = btnMenu;

        GuiPopupMenu* m = new GuiPopupMenu();
        btnMenu->addOnPressedListener([m, btnMenu](OnPressed* e) {
            if (!m->getParent()) {
                m->show(btnMenu->getX(), btnMenu->getY() + btnMenu->getHeight());
            } else {
                m->hide();
            }
        });
        return m;
    }



    static void drawCursorRangeInfo() {

        glm::vec3 center(glm::floor(Ethertia::getBrushCursor().position));

        int n = 2;
        for (int rx = -n; rx <= n; ++rx) {
            for (int ry = -n; ry <= n; ++ry) {
                for (int rz = -n; rz <= n; ++rz) {
                    glm::vec3 p = center + glm::vec3(rx, ry, rz);

                    Cell& c = Ethertia::getWorld()->getCell(p);
                    Gui::drawWorldpoint(p, [=](glm::vec2 sp) {
                        Gui::drawString(sp.x, sp.y, c.mtl->getRegistryId()+"/"+std::to_string(c.density));
                    });
                }
            }
        }

        for (int ry = -n; ry <= n; ++ry) {
            for (int rz = -n; rz <= n; ++rz) {
                Ethertia::getRenderEngine()->drawLine(center + glm::vec3(-n, ry, rz), glm::vec3(2*n, 0, 0), Colors::GRAY);
            }
            for (int rx = -n; rx <= n; ++rx) {
                Ethertia::getRenderEngine()->drawLine(center + glm::vec3(rx, ry, -n), glm::vec3(0, 0, 2*n), Colors::GRAY);
            }
        }
        for (int rx = -n; rx <= n; ++rx) {
            for (int rz = -n; rz <= n; ++rz) {
                Ethertia::getRenderEngine()->drawLine(center + glm::vec3(rx, -n, rz), glm::vec3(0, 2*n, 0), Colors::GRAY);
            }
        }
    }




    static float drawProfilerSection(const Profiler::Section& sec, float x, float y, float w, double w_time) {
        const float SEC_H = 16;

        double sec_time = sec.sumTime;
        double sec_width = (sec_time / w_time) * w;
        glm::vec4 color = Colors::ofRGB(std::hash<std::string>()(sec.name) * 256);
        Gui::drawRect(x, y, sec_width, SEC_H, color);
        Gui::drawString(x, y, sec.name);  // Section Name
        if (sec_width > 200) {
            Gui::drawString(x+sec_width, y, Strings::fmt("{}ms@{}", sec._avgTime * 1000.0, sec.numExec), Colors::WHITE80, 12, {-1.0, 0.0});
        }

        float dx = 0;
        for (const Profiler::Section& sub_sec : sec.sections)
        {
            dx += drawProfilerSection(sub_sec, x+dx, y-SEC_H, sec_width, sec_time);
        }

        if (!Ethertia::isIngame() && Gui::isCursorOver(x, y, sec_width, SEC_H)) {
            float tex_h = 16 * 6;
            Gui::drawRect(x, y-tex_h, sec_width, tex_h, Colors::BLACK80);
            Gui::drawString(x, y-tex_h, sec.name, color);
            Gui::drawString(x, y-tex_h, Strings::fmt("\n"
                                                     "avg {}ms\nlas {}ms\nsum {}ms\nexc {}\n%p  {}",
                                                     sec._avgTime * 1000.0, sec._lasTime * 1000.0, sec.sumTime * 1000.0, sec.numExec, sec.parent ? sec.sumTime / sec.parent->sumTime : Mth::NaN));
//             Gui::drawRect(x, y-tex_h+16, sec_width, tex_h-16, Colors::BLACK10);  // tmp make text gray
        }

        return sec_width;
    }

};

#endif //ETHERTIA_GUIDEBUGV_H
