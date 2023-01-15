//
// Created by Dreamtowards on 2022/12/11.
//

#ifndef ETHERTIA_GUISCREENPAUSE_H
#define ETHERTIA_GUISCREENPAUSE_H

#include <ethertia/gui/GuiText.h>
#include <ethertia/gui/GuiStack.h>
#include <ethertia/gui/GuiCollection.h>
#include <ethertia/gui/screen/GuiIngame.h>

#include <ethertia/entity/player/EntityPlayer.h>
#include <ethertia/gui/inventory/GuiInventory.h>
#include <ethertia/world/World.h>

class GuiScreenPause : public GuiCollection
{
public:
    inline static GuiScreenPause* INST;

    inline static const float TOP_WIDTH = 820;

    inline static Texture* TEX_FRAME = nullptr;
    inline static Texture* TEX_FRAME_ONC = nullptr;

    static void reloadTexture() {

        TEX_FRAME = Loader::loadTexture("gui/frame.png");
        TEX_FRAME_ONC = Loader::loadTexture("gui/frame_onec.png");
    }

    GuiScreenPause() {
        setWidth(Inf);
        setHeight(Inf);

        {
            GuiCollection* topbar = new GuiCollection(0, 0, TOP_WIDTH, 64);
            addGui(new GuiAlign(0.5, 0, topbar));

            {
                GuiStack* rightbox = new GuiStack(GuiStack::D_HORIZONTAL, 8);
                topbar->addGui(new GuiAlign(1.0, 0.0, rightbox));

                rightbox->addGui(new GuiButton("Settings", false));

                GuiButton* btnExitWorld = new GuiButton("Quit", false);
                rightbox->addGui(btnExitWorld);
                btnExitWorld->addOnClickListener([](auto)
                {
                    Ethertia::unloadWorld();
                });
            }
            {
                GuiStack* leftbox = new GuiStack(GuiStack::D_HORIZONTAL, 2);
                topbar->addGui(leftbox);

                leftbox->addGui(new GuiButton("@Player483", false));
                leftbox->addGui(new GuiButton("$100.00", false));
            }
        }

        {
            GuiStack* toolbar = new GuiStack(GuiStack::D_HORIZONTAL, 4);
            addGui(toolbar);
            toolbar->setY(32);
            toolbar->addConstraintAlign(0.5, Inf);

            toolbar->addGui(new GuiButton("Map", false));
            toolbar->addGui(new GuiButton("Inventory", false));
            toolbar->addGui(new GuiButton("Mails", false));
        }

        {
            GuiInventory* gInventory = new GuiInventory(&Ethertia::getPlayer()->m_Inventory);
            addGui(gInventory);
            gInventory->addConstraintAlign(0.5, 0.4);
            gInventory->addPreDraw([](Gui* g)
            {
                Gui::drawStretchCorners(Gui::grow(g->xywh(), 50), TEX_FRAME_ONC, 48, true);
            });
        }

    }

    void implDraw() override
    {
        // background dark
        Gui::drawRect(0, 0, Gui::maxWidth(), Gui::maxHeight(), Colors::BLACK50);

        // topbar
        {
            float topbarX = (Gui::maxWidth()-TOP_WIDTH) / 2.0;
//            Gui::drawRect(topbarX, 0-16, TOP_WIDTH, 64+16, Colors::GRAY, nullptr, 12);
//            Gui::drawRect(topbarX, 0, TOP_WIDTH, 16, Colors::BLACK30);
            Gui::drawStretchCorners({topbarX - 32, 0 - 64, TOP_WIDTH + 64, 64 + 64 + 16}, TEX_FRAME, 48);
//            Gui::drawRect(0, 0, Gui::maxWidth(), 64, Colors::GRAY);
//            Gui::drawRect(0, 0, Gui::maxWidth(), 16, Colors::BLACK30);
        }

        Gui::drawString(Gui::maxWidth()/2, 0, "Inventory", Colors::WHITE, 18, {-0.5, 0});




    }
};

#endif //ETHERTIA_GUISCREENPAUSE_H
