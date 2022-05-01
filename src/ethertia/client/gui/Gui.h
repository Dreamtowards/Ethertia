//
// Created by Dreamtowards on 2022/4/28.
//

#ifndef ETHERTIA_GUI_H
#define ETHERTIA_GUI_H

#include <vector>

#include <ethertia/util/Mth.h>
#include <ethertia/util/QuickTypes.h>
#include <ethertia/util/Collections.h>
#include <ethertia/event/EventBus.h>
#include <ethertia/client/render/Texture.h>
#include <ethertia/util/Colors.h>

class Gui
{
    float x = 0;
    float y = 0;
    float width = 0;
    float height = 0;

    Gui* parent = nullptr;
    std::vector<Gui*> children;

    glm::vec2 childbound{0};

    bool hovered = false;
    bool pressed = false;
    bool focused = false;  // useful for TextBox etc.
    bool enable = true;    // effect to onClick().

    bool visible = true;
    bool clipping = false;

    void* tag = nullptr;

protected:
    EventBus eventbus;
    // Events: OnClick, OnHoverChanged, OnPressChanged, OnFocusChanged, OnAttachChanged, OnVisibleChanged
    // OnMouseIn/Out, OnDraw, OnLayout

    // cachedAbsXY?
    // 1. active: update-child-cachedpos when setPos
    // 2. lazy: Child-CachedPosInvalid, lazy getXY ToUpdate

public:
    static constexpr float NaN = Mth::NaN;
    static constexpr float Inf = Mth::Inf;

    class OnHover{};
    class OnFocus {};
    class OnPressed {};
    class OnReleased {};
    class OnClick {};

    Gui() {}

    Gui(float _rx, float _ry) : x(_rx), y(_ry) {}

    Gui(float _rx, float _ry, float _w, float _h) : x(_rx), y(_ry), width(_w), height(_h) {}

    ////////////// SIZES //////////////

    float getRelativeX() const { return x; }
    float getRelativeY() const { return y; }

    void setRelativeX(float _rx) {
        if (_rx == x) return;
        x = _rx;
    }
    void setRelativeY(float _ry) {
        if (_ry == y) return;
        y = _ry;
    }
    void setRelativeXY(float _rx, float _ry) {
        setRelativeX(_rx);
        setRelativeY(_ry);
    }

    float getX() const {
        if (!parent) return x;
        return parent->getX() + x;
    }
    float getY() const {
        if (!parent) return y;
        return parent->getY() + y;
    }

    void setX(float _x) {
        float px = parent ? parent->getX() : 0;
        setRelativeX(_x - px);
    }
    void setY(float _y) {
        float py = parent ? parent->getY() : 0;
        setRelativeY(_y - py);
    }

    float getWidth() const {
        if (!visible) return 0;
        if (width == -Inf) return childbound.x;
        if (width == Inf) return parent->getWidth();  // sub x?
        return width;
    }
    void setWidth(float w) {
        width = w;
    }
    float getHeight() const {
        if (!visible) return 0;
        if (height == -Inf) return childbound.y;
        if (height == Inf) return parent->getHeight();  // sub y?
        return height;
    }
    void setHeight(float h) {
        height = h;
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
        // broadcastEvent OnAttached
        // requestLayout
    }
    void addGui(Gui* g) {
        addGui(g, count());
    }
    void addGuis(const std::initializer_list<Gui*>& ls) {
        for (Gui* g : ls) {
            addGui(g);
        }
    }

    void removeGui(uint idx) {
        auto it = children.begin()+idx;
        children.erase(it);
        (*it)->parent = nullptr;
        // broadcastEvent OnDetached ??
    }
    void removeGui(Gui* g) {
        removeGui(Collections::indexOf(children, g));
    }
    void removeLastGui() {
        removeGui(count()-1);
    }
    void removeAllGuis() {
        while (count()) {
            removeLastGui();
        }
    }

    void setGui(uint idx, Gui* g) {
        removeGui(idx);
        addGui(g, idx);
    }


    static void forParents(Gui* g, const std::function<void(Gui*)>& visitor) {
        visitor(g);

        while ((g=g->parent)) {
            visitor(g);
        }
    }

    static void forChildren(Gui* g, const std::function<void(Gui*)>& visitor) {
        visitor(g);

        for (Gui* child : g->children) {
            Gui::forChildren(child, visitor);
        }
    }




    bool isHover() const { return hovered; }
    void setHover(bool h) {
        if (h == hovered) return;
        hovered = h;

        fireEvent<OnHover>();
    }

    bool isPressed() { return pressed; }
    void setPressed(bool p) {
        if (p == pressed) return;
        pressed = p;

        if (p) fireEvent<OnPressed>();
        else   fireEvent<OnReleased>();
    }

    bool isFocused() const { return focused; }
    void setFocused(bool f) {
        if (f == focused) return;
        focused = f;

        fireEvent<OnFocus>();
    }




    template<typename E>
    void fireEvent() {
        E e{};
        eventbus.post(&e);
    }

    template<typename E>
    void broadcastEvent() {
        E e{};
        Gui::forChildren(this, [&e](Gui* g) {
            g->eventbus.post(&e);
        });
    }

    ////////////// ?? //////////////

    bool isPointOver(float px, float py) {
        float x = getX(), y = getY();
        return px >= x && px < x + getWidth() &&
               py >= y && py < y + getHeight();
    }
    bool isPointOver(const glm::vec2& p) {
        return isPointOver(p.x, p.y);
    }
    bool isMouseOver();

    static Gui* pointing(Gui* g, glm::vec2 p) {
        if (!g->isPointOver(p))
            return nullptr;
        for (int i = g->count()-1; i >= 0; --i) {
            if (Gui* r = pointing(g->getGui(i), p))
                return r;
        }
        // except GuiAlign. which just an adjuster regardless self-size/interaction.
        return g;
    }


    template<typename LAMBDA, typename T = EventBus::InternalUtil::GetLambdaEventType<LAMBDA>>
    T* exec(LAMBDA l) {
        l.operator()((T*)this);
        return (T*)this;
    }


    virtual void onDraw()
    {
        for (Gui* g : children)
        {
            g->onDraw();
        }
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




    static void drawRect(float x, float y, float w, float h, glm::vec4 color,
                         Texture* tex =Texture::UNIT,
                         float round =0);


    static void drawString(float x, float y, const std::string& str,
                           glm::vec4 color =Colors::WHITE,
                           float textHeight =16,
                           float align =0,
                           bool drawShadow =true);


// drawViewpoint(glm::vec3 worldpos);
// drawCornerStretchTexture
// drawRectBorder
};

#endif //ETHERTIA_GUI_H
