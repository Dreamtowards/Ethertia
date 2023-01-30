//
// Created by Dreamtowards on 2023/1/29.
//

#ifndef ETHERTIA_GUISCREENSETTINGS_H
#define ETHERTIA_GUISCREENSETTINGS_H

#include <ethertia/gui/GuiCommon.h>

class GuiScreenSettings : public GuiCollection
{
public:
    static GuiScreenSettings* Inst() {
        static GuiScreenSettings* INST = new GuiScreenSettings();
        return INST;
    }


    GuiScreenSettings()
    {
        setWidthHeight(Inf, Inf);


        GuiCollection* boxSettings = new GuiCollection();
        addGui(boxSettings);
        boxSettings->setWidthHeight(600, 800);
        boxSettings->addDrawBackground(Colors::BLACK10);
        boxSettings->setY(80);
        boxSettings->addConstraintAlign(0.5, Inf);

        initOpGeneral();
        boxSettings->addGui(opGeneral);


        GuiStack* navsSettings = new GuiStack(GuiStack::D_VERTICAL, 8);
        navsSettings->setWidth(150);
        addGui(navsSettings);
        {
            navsSettings->addGui(new GuiButton("Profile", false));

            navsSettings->addGui(new GuiButton("Current World", false));

            navsSettings->addGui(new GuiSeparator());

            GuiButton* btnGraphics = new GuiButton("Graphics", false);
            navsSettings->addGui(btnGraphics);

            navsSettings->addGui(new GuiButton("Sounds & Music", false));

            navsSettings->addGui(new GuiButton("Controls", false));
            navsSettings->addGui(new GuiButton("Language", false));

            // navsSettings->addGui(new GuiButton("Accessibility", false));

            navsSettings->addGui(new GuiSeparator());

            navsSettings->addGui(new GuiButton("Mods", false));
            navsSettings->addGui(new GuiButton("Shaders", false));
            navsSettings->addGui(new GuiButton("Resource Packs", false));

            navsSettings->addGui(new GuiSeparator());

//            navsSettings->addGui(new GuiButton("Bug Tracker'", false));
//            navsSettings->addGui(new GuiButton("Linkages & Credits", false));

            navsSettings->addGui(new GuiButton("Misc.", false));  // Miscellaneous
        }

        addPreDraw([=, this](auto)
        {
            Gui::drawOptionsBackground();
            Gui::drawString(Gui::getWidth()/2, 32, "Settings", Colors::WHITE, 16, {-0.5, 0});

            navsSettings->setX(boxSettings->getX() - 200);
            navsSettings->setY(boxSettings->getY());
        });
    }

    GuiText* textCaptain(const std::string& text) {
        GuiText* g = new GuiText(text, Colors::WHITE60, 13, false);
        g->m_TextY = 40;
        g->setHeight(40+13+8);
        return g;
    }


    GuiStack* opGeneral = new GuiStack(GuiStack::D_VERTICAL);

    void initOpGeneral() {
        opGeneral->setWidthHeight(Inf, Inf);
        opGeneral->addConstraintLTRB(20, Inf, 20, Inf);

        opGeneral->addGui(textCaptain("Profile"));

        GuiCollection* boxProfile = new GuiCollection();
        opGeneral->addGui(boxProfile);
        boxProfile->setWidthHeight(Inf, 35);

        GuiButton* btnName = new GuiButton("Dreamtowards", false, 20, 0);
        boxProfile->addGui(btnName);
        btnName->addOnClickListener([](auto){

            Loader::openURL("https://ethertia.com/profile/5d2db1fc-4d29-304e-8de8-4879f69fd8d5");
        });

        GuiText* txDesc = new GuiText("ref.dreamtowards@gmail.com", Colors::GRAY, 15);
        boxProfile->addGui(txDesc);
        txDesc->setRelativeY(btnName->m_TextHeight+4);

        //opGeneral->addGui(new Gui(0, 0, 0, 8));
        {
            GuiStack* opsProfile = new GuiStack(GuiStack::D_HORIZONTAL, 4);
            boxProfile->addGui(opsProfile);
            opsProfile->addConstraintAlign(1, 0);

            opsProfile->addGui(new GuiButton("-"));
            opsProfile->addGui(new GuiButton("+"));
            opsProfile->addGui(new GuiButton("@Prismarine"));
            opsProfile->addGui(new GuiButton("@Eldaria"));
        }

        opGeneral->addGui(textCaptain("Character Customization"));
    }

};

#endif //ETHERTIA_GUISCREENSETTINGS_H
