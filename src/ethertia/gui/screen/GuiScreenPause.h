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
//    inline static Texture* TEX_FRAME_ONC = nullptr;

    static void reloadTexture() {

        TEX_FRAME = Loader::loadTexture("gui/hotbar_slot_ex1.png");//"gui/frame.png");
//        TEX_FRAME_ONC = Loader::loadTexture("gui/frame_onec.png");
    }

    GuiScreenPause() {
        setWidth(Inf);
        setHeight(Inf);

        GuiCollection* topbar = new GuiCollection(0, 0, TOP_WIDTH, 64);
        addGui(topbar);
        topbar->addConstraintAlign(0.5, 0);
        {
            {
                GuiStack* rightbox = new GuiStack(GuiStack::D_HORIZONTAL, 8);
                topbar->addGui(new GuiAlign(1.0, 0.0, rightbox));

                GuiButton* btnSignal = new GuiButton("  ");
                rightbox->addGui(btnSignal);
                btnSignal->addPreDraw([](Gui* g) {
                    // 40:30
                    static Texture* TEX_SIGNAL = Loader::loadTexture("gui/signals.png");

                    float w = 24, h = 18;
                    Gui::drawRect(g->getX() + (g->getWidth() - w) * 0.5, g->getY(), w, h, {
                        .tex = TEX_SIGNAL,
                        .tex_size = {1/2.0f, 1/6.0f},
                        .tex_pos = {0, 0}
                    });
                });

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

                GuiButton* btnClock = new GuiButton("        ", true);
                leftbox->addGui(btnClock);
                btnClock->addPreDraw([btnClock](auto) {
                    if (!Ethertia::getWorld())
                        return;
                    float worldtime = Ethertia::getWorld()->m_DayTime;
                    float hr = worldtime * 24;
                    float hr_apm = std::fmod(hr, 12);
                    float min = Mth::frac(hr) * 60;
                    std::string s = Strings::fmt("{}:{} {}", (int)hr_apm, (int)min, worldtime > 0.5 ? "PM" : "AM");
                    btnClock->m_Text = s;
                });
            }
        }

        {
            GuiStack* toolbar = new GuiStack(GuiStack::D_HORIZONTAL, 8);
            addGui(toolbar);
            toolbar->setY(32);
            toolbar->addConstraintAlign(0.5, Inf);

            toolbar->addGui(new GuiButton("Map", false));
            toolbar->addGui(new GuiButton("Inventory", false));
            toolbar->addGui(new GuiButton("Mails", false));

            toolbar->addPreDraw([toolbar](auto) {
                for (Gui* g : toolbar->children()) {
                    bool isCurrent = g == toolbar->at(1);

                    GuiInventory::drawSimpleInvBackground(g->xywh(), "", 4, glm::vec4{isCurrent ? 0.35f : 0.0f});
                }
            });
        }

        {
            GuiInventory* gInventory = new GuiInventory(&Ethertia::getPlayer()->m_Inventory);
            addGui(gInventory);
            gInventory->addConstraintAlign(0.5, 0.8);
            gInventory->addPreDraw([](Gui* g)
            {
                GuiInventory::drawSimpleInvBackground(g->xywh(), "Inventory");
            });
        }

        {
            static Inventory allItemsInventory(40);

            GuiInventory* gAllItemsInventory = new GuiInventory(&allItemsInventory);
            addGui(gAllItemsInventory);
            gAllItemsInventory->addConstraintAlign(0.5, 0.35);
            gAllItemsInventory->addPreDraw([](Gui* g)
            {
                GuiInventory::drawSimpleInvBackground(g->xywh(), "Creative Repository");
            });
            for (auto& it : Item::REGISTRY)
            {
                ItemStack stack(it.second, 100);
                allItemsInventory.putItemStack(stack);
            }
        }


        addPreDraw([topbar](auto)
        {
            // background dark
            Gui::drawRect(0, 0, Gui::maxWidth(), Gui::maxHeight(), Colors::BLACK50);

            GuiInventory::drawSimpleInvBackground(topbar->xywh());
        });
    }

    void implDraw() override
    {

        // topbar
        {
//            float topbarX = (Gui::maxWidth()-TOP_WIDTH) / 2.0;
//            Gui::drawRect(topbarX, 0-16, TOP_WIDTH, 64+16, Colors::GRAY, nullptr, 12);
//            Gui::drawRect(topbarX, 0, TOP_WIDTH, 16, Colors::BLACK30);
//            Gui::drawStretchCorners({topbarX - 32, 0 - 64, TOP_WIDTH + 64, 64 + 64 + 16}, TEX_FRAME, 48);
//            Gui::drawRect(0, 0, Gui::maxWidth(), 64, Colors::GRAY);
//            Gui::drawRect(0, 0, Gui::maxWidth(), 16, Colors::BLACK30);
        }

        Gui::drawString(Gui::maxWidth()/2, 0, "Inventory", Colors::WHITE, 18, {-0.5, 0});




    }
};

#endif //ETHERTIA_GUISCREENPAUSE_H
