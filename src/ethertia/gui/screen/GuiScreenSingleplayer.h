//
// Created by Dreamtowards on 2023/1/25.
//

#ifndef ETHERTIA_GUISCREENSINGLEPLAYER_H
#define ETHERTIA_GUISCREENSINGLEPLAYER_H

#include <ethertia/gui/GuiCommon.h>
#include <ethertia/gui/screen/GuiScreenNewWorld.h>

#include <ethertia/util/Loader.h>

class GuiScreenSingleplayer : public GuiCollection
{
public:
    inline static GuiScreenSingleplayer* INST = nullptr;

    inline static Gui* SELECTED_WORLD = nullptr;

    GuiScreenSingleplayer() {
        setWidthHeight(Inf, Inf);

        GuiStack* lsBtnWorldOps = new GuiStack(GuiStack::D_VERTICAL, 8);
        addGui(lsBtnWorldOps);

        GuiButton* btnOpenWorld = new GuiButton("Open World", false);
        lsBtnWorldOps->addGui(btnOpenWorld);
        btnOpenWorld->addOnClickListener([](auto) {

            Ethertia::loadWorld("saves/world-2");
        });

        GuiButton* btnNewWorld = new GuiButton("New World", false);
        lsBtnWorldOps->addGui(btnNewWorld);
        btnNewWorld->addOnClickListener([](auto) {

            Ethertia::getRootGUI()->addGui(GuiScreenNewWorld::Inst());
        });

        GuiButton* btnModifyWorld = new GuiButton("Modify World", false);
        lsBtnWorldOps->addGui(btnModifyWorld);

        GuiButton* btnDeleteWorld = new GuiButton("Delete World", false);
        lsBtnWorldOps->addGui(btnDeleteWorld);
        btnDeleteWorld->addOnClickListener([](auto) {
            Loader::showMessageBox("Delete World", "Are you sure to delete world?");

        });

        GuiButton* btnRefresh = new GuiButton("Refresh", false);
        lsBtnWorldOps->addGui(btnRefresh);
        btnRefresh->addOnClickListener([](auto){
            SELECTED_WORLD = nullptr;
        });


        GuiStack* lsWorlds = new GuiStack(GuiStack::D_VERTICAL, 8);
        addGui(lsWorlds);
        lsWorlds->addConstraintAlign(0.5, 0.25);

        refreshWorldList(lsWorlds);




        addPreDraw([=](Gui* g) {
            drawOptionsBackground();

            bool btnEnable = SELECTED_WORLD != nullptr;
            btnOpenWorld->m_Enable = btnEnable;
            btnModifyWorld->m_Enable = btnEnable;
            btnDeleteWorld->m_Enable = btnEnable;

            // Layout bind
            lsBtnWorldOps->setX(lsWorlds->getX()-200);
            lsBtnWorldOps->setY(lsWorlds->getY());

//            if (g->isPressed()) {  // click otherwise to cancel select. this should execute before select.
//                SELECTED_WORLD = nullptr;
//            }
        });
    }


    void refreshWorldList(GuiStack* lsWorlds) {

        for (const auto& savedir : std::filesystem::directory_iterator("saves/"))
        {
            if (!savedir.is_directory())
                continue;

            std::string fileWorldInfo = Strings::fmt("{}/{}", savedir.path().string(), "world.dat");
            std::ifstream file(fileWorldInfo, std::ios::in);
            if (!file)
                continue;
            auto tagWorld = nbt::io::read_compound(file).second;
            file.close();

            long dirsize = Loader::calcDirectorySize(savedir.path());
            std::string desc = Strings::fmt("{} | {}", savedir.path(), Strings::size_str(dirsize));

            std::string worldName = (std::string)tagWorld->at("Name");
            //int64_t savedTime = (int64_t)tagWorld->at("SavedTime");


            GuiCollection* itemWorld = new GuiCollection(0, 0, 500, 60);
            lsWorlds->addGui(itemWorld);

            itemWorld->addPreDraw([=](Gui* g) {
                if (g->isPressed() || g->isHover()) {
                    Gui::drawRect({
                                          .xywh = g->xywh(),
                                          .color = g->isPressed() ? Colors::BLACK40 : Colors::BLACK10
                                  });
                    if (g->isPressed()) {
                        SELECTED_WORLD = itemWorld;
                    }
                }
                // Item Border.
                Gui::drawRect({
                                      .xywh = g->xywh(),
                                      .color = SELECTED_WORLD == itemWorld ? Colors::WHITE80 : Colors::WHITE20,
                                      .border = 2
                              });
                float x = g->getX() + 12, y = g->getY() + 12;

                Gui::drawString(x, y, worldName);
                Gui::drawString(x, y+20, desc, Colors::GRAY, 15);
            });
        }
    }

};

#endif //ETHERTIA_GUISCREENSINGLEPLAYER_H
