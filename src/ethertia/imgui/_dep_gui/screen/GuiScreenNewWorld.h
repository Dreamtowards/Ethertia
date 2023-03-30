//
// Created by Dreamtowards on 2023/1/25.
//

#ifndef ETHERTIA_GUISCREENNEWWORLD_H
#define ETHERTIA_GUISCREENNEWWORLD_H


#include <ethertia/gui/GuiCommon.h>

#include <ethertia/world/World.h>

class GuiScreenNewWorld : public GuiCollection
{
public:

    static GuiScreenNewWorld* Inst() {
        static GuiScreenNewWorld* INST = new GuiScreenNewWorld();
        return INST;
    }

    GuiTextBox* m_txSeed = nullptr;
    GuiTextBox* m_txWorldName = nullptr;

    GuiScreenNewWorld() {
        setWidthHeight(Inf, Inf);

        GuiStack* vStack = new GuiStack(GuiStack::D_VERTICAL, 24);
        addGui(vStack);
        vStack->setWidth(426);
        vStack->addConstraintAlign(0.5, 0.22);

        m_txWorldName = new GuiTextBox("New World");
        vStack->addGui(m_txWorldName);
        m_txWorldName->setWidth(Inf);
        m_txWorldName->m_Caption = "World Name";

        vStack->addGui(new Gui(0, 8));


        m_txSeed = new GuiTextBox("");
        vStack->addGui(m_txSeed);
        m_txSeed->m_Caption = "Seed";
        m_txSeed->m_Hint = "Random";

        GuiButton* btnCreate = new GuiButton("Create World");
        vStack->addGui(btnCreate);
        btnCreate->setWidth(Inf);
        btnCreate->setHeight(32);
        btnCreate->addOnClickListener([this](auto)
        {
            WorldInfo worldinfo{
                .Seed = seed(),
                .Name = m_txWorldName->getText()
            };
            Log::info("Creating world '{}' seed {}.", worldinfo.Name, worldinfo.Seed);
            Ethertia::loadWorld(save_path(), &worldinfo);
        });

//        GuiButton* btnOpGamemode = new GuiButton("Gamemode: Creative");
//        vStack->addGui(btnOpGamemode);

//        vStack->addGui(new GuiButton("Advanced options..."));

        addPreDraw([](Gui* g) {
            drawOptionsBackground("New World");

        });
    }

    uint64_t seed() {
        std::string text = m_txSeed->getText();
        if (text.empty())
            return Timer::timestampMillis();

        try {
            return std::stoll(text);
        } catch (std::invalid_argument err) {
            Log::info("Not a number seed, use string hash instead.");

            return std::hash<std::string>()(text);
        }
    }

    std::string save_path() {
        return "./saves/" + m_txWorldName->getText();
    }

    void implDraw() override
    {

        Gui::drawString(m_txWorldName->getX(),
                        m_txWorldName->getY() + m_txWorldName->getHeight() + 2,
                        Strings::fmt("Will be saved at: {}", save_path()),
                        Colors::WHITE60, 12);
    }

};

#endif //ETHERTIA_GUISCREENNEWWORLD_H
