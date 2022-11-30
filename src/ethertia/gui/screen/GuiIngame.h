//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_GUIINGAME_H
#define ETHERTIA_GUIINGAME_H

#include "../GuiCollection.h"

#include <ethertia/gui/GuiStack.h>
#include <ethertia/gui/GuiButton.h>
#include <ethertia/gui/GuiPopupMenu.h>
#include <ethertia/gui/GuiSlider.h>
#include <ethertia/gui/GuiCheckBox.h>
#include <ethertia/gui/GuiAlign.h>
#include <ethertia/gui/GuiText.h>

#include <ethertia/render/Camera.h>
#include <ethertia/render/renderer/ChunkRenderer.h>
#include <ethertia/render/renderer/EntityRenderer.h>
#include <ethertia/world/World.h>
#include <ethertia/entity/player/EntityPlayer.h>

class GuiIngame : public GuiCollection
{
public:
    inline static GuiIngame* INST;

    inline static bool dbgText = true;
    inline static bool dbgPolyLine = false;

    inline static bool dbgBasis = true;
    inline static bool dbgWorldBasis = true;
    inline static bool dbgEntityAABB = false;

    inline static bool dbgCursorRangeInfo = false;

    Gui* optsGui = nullptr;

    GuiIngame()
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
            opts->addDrawBackground(Colors::BLACK50);
            opts->setWidth(140);

            opts->addGui(newLabel("Rendering"));
            opts->addGui(new GuiSlider("FOV", 15, 165, &rde->fov, 5.0f));

            opts->addGui(new GuiSlider("Camera Smoothness", 0, 5, &cam->smoothness, 0.5f));
            opts->addGui(new GuiSlider("Camera Roll", -Mth::PI, Mth::PI, &cam->eulerAngles.z));

            opts->addGui(new GuiSlider("Fog Density", 0, 0.2f, &rde->chunkRenderer->fogDensity, 0.001f));
            opts->addGui(new GuiSlider("Fog Gradient", 0, 5, &rde->chunkRenderer->fogGradient, 0.01f));


            opts->addGui(newLabel("Debug Geo."));

            opts->addGui(new GuiCheckBox("Debug TextInf", &dbgText));
            opts->addGui(new GuiCheckBox("Basis", &dbgBasis));
            opts->addGui(new GuiCheckBox("World Basis", &dbgWorldBasis));
            opts->addGui(new GuiCheckBox("Entity AABB", &dbgEntityAABB));

            opts->addGui(new GuiCheckBox("Norm & Border", &rde->debugChunkGeo));
            opts->addGui(new GuiCheckBox("glPoly Line", &dbgPolyLine));

            opts->addGui(newLabel("World"));
            opts->addGui(new GuiSlider("View Distance", 0, 16, &rde->viewDistance, 1.0f));

            opts->addGui(new GuiSlider("BrushCursor Size", 0, 16, &Ethertia::getBrushCursor().size, 0.2f));
            opts->addGui(new GuiCheckBox("BrushCursor KeepTracking", &Ethertia::getBrushCursor().keepTracking));
            opts->addGui(new GuiCheckBox("BrushCursor RangeInfo", &dbgCursorRangeInfo));

            opts->addGui(new GuiSlider("D/var1", 0, 8, &rde->entityRenderer->debugVar1, 0.1f));
            opts->addGui(new GuiSlider("D/var2", 0, 8, &rde->entityRenderer->debugVar2, 0.1f));

            addGui(optsGui=new GuiAlign(1.0f, 0.14f, opts));
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

    void onDraw() override {


        if (dbgBasis) {
            Ethertia::getRenderEngine()->renderDebugBasis();
        }

        if (dbgWorldBasis) {
            Ethertia::getRenderEngine()->renderDebugWorldBasis();
        }

        glPolygonMode(GL_FRONT_AND_BACK, dbgPolyLine ? GL_LINE : GL_FILL);

        if (dbgText) {
            float dt = Ethertia::getDelta();
            std::string dbg_s = Strings::fmt(
                    "p: {}, E: {}/{}\n"
                    "dt/ {}, {}fs\n"
                    "ChunkStat: {}",
                    glm::to_string(Ethertia::getCamera()->position), Ethertia::getRenderEngine()->entitiesActualRendered, Ethertia::getWorld()->getEntities().size(),
                    dt, Mth::floor(1.0f/dt));
            Gui::drawString(0, 32, dbg_s, Colors::WHITE, 16, 0, false);
        }

        if (dbgEntityAABB) {
            EntityPlayer* e = Ethertia::getPlayer();
            AABB aabb = e->getAABB();
            Ethertia::getRenderEngine()->renderLineBox(aabb.min, aabb.max - aabb.min, Colors::RED);
        }

        if (dbgCursorRangeInfo) {
            drawCursorRangeInfo();
        }


//    Gui::drawWorldpoint(player->intpposition, [](glm::vec2 p) {
//        Gui::drawRect(p.x, p.y, 4, 4, Colors::RED);
//    });

        // Center Cursor.
        Gui::drawRect(Gui::maxWidth()/2 -2, Gui::maxHeight()/2 -2,
                      3, 3, Colors::WHITE);

        GuiCollection::onDraw();
    }

    static void drawCursorRangeInfo() {

        glm::vec3 center(glm::floor(Ethertia::getBrushCursor().position));

        int n = 2;
        for (int rx = -n; rx <= n; ++rx) {
            for (int ry = -n; ry <= n; ++ry) {
                for (int rz = -n; rz <= n; ++rz) {
                    glm::vec3 p = center + glm::vec3(rx, ry, rz);

                    MaterialStat& mtl = Ethertia::getWorld()->getBlock(p);
                    Gui::drawWorldpoint(p, [=](glm::vec2 sp) {
                        Gui::drawString(sp.x, sp.y, std::to_string(mtl.id)+"/"+std::to_string(mtl.density));
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
};

#endif //ETHERTIA_GUIINGAME_H
