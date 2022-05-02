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
    GuiScreenMainMenu()
    {
        setWidth(Inf);
        setHeight(Inf);

        addGui((new GuiAlign(0.3, 0.5))->setContent(
            (new GuiStack(0, 1, 0, 10))->addGuis({
                new GuiButton("Load"),
                new GuiButton("Options"),
                new GuiButton("Exit"),
            })
        ));
    }

};

#endif //ETHERTIA_GUISCREENMAINMENU_H
