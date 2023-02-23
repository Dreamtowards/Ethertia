//
// Created by Dreamtowards on 2022/12/11.
//

#ifndef ETHERTIA_GUISCREENPAUSE_H
#define ETHERTIA_GUISCREENPAUSE_H

#include <ethertia/gui/GuiCommon.h>

#include <ethertia/entity/player/EntityPlayer.h>
#include <ethertia/gui/inventory/GuiInventory.h>
#include <ethertia/world/World.h>
#include <ethertia/item/recipe/Recipe.h>

class GuiScreenPause : public GuiCollection
{
public:
    DECL_Inst_(GuiScreenPause);

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
                topbar->addGui(rightbox);
                rightbox->addConstraintAlign(1.0, 0.0);

                float topbarTextHeight = 13;

//                rightbox->addGui(new GuiButton("@Player483", false));
//                rightbox->addGui(new GuiButton("$100.00", false));

                GuiButton* btnClock = new GuiButton("        ", false, topbarTextHeight, 4);
                rightbox->addGui(btnClock);
                btnClock->addPreDraw([btnClock](auto) {
                    if (!Ethertia::getWorld())
                        return;
                    btnClock->m_Text = Strings::daytime(Ethertia::getWorld()->getDayTime(), true);
                });

//                GuiButton* btnSignal = new GuiButton("  ");
//                rightbox->addGui(btnSignal);
//                btnSignal->addPreDraw([](Gui* g) {
//                    // 40:30
//                    static Texture* TEX_SIGNAL = Loader::loadTexture("gui/signals.png");
//
//                    float w = 24, h = 18;
//                    Gui::drawRect(g->getX() + (g->getWidth() - w) * 0.5, g->getY(), w, h, {
//                        .tex = TEX_SIGNAL,
//                        .tex_pos = {0, 0},
//                        .tex_size = {1/2.0f, 1/6.0f}
//                    });
//                });
//                rightbox->addGui(new GuiButton("offline"));

                GuiButton* btnSettings = new GuiButton("Settings", false, topbarTextHeight, 4);
                rightbox->addGui(btnSettings);
                btnSettings->addOnClickListener([](auto)
                {
                    Ethertia::getRootGUI()->addGui(GuiScreenSettings::Inst());
                });

                GuiButton* btnExitWorld = new GuiButton("Quit", false, topbarTextHeight, 4);
                rightbox->addGui(btnExitWorld);
                btnExitWorld->addOnClickListener([](auto)
                {
                    Ethertia::unloadWorld();
                });
            }
            {
                GuiStack* leftbox = new GuiStack(GuiStack::D_HORIZONTAL, 2);
                topbar->addGui(leftbox);
                leftbox->addConstraintAlign(0, 0.5);

                leftbox->addGui(new GuiButton("Map", false));
                leftbox->addGui(new GuiButton("Inventory", false));
                leftbox->addGui(new GuiButton("Mail", false));
                leftbox->addGui(new GuiButton("Calendar", false));
            }

            {
                GuiStack* leftbox = new GuiStack(GuiStack::D_HORIZONTAL, 2);
                topbar->addGui(leftbox);
                leftbox->addConstraintAlign(1.0, 0.5);

                leftbox->addGui(new GuiButton("$100.00", false));
                leftbox->addGui(new GuiButton("@Dreamtowards", false));
            }
        }

//        {
//            GuiStack* toolbar = new GuiStack(GuiStack::D_HORIZONTAL, 8);
//            addGui(toolbar);
//            toolbar->setY(32);
//            toolbar->addConstraintAlign(0.5, Inf);
//
//            toolbar->addGui(new GuiButton("Map", false));
//            toolbar->addGui(new GuiButton("Inventory", false));
//            toolbar->addGui(new GuiButton("Mails", false));
//
//            toolbar->addPreDraw([toolbar](auto) {
//                for (Gui* g : toolbar->children()) {
//                    bool isCurrent = g == toolbar->at(1);
//
//                    GuiInventory::drawSimpleInvBackground(g->xywh(), "", 4, glm::vec4{isCurrent ? 0.35f : 0.0f});
//                }
//            });
//        }

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
            static Inventory allItemsInventory(60);

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

            GuiInventory::drawSimpleInvBackground({-16, -16, Gui::maxWidth()+32, 48+16});
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

        //Gui::drawString(Gui::maxWidth()/2, 0, "Inventory", Colors::WHITE, 18, {-0.5, 0});


        {
            float x = 100, y = 100, w = 300;

            // Craft Recipes
            int i = 0;
            for (auto& it : Recipe::REGISTRY) {
                Recipe* r = it.second;
                float iy = y + (i*40);
                
                Gui::drawString(x,  iy+ 12, r->m_Produce->getRegistryId());

                Gui::drawString(x+w, iy+40,
                                Strings::join(", ", [&](int i){ return r->m_Source[i]->getRegistryId(); }, r->m_Source.size()),
                                Colors::WHITE,
                                13,
                                {-1, -1});

                ++i;
            }
        }


    }
};

#endif //ETHERTIA_GUISCREENPAUSE_H
