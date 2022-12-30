//
// Created by Dreamtowards on 2022/12/30.
//

#ifndef ETHERTIA_GUIMESSAGELIST_H
#define ETHERTIA_GUIMESSAGELIST_H

#include <ethertia/gui/GuiCommon.h>

class GuiMessageList : public GuiCollection
{
public:
    inline static GuiMessageList* INST = nullptr;

    GuiStack* m_MsgList = nullptr;

    double m_TimeLastMessage = -100;  // negative seconds init.

    GuiMessageList() {
        setWidthHeight(840, 320);

        m_MsgList = new GuiStack(GuiStack::D_VERTICAL);
        m_MsgList->setWidth(Inf);

        {
            GuiScrollBox* scroll = new GuiScrollBox();
            scroll->setContent(m_MsgList);
            scroll->setWidthHeight(Inf, Inf);

            addGui(scroll);
        }
    }

    void addMessage(const std::string& msg) {
        GuiText* g = new GuiText(msg);

        m_MsgList->addGui(g);

        m_TimeLastMessage = Ethertia::getPreciseTime();
    }

    void implDraw() override {
        float x = getX(), y = getY(), w = getWidth(), h = getHeight();

        Gui::drawRect(x,y,w,h, Colors::BLACK40);

        const float DEACTIVATE_SEC = 4;
        if (Ethertia::getRootGUI()->last() != GuiScreenChat::INST && Ethertia::getPreciseTime() - m_TimeLastMessage > DEACTIVATE_SEC) {
            setVisible(false);
        }
    }


};

#endif //ETHERTIA_GUIMESSAGELIST_H
