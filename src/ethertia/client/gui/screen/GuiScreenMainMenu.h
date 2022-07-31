//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_GUISCREENMAINMENU_H
#define ETHERTIA_GUISCREENMAINMENU_H

#include "../GuiCollection.h"
#include "../GuiAlign.h"
#include "../GuiStack.h"
#include "../GuiButton.h"

class GuiScreenMainMenu : public GuiCollection
{
public:
    inline static GuiScreenMainMenu* INST;

    GuiScreenMainMenu()
    {
        setWidth(Inf);
        setHeight(Inf);

        addGui((new GuiAlign(0, 1))->setContent((new GuiStack(GuiStack::D_HORIZONTAL))->addGuis({
                new GuiButton("LD"),
                new GuiButton("Opts"),
            })
        ));
    }

};

#endif //ETHERTIA_GUISCREENMAINMENU_H
