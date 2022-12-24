//
// Created by Dreamtowards on 2022/8/22.
//

#ifndef ETHERTIA_GUISCREENCHAT_H
#define ETHERTIA_GUISCREENCHAT_H

#include <utility>

#include "../GuiCollection.h"
#include "../GuiStack.h"
#include "../GuiTextBox.h"
#include "../GuiAlign.h"

#include <ethertia/entity/player/EntityPlayer.h>

class GuiScreenChat : public GuiCollection
{
public:

    inline static GuiScreenChat* INST;

    GuiStack* chatlist = nullptr;


    GuiScreenChat() {
        setWidth(Inf);
        setHeight(Inf);

        GuiStack* vbox = new GuiStack(GuiStack::D_VERTICAL);
        vbox->setWidth(500);

        {
            chatlist = new GuiStack(GuiStack::D_VERTICAL);

            chatlist->addDrawBackground(Colors::BLACK10);
            chatlist->setWidth(Inf);
            chatlist->setHeight(300);
            vbox->addGui(chatlist);
        }

        vbox->addGui(new Gui(0, 0, 0, 8));  // gap

        GuiTextBox* input = new GuiTextBox("");
        input->setWidth(Inf);
        input->setHeight(28);
        input->setSingleLine(true);
        input->setFocused(true);
        input->addOnReturnListener([=](GuiTextBox::OnReturn* e) {
            const std::string& line = input->getText();

            Ethertia::dispatchCommand(line);

//                std::string msg = Strings::fmt("<{}>: {}", Ethertia::getPlayer()->name, line);
//                appendMessage(msg);

            Ethertia::getRootGUI()->removeGui(this);  // dismiss.
            input->setText("");
        });
        vbox->addGui(input);

        addGui(new GuiAlign(0, 1, vbox));

    }

    void appendMessage(std::string msg) {
        GuiText* g = new GuiText(std::move(msg));

        chatlist->addGui(g);
    }
};

#endif //ETHERTIA_GUISCREENCHAT_H
