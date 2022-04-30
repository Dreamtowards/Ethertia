//
// Created by Dreamtowards on 2022/4/28.
//

#ifndef ETHERTIA_GUI_H
#define ETHERTIA_GUI_H

#include <vector>

#include <ethertia/util/Mth.h>
#include <ethertia/util/QuickTypes.h>
#include <ethertia/util/Collections.h>


class Gui
{
    float x = 0;
    float y = 0;
    float width = 0;
    float height = 0;

    Gui* parent;
    std::vector<Gui*> children;

    glm::vec2 childbound;

    bool hovered = false;
    bool pressed = false;
    bool focused = false;  // useful for TextBox etc.
    bool enable = true;    // effect to onClick().

    bool visible = true;
    bool clipping = false;

    void* tag;

    // eventbus?
    // Events: OnClick, OnHoverChanged, OnPressChanged, OnFocusChanged, OnAttachChanged, OnVisibleChanged
    // OnMouseIn/Out, OnDraw, OnLayout


public:
    static constexpr float NaN = Mth::NaN;
    static constexpr float Inf = Mth::Inf;


    ////////////// SIZES //////////////

    float getRelativeX() const { return x; }
    float getRelativeY() const { return y; }

    float getX() const {
        if (!parent) return x;
        return parent->getX() + x;
    }
    float getY() const {
        if (!parent) return y;
        return parent->getY() + y;
    }

    float getWidth() const {
        if (!visible) return 0;
        if (width == -Inf) return childbound.x;
        if (width == Inf) return parent->getWidth();  // sub x?
        return width;
    }
    float getHeight() const {
        if (!visible) return 0;
        if (height == -Inf) return childbound.y;
        if (height == Inf) return parent->getHeight();  // sub y?
        return height;
    }

    static float maxWidth();
    static float maxHeight();


    ////////////// CHILDREN //////////////

    Gui* getParent() const {
        return parent;
    }
    uint count() const {
        return children.size();
    }
    Gui* getGui(uint idx) {
        return children[idx];
    }

    void addGui(Gui* g, uint idx) {
        if (g->parent) throw std::logic_error("Cannot attach. it belongs to another.");
        children.push_back(g);
        children.insert(children.begin()+idx, g);
        g->parent = this;
        // fireEvent OnAttached
        // requestLayout
    }
    void addGui(Gui* g) {
        addGui(g, count());
    }

    void removeGui(uint idx) {
        auto it = children.begin()+idx;
        children.erase(it);
        (*it)->parent = nullptr;
        // fireEvent OnDetached
    }
    void removeGui(Gui* g) {
        removeGui(Collections::indexOf(children, g));
    }
    void removeAllGuis() {
        while (count()) {
            removeGui(count()-1);
        }
    }

    void setGui(uint idx, Gui* g) {
        removeGui(idx);
        addGui(g, idx);
    }



    ////////////// ?? //////////////

    bool isPointOver(float px, float py) {
        float x = getX(), y = getY();
        return px >= x && px < x + getWidth() &&
               py >= y && py < y + getHeight();
    }
    bool isMouseOver();




    void onDraw()
    {

    }


    void onLayout()
    {

    }

    void _update_childbound() {
        if (width != -Inf && height != -Inf)
            return;
        glm::vec2 mx(0);
        for (auto* g : children) {
            mx.x = Mth::max(mx.x, g->getRelativeX() + g->getWidth());  // what if child size is MATCH_PARENT? then should ignore size but consider pos, or throw logical exception
            mx.y = Mth::max(mx.y, g->getRelativeY() + g->getHeight());
        }
        childbound = mx;
    }

    void fireEvent() {

    }

};

#endif //ETHERTIA_GUI_H
