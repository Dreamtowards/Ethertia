//
// Created by Dreamtowards on 2023/1/25.
//

#ifndef ETHERTIA_GUISCREENNEWWORLD_H
#define ETHERTIA_GUISCREENNEWWORLD_H


#include <ethertia/gui/GuiCommon.h>

class GuiScreenNewWorld : public GuiCollection
{
public:

    static GuiScreenNewWorld* Inst() {
        static GuiScreenNewWorld* INST = new GuiScreenNewWorld();
        return INST;
    }

    GuiScreenNewWorld() {
        setWidthHeight(Inf, Inf);

        GuiStack* vStack = new GuiStack(GuiStack::D_VERTICAL, 8);
        addGui(vStack);
        vStack->setWidth(500);
        vStack->addConstraintAlign(0.5, 0.25);

        GuiTextBox* textWorldName = new GuiTextBox("New World");
        vStack->addGui(textWorldName);
        textWorldName->setWidth(Inf);


        GuiButton* btnOpGamemode = new GuiButton("Gamemode: Creative");
        vStack->addGui(btnOpGamemode);

        GuiButton* btnCreate = new GuiButton("Create!");
        vStack->addGui(btnCreate);
        btnCreate->setWidth(Inf);

        vStack->addGui(new GuiButton("Advanced options..."));

        vStack->addGui(new GuiTextBox("Seed"));

        addPreDraw([](Gui* g) {
            drawOptionsBackground();

        });
    }

};

#endif //ETHERTIA_GUISCREENNEWWORLD_H
