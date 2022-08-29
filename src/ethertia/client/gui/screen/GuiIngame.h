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

class GuiIngame : public GuiCollection
{
public:
    inline static GuiIngame* INST;

    inline static bool dbgText = true;
    inline static bool dbgPolyLine = false;

    GuiIngame()
    {
        setWidth(Inf);
        setHeight(Inf);


        GuiStack* menubar = new GuiStack(GuiStack::D_HORIZONTAL);
        menubar->setWidth(Inf);
        menubar->setHeight(16);

        RenderEngine* rde = Ethertia::getRenderEngine();
        Camera* cam = Ethertia::getCamera();
        {
            GuiPopupMenu* mProfiler = newMenu(menubar, "Profiler");
            mProfiler->addMenu(new GuiCheckBox("Memory Profiler", &rde->debugChunkGeo));

            mProfiler->addMenu(new GuiCheckBox("Debug TextInf", &dbgText));
        }

        {
            GuiPopupMenu* mRendering = newMenu(menubar, "Rendering");
            mRendering->addMenu(new GuiSlider("FOV", 15, 165, &rde->fov, 5.0f));

            mRendering->addMenu(new GuiSlider("Camera Smoothness", 0, 5, &cam->smoothness, 0.5f));
            mRendering->addMenu(new GuiSlider("Camera Roll", -Mth::PI, Mth::PI, &cam->eulerAngles.z));

            mRendering->addMenu(new GuiSlider("Fog Density", 0, 0.2f, &rde->chunkRenderer->fogDensity, 0.001f));
            mRendering->addMenu(new GuiSlider("Fog Gradient", 0, 5, &rde->chunkRenderer->fogGradient, 0.01f));

            mRendering->addMenu(new GuiCheckBox("Norm & Border", &rde->debugChunkGeo));

            mRendering->addMenu(new GuiCheckBox("glPoly Line", &dbgPolyLine));

            mRendering->addMenu(new GuiCheckBox("Basis", &rde->debugChunkGeo));

            mRendering->addMenu(new GuiCheckBox("World Basis", &rde->debugChunkGeo));
        }

        {
            GuiPopupMenu* mEntity = newMenu(menubar, "Entity");
            mEntity->addMenu(new GuiButton("New"));
            mEntity->addMenu(new GuiButton("Modify"));
        }

        {
            GuiPopupMenu* mWorld = newMenu(menubar, "World");

            mWorld->addMenu(new GuiSlider("View Distance", 0, 16, &rde->viewDistance, 1.0f));

            mWorld->addMenu(new GuiButton("Load"));
            mWorld->addMenu(new GuiButton("Unload"));
        }

        {
            GuiPopupMenu* mOpts = newMenu(menubar, "Opts");
        }

        menubar->addDrawBackground(Colors::BLACK10);
        addGui(menubar);

    }

    GuiPopupMenu* newMenu(GuiCollection* p, const std::string& text) {
        GuiButton* btnMenu = new GuiButton(text);
        btnMenu->setHeight(Inf);
        p->addGui(btnMenu);

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
