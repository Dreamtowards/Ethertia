//
// Created by Dreamtowards on 2022/12/11.
//

#ifndef ETHERTIA_GUISCREENPAUSE_H
#define ETHERTIA_GUISCREENPAUSE_H

#include <ethertia/gui/GuiText.h>
#include <ethertia/gui/GuiStack.h>
#include <ethertia/gui/GuiCollection.h>

#include <ethertia/world/World.h>

class GuiScreenPause : public GuiCollection
{
public:
    inline static GuiScreenPause* INST;


    GuiScreenPause() {
        setWidth(Inf);
        // setHeight(Inf);

        {
            GuiCollection* topbar = new GuiCollection(0, 0, Inf, 16);
            addGui(topbar);
            topbar->addDrawBackground(Colors::R);

            {
                GuiStack* rightbox = new GuiStack(GuiStack::D_HORIZONTAL, 8);
                topbar->addGui(new GuiAlign(1.0, 0.0, rightbox));

                rightbox->addGui(new GuiButton("Ops", false));

                GuiButton* btnNewWorld = new GuiButton("New", false);
                rightbox->addGui(btnNewWorld);
                btnNewWorld->addOnClickListener([](auto ) {
                    Ethertia::loadWorld();
                });

                GuiButton* btnExitWorld = new GuiButton("Ext", false);
                rightbox->addGui(btnExitWorld);
                btnExitWorld->addOnClickListener([](auto){
                    Ethertia::unloadWorld();
                });
            }
            {
                GuiStack* navbox = new GuiStack(GuiStack::D_HORIZONTAL, 2);
                topbar->addGui(navbox);
                navbox->setRelativeXY(12, 0);//(48-16) / 2);

                navbox->addGui(new GuiButton("Map", false));
                navbox->addGui(new GuiButton("Inventory", false));
//                navbox->addGui(new GuiButton("Mails", false));
                navbox->addGui(new GuiButton("Profile", false));
            }
        }
//        {
//            GuiCollection* leftbar = new GuiCollection(0, 16, Inf, 48);
//            addGui(leftbar);
//
//
//        }
    }

    void onDraw() override {
        float x = getX(), y = getY(), w = getWidth(), h = getHeight();




        Gui::onDraw();
    }
};

#endif //ETHERTIA_GUISCREENPAUSE_H
