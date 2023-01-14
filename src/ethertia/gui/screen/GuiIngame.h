//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_GUIINGAME_H
#define ETHERTIA_GUIINGAME_H


#include <ethertia/gui/GuiCommon.h>

#include "GuiScreenMainMenu.h"
#include "GuiScreenPause.h"
#include "GuiScreenChat.h"

#include "GuiDebugV.h"
#include "GuiMessageList.h"
#include "GuiF4Lock.h"

#include <ethertia/util/Loader.h>

class GuiIngame : public GuiCollection
{
public:
    inline static GuiIngame* INST;

    static void initGUIs()
    {
        GuiRoot* rt = Ethertia::getRootGUI();

        GuiDebugV::INST = new GuiDebugV();
        GuiMessageList::INST = new GuiMessageList();

        GuiIngame::INST = new GuiIngame();

        GuiScreenMainMenu::INST = new GuiScreenMainMenu();
        GuiScreenChat::INST = new GuiScreenChat();
        GuiScreenPause::INST = new GuiScreenPause();

        GuiF4Lock::INST = new GuiF4Lock();

//        rt->addGui(GuiIngame::INST);
//        rt->addGui(GuiScreenPause::INST);

        rt->addGui(GuiScreenMainMenu::INST);
    }

    GuiIngame()
    {
        setWidth(Inf);
        setHeight(Inf);

        addGui(GuiDebugV::INST);
        addGui(new GuiAlign(Inf, Inf, GuiMessageList::INST, 0, Inf, Inf, 24));


    }

    void implDraw() override
    {

        EntityPlayer* player = Ethertia::getPlayer();



        // hotbar
        const float HOTBAR_WIDTH = 720 * 0.8,
                    HOTBAR_HEIGHT = 80 * 0.8;

        float hbX = (Gui::maxWidth() - HOTBAR_WIDTH) / 2;
        float hbY = Gui::maxHeight() - HOTBAR_HEIGHT - 24;

//        Gui::drawRect(hbX, hbY, HOTBAR_WIDTH, HOTBAR_HEIGHT, Colors::alpha(Colors::WHITE, 0.5), nullptr, 8, 2);
//        Gui::drawRect(hbX, hbY, HOTBAR_WIDTH, HOTBAR_HEIGHT, Colors::alpha(Colors::BLACK, 0.24), nullptr, 8);

        static Texture* TEX_HOTBAR_SLOTS = Loader::loadTexture("gui/hotbar_slots2.png");
        static Texture* TEX_HOTBAR_SLOT_ACTIVE = Loader::loadTexture("gui/hotbar_slot_active2.png");

        int hotbarIdx = Ethertia::getPlayer()->m_HotbarSlot;
        Gui::drawRect(hbX, hbY, HOTBAR_WIDTH, HOTBAR_HEIGHT, TEX_HOTBAR_SLOTS);
        Gui::drawRect(hbX + hotbarIdx*HOTBAR_HEIGHT, hbY, HOTBAR_HEIGHT, HOTBAR_HEIGHT, TEX_HOTBAR_SLOT_ACTIVE);

        if (player->getGamemode() == Gamemode::SURVIVAL)
        {
            // xp
//            float XP_HEIGHT = 6;
//            float xpY = hbY - 5 - XP_HEIGHT;
//            Gui::drawRect(hbX, xpY, HOTBAR_WIDTH, XP_HEIGHT, Colors::BLACK60, nullptr, 3);
//            Gui::drawRect(hbX, xpY, HOTBAR_WIDTH*0.32f, XP_HEIGHT, Colors::GREEN_DARK, nullptr, 3);

            // health
//            float HEALTH_HEIGHT = 12;
//            float HEALTH_WIDTH = 200;
//            float htY = xpY - 5 - HEALTH_HEIGHT;
//            Gui::drawRect(hbX, htY, HEALTH_WIDTH, HEALTH_HEIGHT, Colors::BLACK60, nullptr, 4);
//            Gui::drawRect(hbX, htY, HEALTH_WIDTH*(player->m_Health), HEALTH_HEIGHT, Colors::RED, nullptr, 4);

            // strain
            // Gui::drawRect(hbX, htY, HEALTH_WIDTH*(0.75), HEALTH_HEIGHT, Colors::alpha(Colors::WHITE, 0.4), nullptr, 6);

        }




        // Center Cursor.
        Gui::drawRect(Gui::maxWidth()/2 -1, Gui::maxHeight()/2 -1,
                      3, 3, Colors::WHITE);
    }


};

#endif //ETHERTIA_GUIINGAME_H
