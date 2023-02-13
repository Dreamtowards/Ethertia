//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_GUIINGAME_H
#define ETHERTIA_GUIINGAME_H


#include <ethertia/gui/GuiCommon.h>

#include "GuiScreenMainMenu.h"
#include "GuiDebugV.h"
#include "GuiMessageList.h"
#include <ethertia/gui/inventory/GuiInventory.h>

//#include "GuiScreenChat.h"
//#include "GuiF4Lock.h"
#include <ethertia/util/Loader.h>

class GuiIngame : public GuiCollection
{
public:
    DECL_Inst_(GuiIngame);

    inline static const int HOTBAR_SLOT_MAX = 8;

    static void initGUIs()
    {
        GuiRoot* rt = Ethertia::getRootGUI();

//        rt->addGui(GuiIngame::INST);
//        rt->addGui(GuiScreenPause::INST);

        // default not show Debug
        GuiDebugV::Inst()->setVisible(false);

        rt->addGui(GuiScreenMainMenu::Inst());
    }

    GuiIngame()
    {
        setWidth(Inf);
        setHeight(Inf);

        addGui(GuiDebugV::Inst());

        GuiMessageList::Inst()->addConstraintLTRB(0, Inf, Inf, 24);
        addGui(GuiMessageList::Inst());


    }

    void lateDraw() override
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
                .color = {1,1,1,0.35},
                .tex = TEX_HOTBAR_SLOT_ACTIVE
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
                static Texture* TEX_HEALTH = Loader::loadTexture("gui/health2.png");

                float lv = Ethertia::getPreciseTime() * 0.1f;
                // xp
                float XP_HEIGHT = 12;
                float xpY = hbY - 12 - XP_HEIGHT;
                float xpPercent = Mth::mod(lv, 1.0);
                Gui::drawRect(hbX, xpY, HOTBAR_WIDTH, XP_HEIGHT, {
                        .tex = TEX_XP_BAR,
                        .tex_pos = {0.0, 5/6.0},
                        .tex_size = {1.0, 1/6.0}
                });
                Gui::drawRect(hbX, xpY, HOTBAR_WIDTH*xpPercent, XP_HEIGHT, {
                        .tex = TEX_XP_BAR,
                        .tex_pos = {0.0, 4/6.0},
                        .tex_size = {xpPercent, 1/6.0}
                });
                Gui::drawString(hbX+HOTBAR_WIDTH*0.5, xpY-2, Strings::fmt("{}", int(lv)), Colors::WHITE, 18, {-0.5, 0});


                float health = Ethertia::getPlayer()->m_Health;
                // health
                float HEALTH_SIZE = 16;
                float htY = xpY - 10 - HEALTH_SIZE;
                for (float i = 0; i < Mth::max(health, 10.0f); ++i) {
                    float perc = std::clamp(health - i, 0.0f, 1.0f);

                    float x = hbX + i*(HEALTH_SIZE+2);
                    if (perc < 1.0) {
                        // draw bg.

                        Gui::drawRect(x, htY, HEALTH_SIZE, HEALTH_SIZE, {
                                .tex = TEX_HEALTH,
                                .tex_pos = {0.0, 0.0},
                                .tex_size = {0.5, 1.0}
                        });
                    }
                    Gui::drawRect(x, htY, HEALTH_SIZE * perc, HEALTH_SIZE, {
                            .tex = TEX_HEALTH,
                            .tex_pos = {0.5, 0.0},
                            .tex_size = {0.5 * perc, 1.0}
                    });
                }

                // strain
                // Gui::drawRect(hbX, htY, HEALTH_WIDTH*(0.75), HEALTH_HEIGHT, Colors::alpha(Colors::WHITE, 0.4), nullptr, 6);

            }
        }





        // Center Cursor.
        Gui::drawRect(Gui::maxWidth()/2 -1, Gui::maxHeight()/2 -1,
                      3, 3, Colors::WHITE);


        BrushCursor& cursor = Ethertia::getBrushCursor();
        if (cursor.hit && GuiDebugV::g_BlockMode)
        {
            glm::vec3 pl = glm::floor(cursor.position - cursor.normal*0.2f);
            RenderEngine::drawLineBox(pl, glm::vec3(1.0), Colors::BLACK80);
        }
    }


};

#endif //ETHERTIA_GUIINGAME_H
