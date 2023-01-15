//
// Created by Dreamtowards on 2023/1/15.
//

#ifndef ETHERTIA_GUIINVENTORY_H
#define ETHERTIA_GUIINVENTORY_H

#include <ethertia/gui/GuiCommon.h>

#include <ethertia/item/Inventory.h>

class GuiInventory : public Gui
{
public:
    inline static const int SLOT_GAP = 4,   // gap between slots
                            SLOT_SIZE = 48,
                            ROW_SLOTS = 8;

    Inventory* m_Inventory = nullptr;

    GuiInventory(Inventory* inv) : m_Inventory(inv) {

        // wrapSlots.
        float sz = SLOT_GAP+SLOT_SIZE;
        int n = inv->size();
        setWidthHeight(
                std::max(0.0f, sz * std::min(n, ROW_SLOTS) - SLOT_GAP),
                std::max(0.0f, sz * std::ceil((float)n / (float)ROW_SLOTS) - SLOT_GAP)
        );
    }

    void implDraw() override
    {

        float beginX = getX();
        float x = beginX;
        float y = getY();
        for (int i = 0; i < m_Inventory->size(); ++i)
        {
            bool hover = Gui::isCursorOver(x,y, SLOT_SIZE, SLOT_SIZE);
            Gui::drawRect(x,y,SLOT_SIZE,SLOT_SIZE, hover ? Colors::WHITE60 : Colors::WHITE10);

            x += SLOT_SIZE + SLOT_GAP;

            if (i != 0 && i % ROW_SLOTS == 0) {
                x = beginX;
                y += SLOT_SIZE + SLOT_GAP;
            }
        }
    }

};

#endif //ETHERTIA_GUIINVENTORY_H
