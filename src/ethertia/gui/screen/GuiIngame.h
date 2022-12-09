//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_GUIINGAME_H
#define ETHERTIA_GUIINGAME_H

#include "../GuiCollection.h"
#include "GuiScreenMainMenu.h"

#include <ethertia/gui/GuiStack.h>
#include <ethertia/gui/GuiButton.h>
#include <ethertia/gui/GuiPopupMenu.h>
#include <ethertia/gui/GuiSlider.h>
#include <ethertia/gui/GuiCheckBox.h>
#include <ethertia/gui/GuiAlign.h>
#include <ethertia/gui/GuiText.h>
#include <ethertia/gui/GuiScrollBox.h>

#include <ethertia/render/Camera.h>
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
    inline static bool dbgChunkAABB = false;

    inline static bool dbgCursorRangeInfo = false;

    inline static bool dbgGBuffers = false;

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
            opts->addDrawBackground(Colors::BLACK40);
            opts->setWidth(140);

            opts->addGui(newLabel("Rendering"));
            opts->addGui(new GuiSlider("FOV", 15, 165, &rde->fov, 5.0f));

            opts->addGui(new GuiSlider("Cam Smth", 0, 5, &cam->smoothness, 0.5f));
            opts->addGui(new GuiSlider("Cam Roll", -Mth::PI, Mth::PI, &cam->eulerAngles.z));

            opts->addGui(new GuiSlider("Fog Density", 0, 0.2f, &EntityRenderer::fogDensity, 0.001f));
            opts->addGui(new GuiSlider("Fog Gradient", 0, 5, &EntityRenderer::fogGradient, 0.01f));


            opts->addGui(newLabel("Debug Geo."));

            {
                GuiButton* btnReloadShaders = new GuiButton("Reload Shaders");
                opts->addGui(btnReloadShaders);
                btnReloadShaders->addOnClickListener([](OnReleased* e) {
                    delete Ethertia::getRenderEngine()->entityRenderer;
                    Ethertia::getRenderEngine()->entityRenderer = new EntityRenderer();
                });
            }

            opts->addGui(new GuiCheckBox("Debug TextInf", &dbgText));
            opts->addGui(new GuiCheckBox("Basis", &dbgBasis));
            opts->addGui(new GuiCheckBox("World Basis", &dbgWorldBasis));
            opts->addGui(new GuiCheckBox("Entity AABB", &dbgEntityAABB));
            opts->addGui(new GuiCheckBox("Chunk AABB", &dbgChunkAABB));

            opts->addGui(new GuiCheckBox("GBuffers", &dbgGBuffers));
            opts->addGui(new GuiCheckBox("Norm & Border", &rde->debugChunkGeo));
            opts->addGui(new GuiCheckBox("glPoly Line", &dbgPolyLine));

            opts->addGui(newLabel("World"));
            opts->addGui(new GuiSlider("View Distance", 0, 16, &rde->viewDistance, 1.0f));

            opts->addGui(new GuiSlider("D/var0", -2, 2, &EntityRenderer::debugVar0, 0.01f));
            opts->addGui(new GuiSlider("D/var1", 0, 4, &EntityRenderer::debugVar1, 0.01f));
            opts->addGui(new GuiSlider("D/var2", 0, 256, &EntityRenderer::debugVar2, 0.1f));

            opts->addGui(new GuiSlider("Brush Size", 0, 16, &Ethertia::getBrushCursor().size, 0.2f));
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

        RenderEngine* rde = Ethertia::getRenderEngine();

        if (dbgBasis) {
            rde->renderDebugBasis();
        }

        if (dbgWorldBasis) {
            rde->renderDebugWorldBasis();
        }

        glPolygonMode(GL_FRONT_AND_BACK, dbgPolyLine ? GL_LINE : GL_FILL);

        if (dbgText) {
            float dt = Ethertia::getDelta();
            std::string dbg_s = Strings::fmt(
                    "cam p: {}, ln: {}\n"
                    "E: {}/{}\n"
                    "dt/ {}, {}fs\n",
                    glm::to_string(Ethertia::getCamera()->position), Ethertia::getCamera()->len, rde->entitiesActualRendered, Ethertia::getWorld()->getEntities().size(),
                    dt, Mth::floor(1.0f/dt));
            Gui::drawString(0, 32, dbg_s, Colors::WHITE, 16, 0, false);
        }

        if (dbgEntityAABB) {
            for (Entity* e : Ethertia::getWorld()->getEntities()) {
                AABB aabb = e->getAABB();
                rde->renderLineBox(aabb.min, aabb.max - aabb.min, Colors::RED);
            }
        }

        if (dbgChunkAABB) { using glm::vec3;
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

        if (dbgCursorRangeInfo) {
            drawCursorRangeInfo();
        }

        if (dbgGBuffers) {
            auto* gbuffer = rde->gbuffer;

            float h = Gui::maxHeight() / 6;
            float w = h * 1.5f;

            rde->guiRenderer->render(0, 0, w, h, Colors::WHITE, gbuffer->texColor[0], 0, FLT_MAX, 1);
            rde->guiRenderer->render(0, h, w, h, Colors::WHITE, gbuffer->texColor[0], 0, FLT_MAX, 2);

            Gui::drawRect(0, h*2, w, h, Colors::WHITE, gbuffer->texColor[1]);

            rde->guiRenderer->render(0, h*3, w, h, Colors::WHITE, gbuffer->texColor[2], 0, FLT_MAX, 1);
            rde->guiRenderer->render(0, h*4, w, h, Colors::WHITE, gbuffer->texColor[2], 0, FLT_MAX, 2);
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

                    Cell& c = Ethertia::getWorld()->getCell(p);
                    Gui::drawWorldpoint(p, [=](glm::vec2 sp) {
                        Gui::drawString(sp.x, sp.y, std::to_string(c.id)+"/"+std::to_string(c.density));
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









    void onKeyboard(int key, bool pressed) override {

        if (pressed) {
            if (key == GLFW_KEY_ESCAPE) {
                GuiRoot* rootGui = Ethertia::getRootGUI();

                Gui* top = rootGui->last();
                if (top != GuiIngame::INST) {
                    rootGui->removeGui(top);
                } else {
                    rootGui->addGui(GuiScreenMainMenu::INST);
                }
            }
        }
    }
};

#endif //ETHERTIA_GUIINGAME_H
