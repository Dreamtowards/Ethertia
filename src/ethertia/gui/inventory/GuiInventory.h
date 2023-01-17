//
// Created by Dreamtowards on 2023/1/15.
//

#ifndef ETHERTIA_GUIINVENTORY_H
#define ETHERTIA_GUIINVENTORY_H

#include <ethertia/gui/GuiCommon.h>

#include <ethertia/item/Inventory.h>
#include <ethertia/init/ItemTextures.h>
#include <ethertia/init/Items.h>

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


    static void drawSlotBorder(glm::vec4 xywh) {
        DECL_XYWH;

        Gui::drawRect(x,y,w,h, {
            .color = {1,1,1,0.25},
            .border = 3
        });
    }

    static void drawSimpleInvBackground(glm::vec4 xywh, const std::string& title = "", float padding = 16, glm::vec4 bg = {0.173, 0.157, 0.224, 0.9}) {
        xywh = Gui::grow(xywh, padding);
        if (!title.empty()) {
            xywh = Gui::grow(xywh, 0, padding+16, 0, 0);
        }

        drawRect({
            .xywh = xywh,
            .color = bg
        });
        drawSlotBorder(xywh);

        if (!title.empty()) {
            drawString(xywh.x+padding, xywh.y+padding, title);
        }
    }

    inline static const ItemStack* HOVER_ITEM = nullptr;

    static void drawItemStack(float x, float y, const ItemStack& stack, float size = SLOT_SIZE)
    {
        if (stack.empty())
            return;

        float n = Item::REGISTRY.size();
        float i = Collections::find(Item::REGISTRY.m_Ordered, (Item*)stack.item());
        Gui::drawRect(x,y,size,size, {
            .tex = ItemTextures::ITEM_ATLAS,
            .tex_size = {1.0/n, 1.0},
            .tex_pos  = {i/n, 0.0}
        });

        Gui::drawString(x+size,y+size, std::to_string(stack.m_Amount), Colors::WHITE, 15, {-1.0, -1.0});

        if (!HOVER_ITEM && Gui::isCursorOver(x,y,size,size)) {
            HOVER_ITEM = &stack;
        }
    }

    static void doDrawHoveredItem() {
        const ItemStack* stack = HOVER_ITEM;
        if (!stack)
            return;
        const Item& item = *stack->item();

        float x = Gui::cursorX() + 22, y = Gui::cursorY() - 32, w = 128, h = w +18+8+16;

        drawSimpleInvBackground({x,y,w,h}, "", 12, Colors::BLACK80);

        drawItemStack(x,y, *stack, w);

        drawString(x,y+w+4, item.getRegistryId(), Colors::WHITE, 17);

        std::stringstream info;
        if (item.hasComponent<ItemComponentMaterial>()) {
            info << "material\n";
        }
        if (item.hasComponent<Item::ComponentFood>()) {
            info << "food\n";
        }
        if (item.hasComponent<Item::ComponentTool>()) {
            info << "tool\n";
        }
        drawString(x,y+w+4+20, info.str(), Colors::GRAY, 15);
    }
    static void doDrawPickingItem() {

        if (PICKING_ITEM) {

            drawItemStack(Gui::cursorX()-SLOT_SIZE/2, Gui::cursorY()-SLOT_SIZE/2, *PICKING_ITEM);
        }
    }

    inline static ItemStack* PICKING_ITEM = nullptr;

    void implDraw() override
    {
        bool mlbPressed = Ethertia::getWindow()->isMouseDown(GLFW_MOUSE_BUTTON_LEFT);

        float beginX = getX();
        float x = beginX;
        float y = getY();
        for (int i = 0; i < m_Inventory->size(); ++i)
        {
            if (i != 0 && i % ROW_SLOTS == 0) {
                x = beginX;
                y += SLOT_SIZE + SLOT_GAP;
            }

            bool hover = Gui::isCursorOver(x,y, SLOT_SIZE, SLOT_SIZE);
            Gui::drawRect(x,y,SLOT_SIZE,SLOT_SIZE, hover ? Colors::WHITE60 : Colors::WHITE10);

            drawSlotBorder({x,y,SLOT_SIZE,SLOT_SIZE});

            ItemStack& stack = m_Inventory->at(i);

            if (!mlbPressed && PICKING_ITEM && hover)
            {
                if (stack.empty()) {
                    PICKING_ITEM->moveTo(stack);
                    PICKING_ITEM = nullptr;
                }
            }

            if (!stack.empty() && !PICKING_ITEM && hover && mlbPressed)
            {
                PICKING_ITEM = &stack;
            }

            if (!stack.empty() && PICKING_ITEM!=&stack)
            {


                drawItemStack(x,y, stack);
            }


            x += SLOT_SIZE + SLOT_GAP;
        }
    }

};

#endif //ETHERTIA_GUIINVENTORY_H
