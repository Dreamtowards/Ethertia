//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_GUIINGAME_H
#define ETHERTIA_GUIINGAME_H

#include "GuiScreenMainMenu.h"
#include "GuiScreenPause.h"
#include "GuiScreenChat.h"

#include <ethertia/gui/GuiCommon.h>

#include "GuiDebugV.h"
#include "GuiMessageList.h"

class GuiIngame : public GuiCollection
{
public:
    inline static GuiIngame* INST;


    GuiIngame()
    {
        setWidth(Inf);
        setHeight(Inf);

        GuiDebugV::INST = new GuiDebugV();
        GuiMessageList::INST = new GuiMessageList();

        addGui(GuiDebugV::INST);
        addGui(new GuiAlign(Inf, Inf, GuiMessageList::INST, 0, Inf, Inf, 24));


    }



};

#endif //ETHERTIA_GUIINGAME_H
