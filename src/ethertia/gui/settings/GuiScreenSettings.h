//
// Created by Dreamtowards on 2023/1/29.
//

#ifndef ETHERTIA_GUISCREENSETTINGS_H
#define ETHERTIA_GUISCREENSETTINGS_H

#include <ethertia/gui/GuiCommon.h>

class GuiScreenSettings : public GuiCollection
{
public:
    DECL_Inst_(GuiScreenSettings);

    GuiCollection* m_SettingsBox;

    GuiScreenSettings()
    {
        setWidthHeight(Inf, Inf);


        m_SettingsBox = new GuiCollection();
        addGui(m_SettingsBox);
        m_SettingsBox->setWidthHeight(600, 800);
        m_SettingsBox->addDrawBackground(Colors::BLACK10);
        m_SettingsBox->setY(80);
        m_SettingsBox->addConstraintAlign(0.5, Inf);



        GuiStack* navsSettings = new GuiStack(GuiStack::D_VERTICAL, 8);
        navsSettings->setWidth(150);
        addGui(navsSettings);
        {
            GuiButton* btnProfile = new GuiButton("Profile", false);
            navsSettings->addGui(btnProfile);
            btnProfile->addOnClickListener([this](auto) {
                switchOptionGui(_GetOpProfile());
            });

            GuiButton* btnCurrentWorld = new GuiButton("Current World", false);
            navsSettings->addGui(btnCurrentWorld);
            btnCurrentWorld->addOnClickListener([this](auto) {
                switchOptionGui(_GetOpCurrentWorld());
            });

            navsSettings->addGui(new GuiSeparator());

            GuiButton* btnGraphics = new GuiButton("Graphics", false);
            navsSettings->addGui(btnGraphics);
            btnGraphics->addOnClickListener([this](auto) {
                switchOptionGui(_GetOpGraphics());
            });

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

            navsSettings->setX(m_SettingsBox->getX() - 200);
            navsSettings->setY(m_SettingsBox->getY());
        });
    }

    static GuiText* textCaptain(const std::string& text) {
        GuiText* g = new GuiText(text, Colors::WHITE60, 13, false);
        g->m_TextY = 40;
        g->setHeight(40+13+8);
        return g;
    }

    void switchOptionGui(Gui* g)
    {
        if (m_SettingsBox->count() > 0)
            m_SettingsBox->removeLastGui();
        m_SettingsBox->addGui(g);
    }



    static Gui* _GetOpProfile() {
        static GuiStack* op = nullptr;
        if (op) return op;

        op = new GuiStack(GuiStack::D_VERTICAL);

        op->setWidthHeight(Inf, Inf);
        op->addConstraintLTRB(20, Inf, 20, Inf);

        op->addGui(textCaptain("Profile"));

        GuiCollection* boxProfile = new GuiCollection();
        op->addGui(boxProfile);
        boxProfile->setWidthHeight(Inf, 35);

        GuiButton* btnName = new GuiButton("Dreamtowards", false, 20, 0);
        boxProfile->addGui(btnName);
        btnName->addOnClickListener([](auto)
        {
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

        op->addGui(textCaptain("Character Customization"));
        return op;
    }

    static Gui* _GetOpCurrentWorld() {
        static GuiStack* op = nullptr;
        if (op) return op;
        op = new GuiStack(GuiStack::D_VERTICAL);
        op->setWidthHeight(Inf, Inf);
        op->addConstraintLTRB(20, Inf, 20, Inf);

        op->addGui(textCaptain("Current World"));

        op->addGui(new Gui(0, 0, 0, 14));
        GuiTextBox* txWorldName = new GuiTextBox("*Current World Name");
        op->addGui(txWorldName);
        txWorldName->m_Caption = "Name";

        op->addGui(new Gui(0, 0, 0, 14));
        GuiTextBox* txSeed = new GuiTextBox("364823746283746234");
        op->addGui(txSeed);
        txSeed->m_Caption = "Seed";


        return op;
    }

    static Gui* _GetOpGraphics() {
        static GuiStack* op = nullptr;
        if (op) return op;
        op = new GuiStack(GuiStack::D_VERTICAL);
        op->setWidthHeight(Inf, Inf);
        op->addConstraintLTRB(20, Inf, 20, Inf);

        op->addGui(textCaptain("General Graphics"));

        static bool a;
        op->addGui(new GuiCheckBox("Shadow Mapping", &a));

        op->addGui(new GuiCheckBox("SSAO", &a));



        return op;
    }

};

#endif //ETHERTIA_GUISCREENSETTINGS_H
