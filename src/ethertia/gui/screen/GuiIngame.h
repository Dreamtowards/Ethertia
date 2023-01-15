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

    inline static const int HOTBAR_SLOT_MAX = 8;

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


        {
            // hotbar
            const float HOTBAR_WIDTH = 720 * 0.8,
                        HOTBAR_HEIGHT = 80 * 0.8;

            float hbX = (Gui::maxWidth() - HOTBAR_WIDTH) / 2;
            float hbY = Gui::maxHeight() - HOTBAR_HEIGHT - 24 - 8;

            static Texture *TEX_HOTBAR_SLOTS = Loader::loadTexture("gui/hotbar_slots2.png");
            static Texture *TEX_HOTBAR_SLOT_ACTIVE = Loader::loadTexture("gui/hotbar_slot_active5.png");

            int hotbarIdx = Ethertia::getPlayer()->m_HotbarSlot;
            Gui::drawRect(hbX, hbY, HOTBAR_WIDTH, HOTBAR_HEIGHT, TEX_HOTBAR_SLOTS);
            Gui::drawRect(hbX + hotbarIdx * HOTBAR_HEIGHT, hbY, HOTBAR_HEIGHT, HOTBAR_HEIGHT, {
                .tex = TEX_HOTBAR_SLOT_ACTIVE,
                .color = {1,1,1,0.35}
            });

            //float hbItemWG = HOTBAR_WIDTH / HOTBAR_SLOT_MAX;
            float hbItemH = 48;

            for (int i = 0; i < HOTBAR_SLOT_MAX; ++i)
            {
                GuiInventory::drawItemStack(hbX + i*HOTBAR_HEIGHT,
                                            hbY + (HOTBAR_HEIGHT-hbItemH)*0.5,
                                            Ethertia::getPlayer()->m_Inventory.at(i));
            }


            if (player->getGamemode() == Gamemode::SURVIVAL)
            {
                static Texture* TEX_XP_BAR = Loader::loadTexture("gui/bars.png");
                static Texture* TEX_HEALTH = Loader::loadTexture("gui/health.png");

                float lv = Ethertia::getPreciseTime() * 0.1;
                // xp
                float XP_HEIGHT = 14;
                float xpY = hbY - 12 - XP_HEIGHT;
                float xpPercent = Mth::mod(lv, 1.0);
                Gui::drawRect(hbX, xpY, HOTBAR_WIDTH, XP_HEIGHT, {
                        .tex = TEX_XP_BAR,
                        .tex_size = {1.0, 1/6.0},
                        .tex_pos = {0.0, 5/6.0}
                });
                Gui::drawRect(hbX, xpY, HOTBAR_WIDTH*xpPercent, XP_HEIGHT, {
                        .tex = TEX_XP_BAR,
                        .tex_size = {xpPercent, 1/6.0},
                        .tex_pos = {0.0, 4/6.0}
                });
                Gui::drawString(hbX+HOTBAR_WIDTH*0.5, xpY-2, Strings::fmt("{}", int(lv)), Colors::WHITE, 18, {-0.5, 0});


                float health = Ethertia::getPlayer()->m_Health;
                // health
                float HEALTH_SIZE = 20;
                float htY = xpY - 10 - HEALTH_SIZE;
                for (int i = 0; i < Mth::max(health/2.0, 10.0); ++i) {
                    bool none = health < i*2;
                    bool half = health < i*2 + 0.5;

                    Gui::drawRect(hbX + i*(HEALTH_SIZE+2), htY, HEALTH_SIZE, HEALTH_SIZE, {
                            .tex = TEX_HEALTH,
                            .tex_size = {1/3.0, 1.0},
                            .tex_pos = {none ? 2/3.0 : half ? 1/3.0 : 0.0, 0.0}
                    });
                }

                // strain
                // Gui::drawRect(hbX, htY, HEALTH_WIDTH*(0.75), HEALTH_HEIGHT, Colors::alpha(Colors::WHITE, 0.4), nullptr, 6);

            }
        }





        // Center Cursor.
        Gui::drawRect(Gui::maxWidth()/2 -1, Gui::maxHeight()/2 -1,
                      3, 3, Colors::WHITE);
    }


};

#endif //ETHERTIA_GUIINGAME_H
