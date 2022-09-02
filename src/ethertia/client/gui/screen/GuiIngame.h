//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_GUIINGAME_H
#define ETHERTIA_GUIINGAME_H

#include <ethertia/client/gui/GuiStack.h>
#include <ethertia/client/gui/GuiButton.h>
#include <ethertia/client/gui/GuiPopupMenu.h>
#include <ethertia/client/gui/GuiSlider.h>
#include "../GuiCollection.h"

#include <ethertia/client/render/Camera.h>
#include <ethertia/client/render/renderer/ChunkRenderer.h>
#include <ethertia/client/gui/GuiCheckBox.h>
#include <ethertia/client/gui/GuiAlign.h>

#include <ethertia/world/World.h>

class GuiIngame : public GuiCollection
{
public:
    inline static GuiIngame* INST;

    inline static bool dbgText = true;
    inline static bool dbgPolyLine = false;

    inline static bool dbgBasis = true;
    inline static bool dbgWorldBasis = true;

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

            opts->addGui(new GuiCheckBox("Debug TextInf", &dbgText));

            opts->addGui(new GuiSlider("FOV", 15, 165, &rde->fov, 5.0f));

            opts->addGui(new GuiSlider("Camera Smoothness", 0, 5, &cam->smoothness, 0.5f));
            opts->addGui(new GuiSlider("Camera Roll", -Mth::PI, Mth::PI, &cam->eulerAngles.z));

            opts->addGui(new GuiSlider("Fog Density", 0, 0.2f, &rde->chunkRenderer->fogDensity, 0.001f));
            opts->addGui(new GuiSlider("Fog Gradient", 0, 5, &rde->chunkRenderer->fogGradient, 0.01f));


            opts->addGui(new GuiCheckBox("Norm & Border", &rde->debugChunkGeo));

            opts->addGui(new GuiCheckBox("glPoly Line", &dbgPolyLine));

            opts->addGui(new GuiCheckBox("Basis", &dbgBasis));

            opts->addGui(new GuiCheckBox("World Basis", &dbgWorldBasis));

            opts->addGui(new GuiSlider("View Distance", 0, 16, &rde->viewDistance, 1.0f));

            opts->addGui(new GuiSlider("BrushCursor Size", 0, 16, &Ethertia::getBrushCursor().size, 0.2f));
            opts->addGui(new GuiCheckBox("BrushCursor KeepTracking", &Ethertia::getBrushCursor().keepTracking));

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


        if (dbgBasis)
            Ethertia::getRenderEngine()->renderDebugBasis();

        if (dbgWorldBasis)
            Ethertia::getRenderEngine()->renderDebugWorldBasis();

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

        GuiCollection::onDraw();
    }
};

#endif //ETHERTIA_GUIINGAME_H
