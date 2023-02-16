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
    static GuiScreenSingleplayer* Inst() {
        static GuiScreenSingleplayer* INST = new GuiScreenSingleplayer();
        return INST;
    }

    std::string m_SelectedWorldPath;

    GuiStack* m_WorldList = nullptr;

    GuiScreenSingleplayer() {
        setWidthHeight(Inf, Inf);

        GuiStack* lsBtnWorldOps = new GuiStack(GuiStack::D_VERTICAL, 8);
        addGui(lsBtnWorldOps);

        GuiButton* btnOpenWorld = new GuiButton("Open World", false);
        lsBtnWorldOps->addGui(btnOpenWorld);
        btnOpenWorld->addOnClickListener([this](auto) {

            Ethertia::loadWorld(m_SelectedWorldPath);
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
        btnRefresh->addOnClickListener([this](auto){
            m_SelectedWorldPath = "";

            refreshWorldList();
        });


        m_WorldList = new GuiStack(GuiStack::D_VERTICAL, 8);
        addGui(m_WorldList);
        m_WorldList->addConstraintAlign(0.5, 0.25);

        refreshWorldList();




        addPreDraw([=](Gui* g) {
            drawOptionsBackground("Singleplayer");

            bool btnEnable = !m_SelectedWorldPath.empty();
            btnOpenWorld->m_Enable = btnEnable;
            btnModifyWorld->m_Enable = btnEnable;
            btnDeleteWorld->m_Enable = btnEnable;

            // Layout bind
            lsBtnWorldOps->setX(m_WorldList->getX()-200);
            lsBtnWorldOps->setY(m_WorldList->getY());

//            if (g->isPressed()) {  // click otherwise to cancel select. this should execute before select.
//                SELECTED_WORLD = nullptr;
//            }
        });
    }


    void refreshWorldList() {
        m_WorldList->removeAllGuis();  // delete/clean?

        if (!Loader::fileExists("saves/")) {

            return;
        }

        for (const auto& savedir : std::filesystem::directory_iterator("saves/"))
        {
            if (!savedir.is_directory())
                continue;
            const std::string& dir = savedir.path().string();

            std::string fileWorldInfo = Strings::fmt("{}/{}", dir, "world.dat");
            std::ifstream file(fileWorldInfo, std::ios::in);
            if (!file)
                continue;
            auto tagWorld = nbt::io::read_compound(file).second;
            file.close();

            long dirsize = Loader::calcDirectorySize(savedir.path().string());
            std::string desc = Strings::fmt("{} | {}", Strings::size_str(dirsize), savedir.path());

            std::string worldName = (std::string)tagWorld->at("Name");
            //int64_t savedTime = (int64_t)tagWorld->at("SavedTime");


            GuiCollection* itemWorld = new GuiCollection(0, 0, 500, 60);
            m_WorldList->addGui(itemWorld);

            itemWorld->addPreDraw([=](Gui* g) {
                if (g->isPressed() || g->isHover()) {
                    Gui::drawRect({
                                          .xywh = g->xywh(),
                                          .color = g->isPressed() ? Colors::BLACK40 : Colors::BLACK10
                                  });
                    if (g->isPressed()) {
                        m_SelectedWorldPath = dir;
                    }
                }
                // Item Border.
                Gui::drawRect({
                                      .xywh = g->xywh(),
                                      .color = m_SelectedWorldPath == dir ? Colors::WHITE80 : Colors::WHITE20,
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
