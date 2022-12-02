//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_GUISCREENMAINMENU_H
#define ETHERTIA_GUISCREENMAINMENU_H

#include "../GuiSlider.h"
#include "../GuiText.h"
#include "../GuiCollection.h"
#include "../GuiAlign.h"
#include "../GuiStack.h"
#include "../GuiButton.h"

#include <ethertia/render/Camera.h>

class GuiScreenMainMenu : public GuiCollection
{
public:
    inline static GuiScreenMainMenu* INST;

    GuiScreenMainMenu()
    {
//        setWidth(Inf);
//        setHeight(Inf);

//        // Options
//        GuiStack* opts = new GuiStack(GuiStack::D_VERTICAL, 5);
//        {
//            opts->setY(100);
//            opts->setWidth(300);
//
//            RenderEngine* rde = Ethertia::getRenderEngine();
//            Camera* cam = Ethertia::getCamera();
//
////            opts->addGui(new GuiSlider("FOV", 15, 165, &rde->fov, 5.0f));
////
////            opts->addGui(new GuiSlider("Camera Smoothness", 0, 5, &cam->smoothness, 0.5f));
////            opts->addGui(new GuiSlider("Camera Roll", -Mth::PI, Mth::PI, &cam->eulerAngles.z));
//
////            opts->addGui(new GuiSlider("View Distance", 0, 16, &rde->viewDistance, 1.0f));
//
////            opts->addGui(new GuiSlider("Fog Density", 0, 0.2f, &rde->chunkRenderer->fogDensity, 0.001f));
////            opts->addGui(new GuiSlider("Fog Gradient", 0, 5, &rde->chunkRenderer->fogGradient, 0.01f));
////
////            opts->addGui(new GuiSlider("ChunkDebugGeo", 0, 1, &rde->debugChunkGeo, 1));
//
//            static float glPolyLine = 0;
//            opts->addGui(new GuiSlider("glPolygonMode Fill/Line", 0, 1, &glPolyLine, 1));
//            glPolygonMode(GL_FRONT_AND_BACK, glPolyLine ? GL_LINE : GL_FILL);
//
//            opts->addDrawBackground(Colors::BLACK10);
//        }
//
//        GuiCollection* lsTopbar = new GuiCollection();
//        {
//            lsTopbar->setWidth(Inf);
//            lsTopbar->setHeight(20);
//
//            {
//                GuiStack* barRight = new GuiStack(GuiStack::D_HORIZONTAL);
//
//
//                auto btnLoadWorld = new GuiButton("Load");
//                btnLoadWorld->setWidth(45);
//                btnLoadWorld->addOnClickListener([opts](OnReleased* e){
//
//                    Ethertia::loadWorld();
//                });
//                barRight->addGui(btnLoadWorld);
//
//                auto btnUnloadWorld = new GuiButton("Unload");
//                btnUnloadWorld->setWidth(45);
//                btnUnloadWorld->addOnClickListener([opts](OnReleased* e){
//
//                    Ethertia::unloadWorld();
//                });
//                barRight->addGui(btnUnloadWorld);
//
//                auto btnOpts = new GuiButton("Opts");
//                btnOpts->setWidth(45);
//                btnOpts->addOnClickListener([opts](OnReleased* e){
//
//                    opts->setVisible(!opts->isVisible());
//                });
//                barRight->addGui(btnOpts);
//
//                auto btnExit = new GuiButton("Exit");
//                btnExit->setWidth(45);
//                barRight->addGui(btnExit);
//
//                lsTopbar->addGui(new GuiAlign(1.0f, 0.0f, barRight));
//            }
//            lsTopbar->addDrawBackground(Colors::BLACK10);
//        }
//
//        addGui(lsTopbar);
//        addGui(opts);
    }

    Gui* buildOptItem(const std::string& name, Gui* g) {

        auto* it = new GuiStack(GuiStack::D_HORIZONTAL);

        GuiText* label = new GuiText(name);


        it->addGui(label);
        it->addGui(g);

        return it;
    }
};

#endif //ETHERTIA_GUISCREENMAINMENU_H
