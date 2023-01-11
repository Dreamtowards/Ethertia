//
// Created by Dreamtowards on 2022/5/2.
//

#ifndef ETHERTIA_GUISCREENMAINMENU_H
#define ETHERTIA_GUISCREENMAINMENU_H

#include "../GuiSlider.h"
#include "../GuiText.h"
#include "../GuiCollection.h"
#include "../GuiAlign.h"
#include "../GuiStack.h"
#include "../GuiButton.h"

#include <ethertia/render/Camera.h>

class GuiScreenMainMenu : public GuiCollection
{
public:
    inline static GuiScreenMainMenu* INST;

    GuiScreenMainMenu()
    {
        addDrawBackground(Colors::WHITE80);
        setWidthHeight(Inf, Inf);

        GuiStack* vstack = new GuiStack(GuiStack::D_VERTICAL, 16);

        GuiButton* btnSingleplayer = new GuiButton("Singleplayer");
        vstack->addGui(btnSingleplayer);
        btnSingleplayer->addOnClickListener([](auto)
        {
            Ethertia::loadWorld();
        });

        GuiButton* btnMultiplayer = new GuiButton("Multiplayer");
        vstack->addGui(btnMultiplayer);
        btnMultiplayer->addOnClickListener([](auto) {
            Loader::openURL(Loader::showInputBox("URL", "File, Folder, or URL", ""));
        });

        GuiButton* btnSettings = new GuiButton("Settings");
        vstack->addGui(btnSettings);
        btnSettings->addOnClickListener([](auto) {

        });

        GuiButton* btnExit = new GuiButton("Terminate");
        vstack->addGui(btnExit);
        btnExit->addOnClickListener([](auto)
        {
            Ethertia::shutdown();
        });

        addGui(new GuiAlign(0.12, 0.5, vstack));
    }

    void implDraw() override
    {

        Gui::drawString(0, Gui::maxHeight(), "0 mods loaded.\n"
                                                "Ethertia "+Ethertia::Version::name(), Colors::WHITE60, 16,{0.0f, -1.0f});


        Gui::drawString(Gui::maxWidth(), Gui::maxHeight(), "Eldrine Le Prismarine.\n"
                                                  "Copyright Elytra Corporation. Do not distribute!", Colors::WHITE60, 16, {1.0, -1.0f});
    }

    Gui* buildOptItem(const std::string& name, Gui* g) {

        auto* it = new GuiStack(GuiStack::D_HORIZONTAL);

        GuiText* label = new GuiText(name);


        it->addGui(label);
        it->addGui(g);

        return it;
    }
};

#endif //ETHERTIA_GUISCREENMAINMENU_H
