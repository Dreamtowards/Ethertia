//
// Created by Dreamtowards on 2022/12/11.
//

#ifndef ETHERTIA_GUISCREENPAUSE_H
#define ETHERTIA_GUISCREENPAUSE_H

#include <ethertia/gui/GuiText.h>
#include <ethertia/gui/GuiStack.h>
#include <ethertia/gui/GuiCollection.h>

class GuiScreenPause : public GuiCollection
{
public:
    inline static GuiScreenPause* INST;


    GuiScreenPause() {
        setWidth(Inf);
        setHeight(Inf);

        {
            GuiCollection* topbar = new GuiCollection(0, 0, Inf, 16);
            addGui(topbar);
            topbar->addDrawBackground(Colors::BLACK);

            {
                GuiStack* rightbox = new GuiStack(GuiStack::D_HORIZONTAL, 8);
                topbar->addGui(new GuiAlign(1.0, 0.0, rightbox));

                rightbox->addGui(new GuiButton("Ops", false));
                rightbox->addGui(new GuiButton("Ext", false));
            }
        }
        {
            GuiCollection* toolbar = new GuiCollection(0, 16, Inf, 48);
            addGui(toolbar);
            toolbar->addDrawBackground(Colors::BLACK60);

            {
                GuiStack* navbox = new GuiStack(GuiStack::D_HORIZONTAL, 8);
                toolbar->addGui(navbox);
                navbox->setRelativeXY(32, (48-16) / 2);

                navbox->addGui(new GuiButton("Map", false));
                navbox->addGui(new GuiButton("Inventory", false));
                navbox->addGui(new GuiButton("Mails", false));
                navbox->addGui(new GuiButton("Profile", false));
            }
        }
    }

    void onDraw() override {
        float x = getX(), y = getY(), w = getWidth(), h = getHeight();




        Gui::onDraw();
    }
};

#endif //ETHERTIA_GUISCREENPAUSE_H