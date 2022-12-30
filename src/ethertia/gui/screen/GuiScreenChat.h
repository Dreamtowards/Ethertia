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


    GuiScreenChat() {
        setWidth(Inf);
        setHeight(Inf);



        GuiTextBox* input = new GuiTextBox("");
        input->setWidth(Inf);
        input->setHeight(20);
        input->m_TextY = (input->getHeight() - input->getTextHeight()) / 2;
        input->addDrawBackground(Colors::BLACK80);
        input->setSingleLine(true);
        input->setFocused(true);
        input->addOnReturnListener([this, input](GuiTextBox::OnReturn* e) {
            const std::string& line = input->getText();

            Ethertia::dispatchCommand(line);

//                std::string msg = Strings::fmt("<{}>: {}", Ethertia::getPlayer()->name, line);
//                appendMessage(msg);

            Ethertia::getRootGUI()->removeGui(this);  // dismiss.
            input->setText("");
        });

        addGui(new GuiAlign(0, 1, input));

    }

};

#endif //ETHERTIA_GUISCREENCHAT_H
